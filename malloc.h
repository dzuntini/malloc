/* Malloc Library Implementation */

#pragma once

#define _GNU_SOURCE

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include "libft/libft.h"
#include <stdbool.h>

/* Alignment */
#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define PAGE_SIZE 4096

/* Size thresholds */
#define TINY_SIZE 128
#define SMALL_SIZE 1024
#define TINY_ZONE_SIZE (4 * PAGE_SIZE)
#define SMALL_ZONE_SIZE (16 * PAGE_SIZE)

/* Block header structure */
typedef struct s_block
{
	size_t size;
	bool is_free;
	struct s_block *next;
	struct s_block *prev;
} t_block;

/* Zone structure for TINY and SMALL allocations */
typedef struct s_zone
{
	size_t zone_size;
	size_t block_count;
	struct s_zone *next;
	t_block *blocks;
} t_zone;

/* Global memory manager */
typedef struct s_malloc_manager
{
	t_zone *tiny_zones;
	t_zone *small_zones;
	t_block *large_blocks;
	pthread_mutex_t mutex;
} t_malloc_manager;

/* Function prototypes */
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);

/* Utility functions */
t_block *find_free_block(t_zone *zone, size_t size);
t_zone *create_zone(size_t zone_size);
void *allocate_large(size_t size);
t_block *get_block_header(void *ptr);
void coalesce_blocks(t_zone *zone);
t_malloc_manager *get_malloc_manager(void);
