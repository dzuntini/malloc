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

/* Get block header from user pointer */
t_block *get_block_header(void *ptr)
{
	if (!ptr)
		return NULL;
	return (t_block *)((char *)ptr - sizeof(t_block));
}

/* Create a new zone for TINY or SMALL allocations */
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

/* Find a free block in a zone that fits the requested size */
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

/* Split a block if it's large enough */
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

/* Allocate memory from a zone (TINY or SMALL) */
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

/* Allocate a large block directly with mmap */
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

/* Main malloc function */
void *malloc(size_t size)
{
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