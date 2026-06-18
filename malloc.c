#include "malloc.h"
#include <string.h>

/* Global malloc manager instance */
static t_malloc_manager g_manager = {
	.tiny_zones = NULL,
	.small_zones = NULL,
	.large_blocks = NULL,
	.mutex = PTHREAD_MUTEX_INITIALIZER
};

static bool g_initialized = false;
static __thread bool g_in_malloc = false;

/* Initialize the malloc manager */
void init_malloc_manager(void)
{
	if (!g_initialized)
	{
		pthread_mutex_init(&g_manager.mutex, NULL);
		g_manager.tiny_zones = NULL;
		g_manager.small_zones = NULL;
		g_manager.large_blocks = NULL;
		g_initialized = true;
	}
}

t_malloc_manager *get_malloc_manager(void)
{
	if (!g_initialized)
		init_malloc_manager();
	return &g_manager;
}

/**
 * get_block_header - Récupère l'en-tête de métadonnées d'un bloc alloué
 * @ptr: Pointeur vers la mémoire alloué par malloc
 * 
 * Retourne:
 *   - Pointeur vers la structure t_block juste avant le pointeur utilisable
 *   - NULL si ptr est NULL
 * 
 * Description:
 *   Chaque bloc alloué est précédé d'une structure t_block contenant:
 *   - size: taille du bloc utilisable
 *   - is_free: état du bloc (libre ou utilisé)
 *   - next/prev: pointeurs pour la liste chaînée
 */
t_block *get_block_header(void *ptr)
{
	if (!ptr)
		return NULL;
	return (t_block *)((char *)ptr - sizeof(t_block));
}

/**
 * create_zone - Crée une nouvelle zone de mémoire mappée
 * @zone_size: Taille de la zone à créer
 * 
 * Retourne:
 *   - Pointeur vers la nouvelle zone créée
 *   - NULL si l'appel mmap échoue
 * 
 * Description:
 *   - Utilise mmap() pour allouer zone_size bytes
 *   - Initialise la structure t_zone au début de la zone
 *   - Crée un bloc initial libre occupant tout l'espace disponible
 *   - Utilisé pour les allocations TINY (16KB) et SMALL (64KB)
 */
t_zone *create_zone(size_t zone_size)
{
	t_zone *zone;
	void *mapped;

	mapped = mmap(NULL, zone_size, PROT_READ | PROT_WRITE,
				  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mapped == MAP_FAILED)
		return NULL;

	zone = (t_zone *)mapped;
	zone->zone_size = zone_size;
	zone->block_count = 0;
	zone->next = NULL;
	zone->blocks = (t_block *)((char *)zone + ALIGN(sizeof(t_zone)));

	/* Initialize first block */
	zone->blocks->size = zone_size - ALIGN(sizeof(t_zone)) - sizeof(t_block);
	zone->blocks->is_free = true;
	zone->blocks->next = NULL;
	zone->blocks->prev = NULL;

	return zone;
}

/**
 * find_free_block - Cherche un bloc libre suffisamment grand dans une chaîne de zones
 * @zone: Pointeur vers la première zone de la chaîne
 * @size: Taille minimale du bloc recherché
 * 
 * Retourne:
 *   - Pointeur vers le premier bloc libre de taille >= size
 *   - NULL si aucun bloc libre trouvé
 * 
 * Description:
 *   - Parcourt toutes les zones chaînées
 *   - Pour chaque zone, parcourt tous les blocs
 *   - Retourne le premier bloc libre pouvant accueillir size bytes
 *   - Utilise la stratégie "first fit"
 */
t_block *find_free_block(t_zone *zone, size_t size)
{
	t_block *current;

	while (zone)
	{
		current = zone->blocks;
		while (current)
		{
			if (current->is_free && current->size >= size)
				return current;
			current = current->next;
		}
		zone = zone->next;
	}
	return NULL;
}

/**
 * split_block - Fragmente un bloc en deux si la taille restante le permet
 * @block: Pointeur vers le bloc à fragmenter
 * @size: Taille à assigner au premier bloc
 * 
 * Description:
 *   - Calcule la taille restante après allocation
 *   - Si restant >= ALIGNMENT: crée un nouveau bloc avec l'espace restant
 *   - Sinon: n'effectue aucune fragmentation (bloc trop petit)
 *   - Maintient la chaîne doublement liée (next/prev)
 *   
 * Exemple:
 *   Avant: [Bloc libre 256B]
 *   Après split(block, 64): [Bloc 64B] -> [Bloc libre 192B - sizeof(t_block)]
 */
void split_block(t_block *block, size_t size)
{
	t_block *new_block;
	size_t remaining;

	remaining = block->size - size - sizeof(t_block);
	if (remaining < ALIGNMENT)
		return;

	new_block = (t_block *)((char *)block + sizeof(t_block) + size);
	new_block->size = remaining;
	new_block->is_free = true;
	new_block->next = block->next;
	new_block->prev = block;

	if (block->next)
		block->next->prev = new_block;

	block->next = new_block;
	block->size = size;
}

