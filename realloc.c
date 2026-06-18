#include "malloc.h"
#include <string.h>

/**
 * realloc - Redimensionne un bloc de mémoire alloué
 * @ptr: Pointeur vers le bloc à redimensionner
 * @size: Nouvelle taille souhaitée en bytes
 * 
 * Retourne:
 *   - Pointeur vers le bloc redimensionné (peut être différent de ptr)
 *   - NULL si size == 0 ou en cas d'erreur d'allocation
 * 
 * Description:
 *   Redimensionne un bloc de mémoire précédemment alloué par malloc/calloc/realloc.
 *   
 *   Cas spéciaux:
 *   - Si ptr == NULL: agit comme malloc(size)
 *   - Si size == 0: agit comme free(ptr) et retourne NULL
 *   - Si size <= taille actuelle: retourne le même pointeur (optimisation)
 *   - Si size > taille actuelle: alloue un nouveau bloc
 *   
 *   Données:
 *   - Les données existantes sont conservées dans le nouveau bloc
 *   - La quantité copiée = min(ancienne_taille, nouvelle_taille)
 *   - Données au-delà de l'ancienne taille sont non initialisées
 * 
 * Algorithme:
 *   1. Gérer les cas spéciaux (ptr == NULL, size == 0)
 *   2. Récupérer l'en-tête du bloc original
 *   3. Si nouvelle_size <= ancienne_size: retourner ptr (pas de changement)
 *   4. Sinon:
 *      a) Allouer un nouveau bloc de size bytes
 *      b) Copier les données de l'ancien vers le nouveau bloc
 *      c) Libérer l'ancien bloc
 *      d) Retourner le nouveau pointeur
 * 
 * Exemple:
 *   int *arr = malloc(10 * sizeof(int));
 *   arr[0] = 42;
 *   
 *   // Agrandissement
 *   arr = realloc(arr, 20 * sizeof(int));
 *   printf("%d\n", arr[0]);  // Affiche: 42 (données conservées)
 *   
 *   // Réduction
 *   arr = realloc(arr, 5 * sizeof(int));
 *   
 *   // Cas spéciaux
 *   int *p1 = realloc(NULL, 100);      // Équivalent à malloc(100)
 *   int *p2 = realloc(p1, 0);          // Équivalent à free(p1)
 * 
 * Important:
 *   - TOUJOURS assigner le résultat: ptr = realloc(ptr, newsize)
 *   - Le pointeur original peut changer (ne pas garder l'ancien)
 *   - Si realloc échoue, retourne NULL et l'ancien pointeur est préservé
 */
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
