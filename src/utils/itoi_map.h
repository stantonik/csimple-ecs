/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : itoi_map
 * @created     : Jeudi jan 02, 2025 01:46:27 CET
 */

#ifndef ITOI_MAP_H
#define ITOI_MAP_H

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
typedef struct itoi_map_entry
{
    int key;
    int value;
    UT_hash_handle hh;
} itoi_map_entry_t;

typedef struct itoi_map
{
    itoi_map_entry_t *entries;
} itoi_map_t;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static inline void itoi_map_init(itoi_map_t *map)
{
    map->entries = NULL;
}

static inline void itoi_map_insert(itoi_map_t *map, int key, int value)
{
    itoi_map_entry_t *entry = (itoi_map_entry_t *)malloc(sizeof(itoi_map_entry_t));
    entry->key = key;
    entry->value = value;
    HASH_ADD_INT(map->entries, key, entry);
}

static inline int itoi_map_get(itoi_map_t *map, int key, int *value)
{
    itoi_map_entry_t *entry;
    HASH_FIND_INT(map->entries, &key, entry);
    if (entry)
    {
        if (value)
        {
            *value = entry->value;
        }
        return 1;
    }
    return 0;
}

static inline void itoi_map_remove(itoi_map_t *map, int key)
{
    itoi_map_entry_t *entry;
    HASH_FIND_INT(map->entries, &key, entry);
    if (entry)
    {
        HASH_DEL(map->entries, entry);
        free(entry);
    }
}

static inline void itoi_map_destroy(itoi_map_t *map)
{
    itoi_map_entry_t *entry, *tmp;
    HASH_ITER(hh, map->entries, entry, tmp)
    {
        HASH_DEL(map->entries, entry);
        free(entry);
    }
}


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* ITOI_MAP_H */

