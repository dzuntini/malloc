#include "malloc.h"
#include <string.h>
#include <stdint.h>

/**
 * calloc - Alloue de la mémoire pour un tableau et l'initialise à zéro
 * @nmemb: Nombre d'éléments du tableau
 * @size: Taille de chaque élément en bytes
 * 
 * Retourne:
 *   - Pointeur vers la mémoire allouée et initialisée si succès
 *   - NULL si:
 *     * nmemb == 0 ou size == 0
 *     * Débordement multiplicatif: nmemb * size > SIZE_MAX
 *     * malloc échoue
 * 
 * Description:
 *   Alloue de la mémoire pour un tableau de nmemb éléments de size bytes chacun,
 *   puis initialise TOUS les bytes à 0x00.
 *   
 *   Différences avec malloc:
 *   - calloc initialise toute la mémoire à zéro
 *   - calloc prend deux paramètres (nombre + taille) au lieu d'un
 *   - calloc vérifie les débordements multiplicatifs
 *   
 *   Débordement:
 *   - Si nmemb * size > SIZE_MAX: retourne NULL
 *   - Exemple: calloc(SIZE_MAX, 2) retourne NULL (débordement)
 * 
 * Algorithme:
 *   1. Vérifier que nmemb > 0 ET size > 0
 *   2. Vérifier le débordement: if (nmemb > SIZE_MAX / size) return NULL
 *   3. Calculer total_size = nmemb * size
 *   4. Allouer la mémoire avec malloc(total_size)
 *   5. Si allocation réussie: initialiser à zéro avec memset
 *   6. Retourner le pointeur alloué
 * 
 * Cas d'utilisation:
 *   
 *   // Tableau d'entiers (tous à 0)
 *   int *arr = calloc(10, sizeof(int));
 *   printf("%d\n", arr[0]);  // Affiche: 0 (garanti)
 *   
 *   // Allocation de structures
 *   struct Node {
 *       int data;
 *       struct Node *next;
 *   };
 *   struct Node *nodes = calloc(5, sizeof(struct Node));
 *   // Tous les champs sont à 0, tous les pointeurs are NULL
 *   
 *   // Vs malloc
 *   int *arr1 = malloc(10 * sizeof(int));
 *   printf("%d\n", arr1[0]);  // Affiche: ??? (valeur aléatoire)
 *   
 *   int *arr2 = calloc(10, sizeof(int));
 *   printf("%d\n", arr2[0]);  // Affiche: 0 (garanti)
 * 
 * Performance:
 *   calloc est légèrement plus lent que malloc car il initialise la mémoire.
 *   Pour les tableaux de structures complexes, l'initialisation à zéro
 *   est très utile (pointeurs = NULL, etc.)
 * 
 * Important:
 *   - Toujours libérer avec free(): free(arr);
 *   - calloc(0, size) et calloc(nmemb, 0) retournent NULL
 *   - Pas d'initialisation à 0 avec malloc!
 */
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
