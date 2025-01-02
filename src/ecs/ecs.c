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
    vector_t array;

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

    vector_t pentities;
    int entity_count;
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
    components = calloc(ECS_MAX_COMPONENTS, sizeof(*components) * ECS_MAX_COMPONENTS);

    ret |= components == NULL;
    ret |= vector_init(&entities, sizeof(entity_info_t), 1);
    ret |= vector_init(&recycled_entities, sizeof(ecs_entity_t), 1);
    ret |= vector_init(&systems, sizeof(system_info_t), 1);
    if (ret != ECS_OK)
    {
        return ECS_ERR_MEM;
    }

    stoi_map_init(&component_name_to_index_map);
    stoi_map_init(&system_name_to_index_map);
    itoi_map_init(&entity_to_index_map);
    itoi_map_init(&event_to_system_index_map);

    return ECS_OK;
}

ecs_err_t ecs_terminate()
{
    vector_free(&entities);
    vector_free(&recycled_entities);

    for (int i = 0; i < ECS_MAX_COMPONENTS; ++i)
    {
        vector_free(&components[i].array);
        itoi_map_destroy(&components[i].entity_to_index_map);
        itoi_map_destroy(&components[i].index_to_entity_map);
    }
    free(components);
    components = NULL;

    for (int i = 0; i < systems.size; ++i)
    {
        system_info_t *sys;
        vector_get(&systems, i, (void **)&sys);
        vector_free(&sys->pentities);
    }
    vector_free(&systems);

    stoi_map_destroy(&component_name_to_index_map);
    stoi_map_destroy(&system_name_to_index_map);
    itoi_map_destroy(&entity_to_index_map);
    itoi_map_destroy(&event_to_system_index_map);

    return ECS_OK;
}

ecs_err_t ecs_register_component_by_name(const char *name, size_t size)
{
    if (stoi_map_get(&component_name_to_index_map, name, NULL))
    {
        return ECS_ERR_EXISTS;
    }

    for (int i = 0; i < ECS_MAX_COMPONENTS; ++i)
    {
        if (components[i].array.element_size == 0)
        {
            stoi_map_insert(&component_name_to_index_map, name, i);
            components[i].array.element_size = size;

            vector_init(&components[i].array, size, 1);
            itoi_map_init(&components[i].entity_to_index_map);
            itoi_map_init(&components[i].index_to_entity_map);
            return ECS_OK;
        }
    }

    return ECS_ERR_MEM;
}

ecs_err_t ecs_unregister_component_by_name(const char *name)
{
    int comp_info_ind;
    if (!stoi_map_get(&component_name_to_index_map, name, &comp_info_ind))
    {
        return ECS_ERR_NULL;
    }

    component_info_t *comp_info = &components[comp_info_ind];
    
    vector_free(&comp_info->array);
    itoi_map_destroy(&comp_info->entity_to_index_map);
    itoi_map_destroy(&comp_info->index_to_entity_map);

    stoi_map_remove(&component_name_to_index_map, name);

    // Remove the component from all entities
    for (int i = 0; i < entities.size; ++i)
    {
        entity_info_t *entity_info;
        vector_get(&entities, i, (void **)&entity_info);

        entity_info->signature &= ~(1 << comp_info_ind);
    }

    return ECS_OK;
}

ecs_err_t ecs_create_entity(ecs_entity_t *entity)
{
    ecs_entity_t nentity;
    if (recycled_entities.size > 0)
    {
        vector_get_cpy(&recycled_entities, 0, &nentity);
        vector_remove_ind(&recycled_entities, 0);
    }
    else
    {
        nentity = next_entities++;
    }

    vector_push_back(&entities, &nentity);
    itoi_map_insert(&entity_to_index_map, nentity, entities.size - 1);
    *entity = nentity;

    return ECS_OK;
}

