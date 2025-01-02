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
extern void stoi_map_init(stoi_map_t *map);
extern void stoi_map_insert(stoi_map_t *map, const char *key, int value);
extern int stoi_map_get(stoi_map_t *map, const char *key, int *value);
extern void stoi_map_remove(stoi_map_t *map, const char *key);
extern void stoi_map_destroy(stoi_map_t *map);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* STOI_MAP_H */

