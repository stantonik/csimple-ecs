/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : stoi_map
 * @created     : Jeudi jan 02, 2025 01:43:01 CET
 */

#ifndef STOI_MAP_H
#define STOI_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "../src/external/uthash.h"

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct stoi_map_entry 
{
    char *key;
    int value;
    UT_hash_handle hh;
} stoi_map_entry_t;

typedef struct stoi_map 
{
    stoi_map_entry_t *entries;
} stoi_map_t;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static inline void stoi_map_init(stoi_map_t *map) 
{
    map->entries = NULL;
}

static inline void stoi_map_insert(stoi_map_t *map, const char *key, int value) 
{
    stoi_map_entry_t *entry = (stoi_map_entry_t *)malloc(sizeof(stoi_map_entry_t));
    entry->key = strdup(key);
    entry->value = value;
    HASH_ADD_KEYPTR(hh, map->entries, entry->key, strlen(entry->key), entry);
}

static inline int stoi_map_get(stoi_map_t *map, const char *key, int *value) 
{
    stoi_map_entry_t *entry;
    HASH_FIND_STR(map->entries, key, entry);
    if (entry) {
        if (value != NULL)
        {
            *value = entry->value;
        }
        return 1;
    }

    return 0;
}

static inline void stoi_map_remove(stoi_map_t *map, const char *key)
{
    stoi_map_entry_t *entry;
    HASH_FIND_STR(map->entries, key, entry);
    if (entry) {
        HASH_DEL(map->entries, entry);
        free(entry->key);
        free(entry);
    }
}

static inline void stoi_map_destroy(stoi_map_t *map)
{
    stoi_map_entry_t *entry, *tmp;
    HASH_ITER(hh, map->entries, entry, tmp) {
        HASH_DEL(map->entries, entry);
        free(entry->key);
        free(entry);
    }
}

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* STOI_MAP_H */