/**
 * allocate_from_zone - Alloue de la mémoire depuis les zones TINY ou SMALL
 * @zones: Pointeur vers la liste des zones (tiny_zones ou small_zones)
 * @size: Taille demandée en bytes
 * @zone_size: Taille d'une nouvelle zone si création nécessaire
 * 
 * Retourne:
 *   - Pointeur vers la mémoire allouée
 *   - NULL si échec
 * 
 * Description:
 *   1. Aligne la taille demandée sur ALIGNMENT (16 bytes)
 *   2. Cherche un bloc libre dans les zones existantes
 *   3. Si trouvé: utilise ce bloc
 *   4. Si pas trouvé: crée une nouvelle zone
 *   5. Fragmente le bloc si possible
 *   6. Marque le bloc comme utilisé
 *   7. Retourne pointeur vers mémoire utilisable (après t_block header)
 */
void *allocate_from_zone(t_zone **zones, size_t size, size_t zone_size)
{
	t_zone *zone;
	t_block *block;
	size_t aligned_size;

	aligned_size = ALIGN(size);

	/* Find a free block in existing zones */
	block = find_free_block(*zones, aligned_size);

	/* If no free block found, create a new zone */
	if (!block)
	{
		zone = create_zone(zone_size);
		if (!zone)
			return NULL;

		zone->next = *zones;
		*zones = zone;
		block = zone->blocks;
	}

	/* Split block if possible */
	split_block(block, aligned_size);

	block->is_free = false;
	return (void *)((char *)block + sizeof(t_block));
}

/**
 * allocate_large - Alloue directement une grande zone de mémoire via mmap
 * @size: Taille à allouer en bytes
 * 
 * Retourne:
 *   - Pointeur vers la mémoire allouée
 *   - NULL si mmap échoue
 * 
 * Description:
 *   - Utilisé pour les allocations > 1024 bytes
 *   - Alloue exactement size bytes (pas de fragmentation)
 *   - Crée une structure t_block au début de la zone mappée
 *   - Ajoute le bloc à la liste chaînée large_blocks
 *   - Lors du free(), ce bloc sera démappé (munmap) immédiatement
 */
void *allocate_large(size_t size)
{
	t_block *block;
	size_t total_size;
	void *mapped;

	total_size = ALIGN(size + sizeof(t_block));
	mapped = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
				  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mapped == MAP_FAILED)
		return NULL;

	block = (t_block *)mapped;
	block->size = size;
	block->is_free = false;
	block->next = g_manager.large_blocks;
	block->prev = NULL;

	if (g_manager.large_blocks)
		g_manager.large_blocks->prev = block;

	g_manager.large_blocks = block;

	return (void *)((char *)block + sizeof(t_block));
}

/**
 * malloc - Alloue dynamiquement de la mémoire
 * @size: Nombre de bytes à allouer
 * 
 * Retourne:
 *   - Pointeur vers la mémoire allouée si succès
 *   - NULL si size == 0 ou échec d'allocation
 * 
 * Description:
 *   Implémentation personnalisée de malloc utilisant une stratégie à trois niveaux:
 *   
 *   TINY (size <= 128 bytes):
 *     - Alloué dans des zones de 16KB (4 pages)
 *     - Réutilise les blocs libérés précédemment
 *   
 *   SMALL (128 < size <= 1024 bytes):
 *     - Alloué dans des zones de 64KB (16 pages)
 *     - Réutilise les blocs libérés précédemment
 *   
 *   LARGE (size > 1024 bytes):
 *     - Alloué directement avec mmap()
 *     - Pas de réutilisation, retourné au système lors du free()
 *   
 *   Caractéristiques:
 *   - Thread-safe: utilise un mutex pour protéger les opérations
 *   - Alignement: tous les blocs sont alignés sur 16 bytes
 *   - Récursion: prévient les appels récursifs avec un buffer d'urgence
 * 
 * Exemple:
 *   int *arr = malloc(10 * sizeof(int));
 *   if (!arr) return -1;  // Toujours vérifier!
 *   arr[0] = 42;
 *   free(arr);
 */
void *malloc(size_t size)
{
	ft_printf("truc bidule chouatte");
	void *ptr;
	t_malloc_manager *manager;
	static char emergency_buffer[1024];
	static size_t emergency_offset = 0;

	if (size == 0)
		return NULL;

	/* Prevent recursive malloc calls during initialization */
	if (g_in_malloc)
	{
		if (emergency_offset + size <= sizeof(emergency_buffer))
		{
			ptr = emergency_buffer + emergency_offset;
			emergency_offset += ALIGN(size);
			return ptr;
		}
		return NULL;
	}

	g_in_malloc = true;

	if (!g_initialized)
		init_malloc_manager();

	manager = get_malloc_manager();
	pthread_mutex_lock(&manager->mutex);

	if (size <= TINY_SIZE)
		ptr = allocate_from_zone(&manager->tiny_zones, size, TINY_ZONE_SIZE);
	else if (size <= SMALL_SIZE)
		ptr = allocate_from_zone(&manager->small_zones, size, SMALL_ZONE_SIZE);
	else
		ptr = allocate_large(size);

	pthread_mutex_unlock(&manager->mutex);
	g_in_malloc = false;

	return ptr;
}
