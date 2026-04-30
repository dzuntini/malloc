#include "malloc.h"
#include <string.h>

void *realloc(void *ptr, size_t size)
{
	t_block *block;
	void *new_ptr;
	size_t copy_size;

	if (!ptr)
		return malloc(size);

	if (size == 0)
	{
		free(ptr);
		return NULL;
	}

	block = get_block_header(ptr);
	if (!block)
		return NULL;

	/* If the new size fits in the current block, return the same pointer */
	if (block->size >= size)
		return ptr;

	/* Allocate new block */
	new_ptr = malloc(size);
	if (!new_ptr)
		return NULL;

	/* Copy data from old block to new block */
	copy_size = block->size < size ? block->size : size;
	memcpy(new_ptr, ptr, copy_size);

	/* Free old block */
	free(ptr);

	return new_ptr;
}