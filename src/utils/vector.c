/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : vector
 * @created     : Vendredi jan 03, 2025 12:50:47 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "vector.h"

#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
int vector_init(vector_t *vec, size_t element_size, size_t initial_capacity)
{
    vec->element_size = element_size;
    vec->capacity = initial_capacity;
    vec->size = 0;
    if (initial_capacity)
    {
        vec->data = malloc(element_size * vec->capacity);
        if (vec->data == NULL)
        {
            return 1;
        }
    }
    else
    {
        vec->data = NULL;
    }

    return 0;
}

int vector_reserve(vector_t *vec, size_t additional_capacity)
{
    void *ndata = realloc(vec->data, (vec->capacity + additional_capacity) * vec->element_size);
    if (ndata == NULL)
    {
        return 1;
    }
    vec->data = ndata;
    vec->capacity += additional_capacity;

    return 0;
}

int vector_shrink(vector_t *vec)
{
    if (vec->size == 0)
    {
        free(vec->data);
        vec->data = NULL;
        vec->capacity = 0;
    }
    else
    {
        void *ndata = realloc(vec->data, vec->size * vec->element_size);
        if (ndata == NULL)
        {
            return 1;
        }
        vec->data = ndata;
        vec->capacity = vec->size;
    }

    return 0;
}

int vector_push_back(vector_t *vec, void *element)
{
    if (vec->size >= vec->capacity)
    {
        size_t new_capacity = vec->capacity ? vec->capacity * 2 : 1;
        void *ndata = realloc(vec->data, new_capacity * vec->element_size);
        if (ndata == NULL)
        {
            return 1;
        }
        vec->data = ndata;
        vec->capacity = new_capacity;
    }

    memcpy((char *)vec->data + vec->size * vec->element_size, element, vec->element_size);
    ++vec->size;

    return 0;
}

