/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : vector
 * @created     : Jeudi jan 02, 2025 02:50:49 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "vector.h"
#include <stdlib.h>
#include <string.h>

int vector_init(vector_t *vec, size_t element_size, int initial_capacity)
{
    vec->array = malloc(initial_capacity * sizeof(void *));
    if (vec->array == NULL)
    {
        return 1;
    }
    vec->size = 0;
    vec->capacity = initial_capacity;
    vec->element_size = element_size;

    return 0;
}

int vector_resize(vector_t *vec)
{
    void **new_array = realloc(vec->array, (vec->capacity * 2) * sizeof(void *));
    if (new_array == NULL)
    {
        return 1;
    }
    vec->array = new_array;
    vec->capacity *= 2;

    return 0;
}

int vector_shrink(vector_t *vec)
{
    if (vec->size == 0) 
    {
        free(vec->array);
        vec->array = NULL;
        vec->capacity = 0;
    } 
    else 
    {
        vec->capacity = vec->size;
        void **new_array = realloc(vec->array, vec->capacity * sizeof(void *));
        if (new_array == NULL)
        {
            return 1;
        }
        vec->array = new_array;
    }

    return 0;
}

int vector_add_element(vector_t *vec, void *element)
{
    if (vec->size == vec->capacity)
    {
        int ret = vector_resize(vec);
        if(ret)
        {
            return ret;
        }
    }

    void *new_element = malloc(vec->element_size);
    if (new_element == NULL)
    {
        return 1;
    }
    memcpy(new_element, element, vec->element_size);
    vec->array[vec->size++] = new_element;

    return 0;
}

inline void vector_get_element(vector_t *vec, int index, void **out_element)
{
    *out_element = vec->array[index];
}

void vector_remove_element(vector_t *vec, int index)
{
    if (vec->size == 0) 
    {
        return;
    }

    free(vec->array[index]);

    if (index < vec->size - 1) 
    {
        vec->array[index] = vec->array[vec->size - 1];
    }

    vec->size--;
}

void vector_free(vector_t *vec)
{
    for (int i = 0; i < vec->size; i++)
    {
        free(vec->array[i]);
    }

    free(vec->array);
}
