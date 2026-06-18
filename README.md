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

Consultez les **commentaires Javadoc détaillés** dans les fichiers source pour comprendre le fonctionnement exact de chaque fonction :

### `malloc(size_t size)` - Allocation mémoire
**Fichier :** `malloc.c` (ligne ~200)

Alloue un bloc de mémoire et retourne un pointeur. Utilise la stratégie à trois niveaux (TINY, SMALL, LARGE).

**Prototypes des fonctions utilitaires :**
- `allocate_from_zone()` - Alloue depuis zones TINY/SMALL
- `allocate_large()` - Alloue directement via mmap pour LARGE
- `create_zone()` - Crée une nouvelle zone
- `find_free_block()` - Cherche un bloc libre
- `split_block()` - Fragmente un bloc

### `free(void *ptr)` - Libération mémoire
**Fichier :** `free.c` (ligne ~82)

Libère le bloc de mémoire pointé par `ptr`. 
- TINY/SMALL : bloc marqué comme libre, réutilisable
- LARGE : bloc démappé immédiatement via munmap

**Prototypes des fonctions utilitaires :**
- `free_zone_block()` - Libère un bloc dans une zone
- `free_large_block()` - Démapple un bloc LARGE
- `is_in_zone()` - Vérifie si un pointeur est dans une zone
- `coalesce_blocks()` - Fusionne les blocs libres adjacents

### `realloc(void *ptr, size_t size)` - Réallocation mémoire
**Fichier :** `realloc.c` (ligne ~4)

Redimensionne un bloc de mémoire. Peut déplacer la mémoire si nécessaire.

**Cas spéciaux :**
- `ptr == NULL` → agit comme `malloc(size)`
- `size == 0` → agit comme `free(ptr)`, retourne NULL
- `size <= bloc actuel` → retourne le même pointeur (optimisation)

### `calloc(size_t nmemb, size_t size)` - Allocation avec initialisation
**Fichier :** `calloc.c` (ligne ~5)

Alloue de la mémoire pour un tableau et initialise tout à zéro.

**Vérifie le débordement :** `if (nmemb > SIZE_MAX / size) return NULL`

---

## 🔧 Fonctions utilitaires

| Fonction | Fichier | Description |
|----------|---------|-------------|
| `get_block_header()` | malloc.c | Récupère l'en-tête du bloc |
| `create_zone()` | malloc.c | Crée une nouvelle zone mappée |
| `find_free_block()` | malloc.c | Cherche un bloc libre suffisant |
| `allocate_from_zone()` | malloc.c | Alloue depuis zones TINY/SMALL |
| `allocate_large()` | malloc.c | Alloue un bloc LARGE via mmap |
| `split_block()` | malloc.c | Fragmente un bloc en deux |
| `coalesce_blocks()` | free.c | Fusionne blocs libres adjacents |
| `is_in_zone()` | free.c | Vérifie si pointeur dans zone |
| `free_zone_block()` | free.c | Libère bloc TINY/SMALL |
| `free_large_block()` | free.c | Démapple bloc LARGE |

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
gcc -shared -fPIC malloc.c free.c realloc.c calloc.c \
  -o libft_malloc.so -lpthread
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
    if (!arr) return -1;
    arr[0] = 42;
    printf("arr[0] = %d\n", arr[0]);
    
    // calloc - allocation avec initialisation
    char *buffer = calloc(256, sizeof(char));
    printf("buffer[0] = %d (zéro)\n", buffer[0]);
    
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

---

## 📊 Résumé des stratégies

| Opération | TINY (<= 128B) | SMALL (129-1024B) | LARGE (> 1024B) |
|-----------|----------------|------------------|-----------------|
| Allocation | Zone 16KB | Zone 64KB | mmap direct |
| Réutilisation | Blocs libres | Blocs libres | Non (munmap) |
| Fragmentation | Minimale | Minimale | Aucune |
| Coalesce | Lazy | Lazy | N/A |

---

## 📖 Documentation détaillée

Pour une compréhension approfondie du fonctionnement de chaque fonction, consultez les **commentaires Javadoc** directement dans les fichiers source :

- 📄 `malloc.c` : Fonctions d'allocation
- 📄 `free.c` : Fonctions de libération
- 📄 `realloc.c` : Fonction de réallocation
- 📄 `calloc.c` : Fonction d'allocation initialisée
- 📄 `malloc.h` : Structures et prototypes

---

**Auteur :** dzuntini  
**Langage :** C  
**Système :** Linux x86_64
