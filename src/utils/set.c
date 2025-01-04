/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : set
 * @created     : Samedi jan 04, 2025 01:28:34 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "set.h"

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
void set_init(set_t *set)
{
    set->entry = NULL;
}

void set_insert(set_t *set, int key)
{
    set_entry_t *entry = NULL;
    HASH_FIND_INT(set->entry, &key, entry);
    if (entry == NULL)
    {
        entry = malloc(sizeof(set_entry_t));
        if (entry == NULL)
        {
            return;
        }
        entry->key = key;
        HASH_ADD_INT(set->entry, key, entry);
    }
}

int set_contains(set_t *set, int key)
{
    set_entry_t *entry = NULL;
    HASH_FIND_INT(set->entry, &key, entry);
    return entry != NULL;
}

void set_delete(set_t *set, int key)
{
    set_entry_t *entry = NULL;
    HASH_FIND_INT(set->entry, &key, entry);
    if (entry != NULL)
    {
        HASH_DEL(set->entry, entry);
        free(entry);
    }
}

void set_free(set_t *set)
{
    set_entry_t *current, *tmp;
    HASH_ITER(hh, set->entry, current, tmp)
    {
        HASH_DEL(set->entry, current);
        free(current);
    }
}

