/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : vector
 * @created     : Jeudi jan 02, 2025 02:51:21 CET
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

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct {
    void **array;
    int size;
    int capacity;
    size_t element_size;
} vector_t;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

extern int vector_init(vector_t *vec, size_t element_size, int initial_capacity);
extern int vector_resize(vector_t *vec);
extern int vector_shrink(vector_t *vec);
extern int vector_add_element(vector_t *vec, void *element);
extern void vector_get_element(vector_t *vec, int index, void **out_element);
extern void vector_remove_element(vector_t *vec, int index);
extern void vector_free(vector_t *vec);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* VECTOR_H */

