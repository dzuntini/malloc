# Malloc - Custom Memory Allocator Implementation

Ce projet implémente une allocation de mémoire personnalisée en C, reproduisant le comportement des fonctions standard `malloc()`, `free()`, `realloc()` et `calloc()`. 

## 📋 Table des matières

- [Vue d'ensemble](#vue-densemble)
- [Architecture](#architecture)
- [Fonctions principales](#fonctions-principales)
- [Compilation](#compilation)
- [Utilisation](#utilisation)

---

## 🎯 Vue d'ensemble

Ce allocateur personnalisé utilise une stratégie **à trois niveaux** pour optimiser l'allocation mémoire :

1. **TINY** : Allocations ≤ 128 bytes
2. **SMALL** : Allocations de 129 à 1024 bytes
3. **LARGE** : Allocations > 1024 bytes

Les zones TINY et SMALL partagent des régions de mémoire prédéfinies et utilisent un système de fragmentation pour réduire le gaspillage. Les allocations LARGE utilisent directement `mmap()` pour allouer exactement la taille demandée.

---

## 🏗️ Architecture

### Structures de données

#### `t_block` (Bloc mémoire)
```c
typedef struct s_block
{
    size_t size;              // Taille du bloc
    bool is_free;             // État du bloc (libre ou utilisé)
    struct s_block *next;     // Pointeur vers le bloc suivant
    struct s_block *prev;     // Pointeur vers le bloc précédent
} t_block;
```
Chaque bloc de mémoire alloué est précédé d'un en-tête contenant ses métadonnées.

#### `t_zone` (Zone mémoire)
```c
typedef struct s_zone
{
    size_t zone_size;         // Taille totale de la zone
    size_t block_count;       // Nombre de blocs
    struct s_zone *next;      // Pointeur vers la zone suivante
    t_block *blocks;          // Pointeur vers le premier bloc
} t_zone;
```
Les zones sont des régions de mémoire mappées qui contiennent plusieurs blocs.

#### `t_malloc_manager` (Gestionnaire global)
```c
typedef struct s_malloc_manager
{
    t_zone *tiny_zones;       // Chaîne de zones TINY
    t_zone *small_zones;      // Chaîne de zones SMALL
    t_block *large_blocks;    // Chaîne de blocs LARGE
    pthread_mutex_t mutex;    // Mutex pour la thread-safety
} t_malloc_manager;
```

### Constantes importantes

| Constante | Valeur | Signification |
|-----------|--------|---------------|
| `ALIGNMENT` | 16 bytes | Alignement en mémoire |
| `TINY_SIZE` | 128 bytes | Seuil pour zones TINY |
| `SMALL_SIZE` | 1024 bytes | Seuil pour zones SMALL |
| `TINY_ZONE_SIZE` | 16 KB (4 pages) | Taille d'une zone TINY |
| `SMALL_ZONE_SIZE` | 64 KB (16 pages) | Taille d'une zone SMALL |

---

## 📚 Fonctions principales

### 1. **malloc(size_t size)** - Allocation mémoire

**Description :** Alloue un bloc de mémoire de `size` bytes et retourne un pointeur vers celui-ci.

**Prototype :**
```c
/**
 * malloc - Alloue dynamiquement de la mémoire
 * @size: Nombre de bytes à allouer
 * 
 * Description:
 *   - Retourne un pointeur vers un bloc de mémoire alloué de taille >= size
 *   - La mémoire n'est pas initialisée (contient des données aléatoires)
 *   - Utilise une stratégie à trois niveaux (TINY, SMALL, LARGE)
 *   - Thread-safe grâce à un mutex
 *   - Prévient les appels récursifs avec un buffer d'urgence
 * 
 * Retourne:
 *   - Pointeur vers la mémoire alloué si succès
 *   - NULL si échec ou size == 0
 * 
 * Cas d'utilisation:
 *   int *array = malloc(10 * sizeof(int));
 */
void *malloc(size_t size);
```

**Fonctionnement pas à pas :**

```
1. Vérifier que size > 0, sinon retourner NULL
2. Acquérir le mutex (thread-safety)
3. Déterminer le type d'allocation selon la taille:
   ├─ Si size ≤ 128 (TINY):
   │  └─ Chercher un bloc libre dans les zones TINY
   │     ├─ Si trouvé: utiliser ce bloc
   │     └─ Si pas trouvé: créer une nouvelle zone TINY
   │
   ├─ Si 128 < size ≤ 1024 (SMALL):
   │  └─ Chercher un bloc libre dans les zones SMALL
   │     ├─ Si trouvé: utiliser ce bloc
   │     └─ Si pas trouvé: créer une nouvelle zone SMALL
   │
   └─ Si size > 1024 (LARGE):
      └─ Allouer directement avec mmap()
4. Fragmenter le bloc si nécessaire
5. Marquer le bloc comme utilisé
6. Libérer le mutex
7. Retourner le pointeur vers la mémoire utilisable
```

**Exemple :**
```c
// Allocation TINY
char *buffer = malloc(64);        // Alloué dans une zone TINY

// Allocation SMALL
int *data = malloc(512);          // Alloué dans une zone SMALL

// Allocation LARGE
double *big = malloc(10000);      // Alloué avec mmap()

// Accès à la mémoire
buffer[0] = 'A';
data[0] = 42;
free(buffer);
free(data);
free(big);
```

---

### 2. **free(void *ptr)** - Libération mémoire

**Description :** Libère le bloc de mémoire pointé par `ptr` et le rend disponible pour futurs allocations.

**Prototype :**
```c
/**
 * free - Libère la mémoire allouée par malloc
 * @ptr: Pointeur vers la mémoire à libérer
 * 
 * Description:
 *   - Marque le bloc comme libre (TINY/SMALL) ou le démapple (LARGE)
 *   - Gère les blocs dans les trois catégories (TINY, SMALL, LARGE)
 *   - Thread-safe grâce au mutex
 *   - Ne fait rien si ptr == NULL (comportement standard)
 *   - Les blocs libres dans zones TINY/SMALL peuvent être réutilisés
 * 
 * Paramètres:
 *   - Si ptr est NULL: fonction retourne sans effet
 *   - Si ptr provient de malloc(): bloc marqué libre ou demap
 *   - Si ptr invalide: comportement indéfini
 * 
 * Cas d'utilisation:
 *   int *ptr = malloc(100);
 *   free(ptr);
 *   ptr = NULL;  // Bonne pratique
 */
void free(void *ptr);
```

**Fonctionnement pas à pas :**

```
1. Vérifier si ptr == NULL, si oui retourner
2. Acquérir le mutex (thread-safety)
3. Identifier où est alloué le bloc:
   ├─ Si dans une zone TINY:
   │  └─ Marquer le bloc comme libre
   │     └─ Déclencher une coalesce pour fusionner les blocs libres adjacents
   │
   ├─ Si dans une zone SMALL:
   │  └─ Marquer le bloc comme libre
   │     └─ Déclencher une coalesce pour fusionner les blocs libres adjacents
   │
   └─ Si dans les blocs LARGE:
      ├─ Retirer le bloc de la liste chaînée
      └─ Appeler munmap() pour libérer les pages mémoire au OS
4. Libérer le mutex
```

**Exemple :**
```c
int *data = malloc(256);
data[0] = 42;
printf("%d\n", data[0]);  // Affiche: 42

free(data);               // Libère la mémoire
data = NULL;              // Bonne pratique
// data[0] = 42;          // ❌ UNDEFINED BEHAVIOR après free!
```

**Spécificités TINY/SMALL vs LARGE :**

| Type | Libération | Réutilisation |
|------|-----------|---------------|
| **TINY/SMALL** | Bloc marqué comme libre | Réutilisable par les futures allocations dans la zone |
| **LARGE** | Démappé immédiatement (munmap) | Retourné au système d'exploitation |

---

### 3. **realloc(void *ptr, size_t size)** - Réallocation mémoire

**Description :** Redimensionne un bloc de mémoire existant. Peut déplacer la mémoire si nécessaire.

**Prototype :**
```c
/**
 * realloc - Redimensionne un bloc de mémoire alloué
 * @ptr: Pointeur vers le bloc à redimensionner
 * @size: Nouvelle taille souhaitée en bytes
 * 
 * Description:
 *   - Redimensionne la mémoire allouée par malloc/calloc/realloc
 *   - Copie les données existantes vers le nouveau bloc
 *   - Cas spéciaux:
 *     * Si ptr == NULL: agit comme malloc(size)
 *     * Si size == 0: agit comme free(ptr) et retourne NULL
 *     * Si size <= bloc actuel: retourne le même pointeur
 *     * Si size > bloc actuel: alloue un nouveau bloc
 *   - Données existantes: conservées (min(ancienne_size, nouvelle_size))
 * 
 * Retourne:
 *   - Pointeur vers le bloc redimensionné
 *   - NULL si erreur ou size == 0
 *   - Peut être différent du pointeur original
 * 
 * Cas d'utilisation:
 *   int *arr = malloc(10 * sizeof(int));
 *   arr = realloc(arr, 20 * sizeof(int));  // Passer le pointeur!
 */
void *realloc(void *ptr, size_t size);
```

**Fonctionnement pas à pas :**

```
1. Cas spéciaux:
   ├─ Si ptr == NULL:
   │  └─ Appeler malloc(size) et retourner
   │
   └─ Si size == 0:
      ├─ Appeler free(ptr)
      └─ Retourner NULL

2. Récupérer l'en-tête du bloc original (get_block_header)

3. Vérifier si optimisation possible:
   └─ Si bloc actuel.size >= size demandée:
      └─ Retourner le même pointeur (pas de déplacement)

4. Sinon: allocation et copie
   ├─ Allouer un nouveau bloc (malloc(size))
   ├─ Copier les données:
   │  └─ memcpy(nouveau, ancien, min(ancienne_size, nouvelle_size))
   ├─ Libérer l'ancien bloc (free(ptr))
   └─ Retourner le nouveau pointeur
```

**Exemple :**
```c
// Allocation initiale
int *arr = malloc(10 * sizeof(int));
arr[0] = 42;

// Agrandissement
arr = realloc(arr, 20 * sizeof(int));
printf("%d\n", arr[0]);  // Affiche: 42 (données préservées)
arr[15] = 100;

// Réduction
arr = realloc(arr, 5 * sizeof(int));

// Cas spéciaux
int *p1 = realloc(NULL, 100);  // Équivalent à malloc(100)
int *p2 = realloc(p1, 0);      // Équivalent à free(p1), retourne NULL

free(arr);
```

**Attention :** 
```c
// ❌ MAUVAIS - Variable non mise à jour
int *ptr = malloc(10);
realloc(ptr, 20);  // Le pointeur a peut-être changé!

// ✅ BON - Mettre à jour le pointeur
int *ptr = malloc(10);
ptr = realloc(ptr, 20);  // Assignation obligatoire
```

---

### 4. **calloc(size_t nmemb, size_t size)** - Allocation avec initialisation

**Description :** Alloue de la mémoire pour un tableau et initialise tout à zéro.

**Prototype :**
```c
/**
 * calloc - Alloue de la mémoire et l'initialise à zéro
 * @nmemb: Nombre d'éléments
 * @size: Taille de chaque élément en bytes
 * 
 * Description:
 *   - Alloue nmemb * size bytes
 *   - Initialise TOUS les bytes à 0x00
 *   - Vérifie le débordement (nmemb * size > SIZE_MAX)
 *   - Useful pour les structures à initialiser (listes, arbres, etc.)
 * 
 * Retourne:
 *   - Pointeur vers la mémoire allouée et initialisée
 *   - NULL si:
 *     * nmemb == 0 ou size == 0
 *     * Débordement multiplicatif (nmemb * size > SIZE_MAX)
 *     * malloc a échoué
 * 
 * Cas d'utilisation:
 *   int *arr = calloc(10, sizeof(int));  // 40 bytes à zéro
 *   struct Node *nodes = calloc(5, sizeof(struct Node));
 */
void *calloc(size_t nmemb, size_t size);
```

**Fonctionnement pas à pas :**

```
1. Vérifier les paramètres
   ├─ Si nmemb == 0 ou size == 0:
   │  └─ Retourner NULL
   │
   └─ Vérifier débordement:
      ├─ Si nmemb > SIZE_MAX / size:
      │  └─ Débordement détecté!
      │     └─ Retourner NULL
      └─ Sinon: calculer total_size = nmemb * size

2. Allouer la mémoire
   └─ Appeler malloc(total_size)

3. Initialiser à zéro
   └─ Si allocation réussie:
      └─ memset(ptr, 0, total_size)

4. Retourner le pointeur alloué et initialisé
```

**Exemple :**
```c
// Allocation de tableau
int *arr = calloc(10, sizeof(int));
printf("%d\n", arr[0]);           // Affiche: 0 (initialisé)

// Allocation de structures
struct Point {
    int x;
    int y;
};
struct Point *points = calloc(5, sizeof(struct Point));
printf("%d\n", points[0].x);      // Affiche: 0 (tous les champs à 0)

// Vs malloc
int *arr1 = malloc(10 * sizeof(int));
printf("%d\n", arr1[0]);          // Affiche: ??? (aléatoire)

int *arr2 = calloc(10, sizeof(int));
printf("%d\n", arr2[0]);          // Affiche: 0 (garanti)

free(arr);
free(points);
free(arr1);
free(arr2);
```

---

## 🔧 Fonctions utilitaires

### `get_block_header(void *ptr)` - Récupère l'en-tête du bloc
```c
/**
 * Récupère la métadonnée d'un bloc alloué
 * Le pointeur utilisable pointe juste après l'en-tête
 * En-tête: [t_block metadata] <- size, is_free, next, prev
 * Pointeur: utilisable à partir d'ici ->
 */
t_block *header = get_block_header(ptr);
printf("Taille: %zu bytes\n", header->size);
printf("Libre: %d\n", header->is_free);
```

### `create_zone(size_t zone_size)` - Crée une nouvelle zone
Utilise `mmap()` pour mapper une région de mémoire physique à une zone virtuelle, puis initialise le premier bloc comme libre.

### `find_free_block(t_zone *zone, size_t size)` - Cherche un bloc libre
Parcourt la chaîne de zones et retourne le premier bloc libre pouvant contenir `size` bytes.

### `allocate_from_zone(...)` - Allocation depuis une zone
Alloue depuis les zones TINY/SMALL: cherche un bloc libre, en crée une zone si nécessaire, fragmente si possible.

### `allocate_large(size_t size)` - Allocation directe mmap
Alloue un bloc LARGE via `mmap()` et l'ajoute à la liste des blocs LARGE.

### `coalesce_blocks(t_zone *zone)` - Fusion de blocs libres
Fusionne les blocs libres adjacents pour réduire la fragmentation:
```
Avant:  [Bloc libre 64B] [Bloc libre 64B] -> Après: [Bloc libre 128B]
```

---

## 🛠️ Compilation

### Makefile
```bash
make           # Compiler la bibliothèque
make clean     # Supprimer les fichiers compilés
make fclean    # Supprimer tous les fichiers générés
make re        # Recompiler tout
make test      # Compiler et lancer les tests
```

### Compilation manuelle
```bash
gcc -c malloc.c -o malloc.o
gcc -c free.c -o free.o
gcc -c realloc.c -o realloc.o
gcc -c calloc.c -o calloc.o
ar rcs libft_malloc.a malloc.o free.o realloc.o calloc.o

# Ou créer une library dynamique
gcc -shared -fPIC -c malloc.c free.c realloc.c calloc.c
gcc -shared -o libft_malloc.so malloc.o free.o realloc.o calloc.o
```

---

## 📖 Utilisation

### Lier contre la librairie
```bash
# Avec archive statique
gcc -o program program.c -L. -lft_malloc

# Avec library dynamique
gcc -o program program.c -L. -lft_malloc
LD_LIBRARY_PATH=. ./program
```

### Exemple de programme
```c
#include <stdio.h>
#include "malloc.h"

int main(void)
{
    // malloc - allocation simple
    int *arr = malloc(10 * sizeof(int));
    arr[0] = 42;
    printf("arr[0] = %d\n", arr[0]);
    
    // calloc - allocation avec initialisation
    char *buffer = calloc(256, sizeof(char));
    printf("buffer[0] = %c (vide)\n", buffer[0]);
    
    // realloc - redimensionnement
    arr = realloc(arr, 20 * sizeof(int));
    arr[15] = 100;
    
    // Libération
    free(arr);
    free(buffer);
    
    return 0;
}
```

---

## 🔒 Thread-Safety

L'allocateur est **thread-safe** grâce à un mutex (`pthread_mutex_t`) qui protège toutes les opérations critiques. Chaque thread peut appeler `malloc()`, `free()`, etc. sans risque de corruption mémoire.

## 🏁 Résumé des stratégies

| Opération | TINY (<= 128B) | SMALL (129-1024B) | LARGE (> 1024B) |
|-----------|----------------|------------------|-----------------|
| Allocation | Zone 16KB | Zone 64KB | mmap direct |
| Réutilisation | Blocs libres | Blocs libres | Non (munmap) |
| Fragmentation | Minimale | Minimale | Aucune |
| Coalesce | Lazy | Lazy | N/A |

---

**Auteur :** dzuntini  
**Langage :** C  
**Système :** Linux x86_64
