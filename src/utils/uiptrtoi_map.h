/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : uiptrtoi_map
 * @created     : Lundi jan 06, 2025 09:32:03 CET
 */

#ifndef UIPTRTOI_MAP_H
#define UIPTRTOI_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "../src/external/uthash.h"

#include <stdint.h>

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct uiptrtoi_map_entry
{
    uintptr_t key;
    int value;
    UT_hash_handle hh;
} uiptrtoi_map_entry_t;

typedef struct uiptrtoi_map
{
    uiptrtoi_map_entry_t *entries;
} uiptrtoi_map_t;

//------------------------------------------------------------------------------
// Inlined Functions
//------------------------------------------------------------------------------
static inline void uiptrtoi_map_init(uiptrtoi_map_t *map)
{
    map->entries = NULL;
}

static inline void uiptrtoi_map_insert(uiptrtoi_map_t *map, uintptr_t key, int value)
{
    uiptrtoi_map_entry_t *entry = (uiptrtoi_map_entry_t *)malloc(sizeof(uiptrtoi_map_entry_t));
    entry->key = key;
    entry->value = value;
    HASH_ADD(hh, map->entries, key, sizeof(uintptr_t), entry);
}

static inline int uiptrtoi_map_get(uiptrtoi_map_t *map, uintptr_t key, int *value)
{
    uiptrtoi_map_entry_t *entry;
    HASH_FIND(hh, map->entries, &key, sizeof(uintptr_t), entry);
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

static inline void uiptrtoi_map_remove(uiptrtoi_map_t *map, uintptr_t key)
{
    uiptrtoi_map_entry_t *entry;
    HASH_FIND(hh, map->entries, &key, sizeof(uintptr_t), entry);
    if (entry)
    {
        HASH_DEL(map->entries, entry);
        free(entry);
    }
}

static inline void uiptrtoi_map_destroy(uiptrtoi_map_t *map)
{
    uiptrtoi_map_entry_t *entry, *tmp;
    HASH_ITER(hh, map->entries, entry, tmp)
    {
        HASH_DEL(map->entries, entry);
        free(entry);
    }
}





#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* UIPTRTOI_MAP_H */

