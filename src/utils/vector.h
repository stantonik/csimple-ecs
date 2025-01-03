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
// Function Prototypes
//------------------------------------------------------------------------------
extern int vector_init(vector_t *vec, size_t element_size, size_t initial_capacity);
static inline void vector_free(vector_t *vec);

extern int vector_shrink(vector_t *vec);
extern int vector_reserve(vector_t *vec, size_t additionnal_capacity);
extern int vector_push_back(vector_t *vec, void *element);
static inline void vector_get(vector_t *vec, unsigned int index, void **output);
static inline void vector_get_copy(vector_t *vec, unsigned int index, void *output);
static inline void vector_remove(vector_t *vec, unsigned int index);

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------
inline void vector_get(vector_t *vec, unsigned int index, void **output)
{
    *output = ((char *)vec->data + index * vec->element_size);
}

inline void vector_get_copy(vector_t *vec, unsigned int index, void *output)
{
    memcpy(output, (char *)vec->data + index * vec->element_size, vec->element_size);
}

inline void vector_remove(vector_t *vec, unsigned int index)
{
    memcpy((char *)vec->data + index * vec->element_size, (char *)vec->data + (vec->size - 1) * vec->element_size, vec->element_size);
    --vec->size;
}

inline void vector_free(vector_t *vec)
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

