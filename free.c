#include "malloc.h"

/* Coalesce adjacent free blocks in a zone */
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

/* Free a block in a zone */
void free_zone_block(void *ptr)
{
	t_block *block;

	block = get_block_header(ptr);
	if (!block)
		return;

	block->is_free = true;
	/* Coalescing is done lazily or periodically */
}

/* Free a large block */
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

/* Determine if pointer is in a zone */
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

/* Main free function */
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