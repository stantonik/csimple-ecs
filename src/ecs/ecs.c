/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : ecs
 * @created     : Jeudi jan 02, 2025 01:05:34 CET
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
#include "../src/utils/itoi_map.h"
#include "../src/utils/stoi_map.h"
#include "../src/utils/vector.h"
#include <stddef.h>

#include "stdio.h"

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef struct
{
    ecs_entity_t entity;
    ecs_signature_t signature;
} entity_info_t;

typedef struct
{
    size_t size;
    void *array;
    size_t count;
    
    itoi_map_t entity_to_index_map;
    itoi_map_t index_to_entity_map;
} component_info_t;

typedef struct
{
    ecs_system_t system;
    void *args;
    ecs_err_t status;

    ecs_signature_t signature;
    ecs_system_event_t event;

    ecs_entity_t **entities;
    size_t entity_count;
} system_info_t;

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static vector_t entities;
static vector_t recycled_entities;
static ecs_entity_t next_entities;
static itoi_map_t entity_to_index_map;

static component_info_t *components;
static int component_count;
static stoi_map_t component_name_to_index_map;

static vector_t systems;
static stoi_map_t system_name_to_index_map;
static itoi_map_t event_to_system_index_map;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t ecs_init()
{
    ecs_err_t ret = ECS_OK;

    next_entities = 0;

    component_count = 0;
    components = malloc(sizeof(*components) * ECS_MAX_COMPONENTS);

    ret |= components == NULL;
    ret |= vector_init(&entities, sizeof(entity_info_t), 1);
    ret |= vector_init(&recycled_entities, sizeof(entity_info_t), 1);
    ret |= vector_init(&systems, sizeof(system_info_t), 1);
    if (ret != ECS_OK)
    {
        return ECS_ERR_MEM;
    }

    stoi_map_init(&component_name_to_index_map);
    stoi_map_init(&system_name_to_index_map);
    itoi_map_init(&entity_to_index_map);
    itoi_map_init(&event_to_system_index_map);
    for (int i = 0; i < ECS_MAX_COMPONENTS; ++i)
    {
        itoi_map_init(&components[i].entity_to_index_map);
        itoi_map_init(&components[i].index_to_entity_map);
    }

    return ECS_OK;
}

ecs_err_t ecs_terminate()
{
    vector_free(&entities);
    vector_free(&recycled_entities);

    for (int i = 0; i < ECS_MAX_COMPONENTS; ++i)
    {
        free(components[i].array);
        itoi_map_destroy(&components[i].entity_to_index_map);
        itoi_map_destroy(&components[i].index_to_entity_map);
    }
    free(components);
    components = NULL;

    for (int i = 0; i < systems.size; ++i)
    {
        system_info_t *sys;
        vector_get_element(&systems, i, (void **)&sys);
        free(sys->entities);
    }
    vector_free(&systems);

    stoi_map_destroy(&component_name_to_index_map);
    stoi_map_destroy(&system_name_to_index_map);
    itoi_map_destroy(&entity_to_index_map);
    itoi_map_destroy(&event_to_system_index_map);

    return ECS_OK;
}

ecs_err_t ecs_create_entity(ecs_entity_t *entity)
{
    ecs_entity_t nentity;
    if (recycled_entities.size > 0)
    {
        entity_info_t *p;
        vector_get_element(&recycled_entities, 0, (void **)&p);
        nentity = p->entity;

        vector_remove_element(&recycled_entities, 0);
    }
    else
    {
        nentity = next_entities++;
    }

    vector_add_element(&entities, &nentity);
    itoi_map_insert(&entity_to_index_map, nentity, entities.size - 1);

    return ECS_OK;
}

ecs_err_t ecs_delete_entity(ecs_entity_t entity)
{
    int del_entity_ind;
    if (!itoi_map_get(&entity_to_index_map, entity, &del_entity_ind))
    {
        return ECS_ERR_NULL;
    }

    vector_remove_element(&entities, del_entity_ind);
    itoi_map_remove(&entity_to_index_map, entity);
    vector_add_element(&recycled_entities, &entity);

    return ECS_OK;
}
