/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : itoi_map
 * @created     : Jeudi jan 02, 2025 01:45:57 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "itoi_map.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
void itoi_map_init(itoi_map_t *map)
{
    map->entries = NULL;
}

void itoi_map_insert(itoi_map_t *map, int key, int value)
{
    itoi_map_entry_t *entry = (itoi_map_entry_t *)malloc(sizeof(itoi_map_entry_t));
    entry->key = key;
    entry->value = value;
    HASH_ADD_INT(map->entries, key, entry);
}

int itoi_map_get(itoi_map_t *map, int key, int *value)
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

void itoi_map_remove(itoi_map_t *map, int key)
{
    itoi_map_entry_t *entry;
    HASH_FIND_INT(map->entries, &key, entry);
    if (entry)
    {
        HASH_DEL(map->entries, entry);
        free(entry);
    }
}

void itoi_map_destroy(itoi_map_t *map)
{
    itoi_map_entry_t *entry, *tmp;
    HASH_ITER(hh, map->entries, entry, tmp)
    {
        HASH_DEL(map->entries, entry);
        free(entry);
    }
}

void itoi_map_iterate(itoi_map_t *map, void (*callback)(int key, int value, void *args), void *args) 
{
    if (!map || !callback) 
    {
        return; 
    }

    itoi_map_entry_t *entry, *tmp;
    HASH_ITER(hh, map->entries, entry, tmp) 
    {
        callback(entry->key, entry->value, args);
    }
}

