#include "malloc.h"

/**
 * coalesce_blocks - Fusionne les blocs libres adjacents dans une zone
 * @zone: Pointeur vers la zone à coalescer
 * 
 * Description:
 *   - Parcourt la chaîne de blocs dans la zone
 *   - Quand deux blocs libres sont adjacents: les fusionne en un seul bloc
 *   - Réduit la fragmentation mémoire
 *   - Maintient les pointeurs next/prev correctement
 *   
 * Exemple:
 *   Avant: [Bloc libre 64B] -> [Bloc libre 64B]
 *   Après:  [Bloc libre 128B + sizeof(t_block)]
 */
void coalesce_blocks(t_zone *zone)
{
	t_block *current;

	if (!zone)
		return;

	current = zone->blocks;
	while (current && current->next)
	{
		if (current->is_free && current->next->is_free)
		{
			current->size += sizeof(t_block) + current->next->size;
			current->next = current->next->next;
			if (current->next)
				current->next->prev = current;
		}
		else
			current = current->next;
	}
}

/**
 * free_zone_block - Libère un bloc dans une zone TINY ou SMALL
 * @ptr: Pointeur vers la mémoire à libérer
 * 
 * Description:
 *   - Récupère l'en-tête du bloc (t_block metadata)
 *   - Marque le bloc comme libre (is_free = true)
 *   - Le bloc peut être réutilisé lors de futurs malloc()
 *   - Ne démapple pas la zone (celle-ci reste allouée)
 *   
 * Note:
 *   La coalesce n'est pas effectuée immédiatement ici (lazy coalesce)
 *   pour optimiser les performances. Elle peut être déclenchée périodiquement.
 */
void free_zone_block(void *ptr)
{
	t_block *block;

	block = get_block_header(ptr);
	if (!block)
		return;

	block->is_free = true;
	/* Coalescing is done lazily or periodically */
}

/**
 * free_large_block - Libère un bloc LARGE et le démapple
 * @ptr: Pointeur vers la mémoire LARGE à libérer
 * 
 * Description:
 *   - Récupère l'en-tête du bloc (t_block metadata)
 *   - Retire le bloc de la liste chaînée large_blocks
 *   - Appelle munmap() pour retourner les pages au système d'exploitation
 *   - Contrairement aux zones TINY/SMALL, cette mémoire est complètement libérée
 */
void free_large_block(void *ptr)
{
	t_block *block;
	size_t total_size;
	t_malloc_manager *manager;

	block = get_block_header(ptr);
	if (!block)
		return;

	manager = get_malloc_manager();

	/* Remove from large blocks list */
	if (block->prev)
		block->prev->next = block->next;
	else
		manager->large_blocks = block->next;

	if (block->next)
		block->next->prev = block->prev;

	total_size = ALIGN(block->size + sizeof(t_block));
	munmap(block, total_size);
}

/**
 * is_in_zone - Détermine si un pointeur appartient à une zone
 * @zones: Pointeur vers la première zone d'une chaîne
 * @ptr: Pointeur à vérifier
 * 
 * Retourne:
 *   - true si ptr est dans l'une des zones
 *   - false sinon
 * 
 * Description:
 *   - Parcourt toutes les zones chaînées
 *   - Pour chaque zone, vérifie si ptr est entre le début et la fin
 *   - Utilisé par free() pour déterminer si c'est un bloc TINY/SMALL ou LARGE
 */
bool is_in_zone(t_zone *zones, void *ptr)
{
	t_zone *current;

	current = zones;
	while (current)
	{
		if ((char *)ptr >= (char *)current &&
			(char *)ptr < (char *)current + current->zone_size)
			return true;
		current = current->next;
	}
	return false;
}

/**
 * free - Libère la mémoire allouée par malloc, calloc ou realloc
 * @ptr: Pointeur vers la mémoire à libérer
 * 
 * Retourne: void
 * 
 * Description:
 *   Implémentation personnalisée de free() qui:
 *   
 *   1. Ne fait rien si ptr == NULL (comportement standard)
 *   
 *   2. Détermine le type de bloc:
 *      - TINY/SMALL: bloc est marqué comme libre, peut être réutilisé
 *      - LARGE: bloc est démappé immédiatement, retourné au système
 *   
 *   3. Thread-safe: utilise un mutex pour les opérations critiques
 * 
 * Cas d'utilisation:
 *   int *arr = malloc(100);
 *   arr[0] = 42;
 *   free(arr);
 *   arr = NULL;  // Bonne pratique pour éviter les use-after-free
 * 
 * Important:
 *   - Ne jamais accéder à la mémoire après free()
 *   - Ne jamais appeler free() deux fois sur le même pointeur (double-free)
 *   - free(NULL) est sûr (pas d'effet)
 */
void free(void *ptr)
{
	t_malloc_manager *manager;

	if (!ptr)
		return;

	manager = get_malloc_manager();
	pthread_mutex_lock(&manager->mutex);

	if (is_in_zone(manager->tiny_zones, ptr) ||
		is_in_zone(manager->small_zones, ptr))
		free_zone_block(ptr);
	else
		free_large_block(ptr);

	pthread_mutex_unlock(&manager->mutex);
}
