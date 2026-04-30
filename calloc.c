#include "malloc.h"
#include <string.h>
#include <stdint.h>

void *calloc(size_t nmemb, size_t size)
{
	void *ptr;
	size_t total_size;

	if (nmemb == 0 || size == 0)
		return NULL;

	/* Check for overflow */
	if (nmemb > SIZE_MAX / size)
		return NULL;

	total_size = nmemb * size;
	ptr = malloc(total_size);

	if (ptr)
		memset(ptr, 0, total_size);

	return ptr;
}