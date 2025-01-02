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
void itoi_map_init(itoi_map_t *map);
void itoi_map_insert(itoi_map_t *map, int key, int value);
int itoi_map_get(itoi_map_t *map, int key, int *value);
void itoi_map_remove(itoi_map_t *map, int key);
void itoi_map_destroy(itoi_map_t *map);
void itoi_map_iterate(itoi_map_t *map, void (*callback)(int key, int value, void *args), void *args);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* ITOI_MAP_H */

