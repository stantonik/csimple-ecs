/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : vector
 * @created     : Vendredi jan 03, 2025 12:50:41 CET
 */

#ifndef VECTOR_H
#define VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct
{
    void *data;
    size_t element_size;
    int size;
    int capacity;

} vector_t;

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------
static inline int vector_init(vector_t *vec, size_t element_size, size_t initial_capacity)
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

static inline int vector_reserve(vector_t *vec, size_t additional_capacity)
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

static inline int vector_shrink(vector_t *vec)
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

static inline int vector_push_back(vector_t *vec, void *element)
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

static inline void vector_get(vector_t *vec, unsigned int index, void **output)
{
    *output = ((char *)vec->data + index * vec->element_size);
}

static inline void vector_get_copy(vector_t *vec, unsigned int index, void *output)
{
    memcpy(output, (char *)vec->data + index * vec->element_size, vec->element_size);
}

static inline void vector_remove(vector_t *vec, unsigned int index)
{
    memmove((char *)vec->data + index * vec->element_size, 
            (char *)vec->data + (index + 1) * vec->element_size, 
            (vec->size - index - 1) * vec->element_size);
    --vec->size;
}

static inline void vector_free(vector_t *vec)
{
    free(vec->data);
    vec->data = NULL;
    vec->element_size = 0;
    vec->capacity = 0;
    vec->size = 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* VECTOR_H */