ecs_err_t ecs_delete_entity(ecs_entity_t entity)
{
    int del_entity_ind;
    if (!itoi_map_get(&entity_to_index_map, entity, &del_entity_ind))
    {
        return ECS_ERR_NULL;
    }

    vector_remove_ind(&entities, del_entity_ind);
    itoi_map_remove(&entity_to_index_map, entity);
    vector_push_back(&recycled_entities, &entity);

    return ECS_OK;
}

ecs_err_t ecs_add_component_by_name(ecs_entity_t entity, const char *name, void *default_value)
{
    int entity_ind, comp_info_ind;
    if (!itoi_map_get(&entity_to_index_map, entity, &entity_ind) || 
        !stoi_map_get(&component_name_to_index_map, name, &comp_info_ind)) 
    {
        return ECS_ERR_NULL;
    }

    // Check if entity already has the component
    component_info_t *comp_info = &components[comp_info_ind];
    if (itoi_map_get(&comp_info->entity_to_index_map, entity, NULL))
    {
        return ECS_ERR_EXISTS;
    }

    entity_info_t *entity_info;
    vector_get(&entities, entity_ind, (void **)&entity_info);

    // Set default value or empty value
    if (default_value)
    {
        vector_push_back(&comp_info->array, default_value);
    }
    else
    {
        void *empty_comp = calloc(1, comp_info->array.element_size);
        if (empty_comp == NULL)
        {
            return ECS_ERR_MEM;
        }
        vector_push_back(&comp_info->array, empty_comp);
        free(empty_comp);
    }

    // Append to the mapping
    itoi_map_insert(&comp_info->entity_to_index_map, entity, comp_info->array.size - 1);
    itoi_map_insert(&comp_info->index_to_entity_map, comp_info->array.size - 1, entity);

    entity_info->signature |= (1 << comp_info_ind);

    // Add entity to the corresponding systems
    for (int i = 0; i < systems.size; ++i)
    {
        system_info_t *sys_info;
        vector_get(&systems, i, (void **)&sys_info);
        if (sys_info->signature == entity_info->signature)
        {
            vector_push_back(&sys_info->pentities, &entity);
        }
    }

    return ECS_OK;
}

ecs_err_t ecs_remove_component_by_name(ecs_entity_t entity, const char *name)
{
    int entity_ind, comp_info_ind;
    if (!itoi_map_get(&entity_to_index_map, entity, &entity_ind) || 
        !stoi_map_get(&component_name_to_index_map, name, &comp_info_ind)) 
    {
        return ECS_ERR_NULL;
    }

    entity_info_t *entity_info;
    vector_get(&entities, entity_ind, (void **)&entity_info);

    // Check if entity has the component
    if (!(entity_info->signature & (1 << comp_info_ind)))
    {
        return ECS_ERR_NULL;
    }

    // Remove component from component array
    component_info_t *comp_info = &components[comp_info_ind];
    int del_comp_id;
    itoi_map_get(&comp_info->entity_to_index_map, entity, &del_comp_id);
    vector_remove_ind(&comp_info->array, del_comp_id);

    // Edit mappings
    itoi_map_remove(&comp_info->entity_to_index_map, entity);
    itoi_map_remove(&comp_info->index_to_entity_map, del_comp_id);

    // Remove entity from the corresponding system
    for (int i = 0; i < systems.size; ++i)
    {
        system_info_t *sys_info;
        vector_get(&systems, i, (void **)&sys_info);
        if (sys_info->signature == entity_info->signature)
        {
            vector_remove_ind(&sys_info->pentities, i);
        }
    }

    return ECS_OK;
}

ecs_err_t ecs_get_component_by_name(ecs_entity_t entity, const char *name, void **dest)
{
    int entity_ind, comp_info_ind;
    if (!itoi_map_get(&entity_to_index_map, entity, &entity_ind) || 
        !stoi_map_get(&component_name_to_index_map, name, &comp_info_ind)) 
    {
        return ECS_ERR_NULL;
    }

    component_info_t *comp_info = &components[comp_info_ind];

    int comp_ind;
    itoi_map_get(&comp_info->entity_to_index_map, entity, &comp_ind);
    vector_get(&comp_info->array, comp_ind, dest);

    return ECS_OK;
}
