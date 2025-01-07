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
#include "../src/utils/uiptrtoi_map.h"
#include "../src/utils/stoi_map.h"
#include "../src/utils/vector.h"
#include "../src/utils/set.h"
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

    set_t entities_s;
    vector_t entities_v;
} system_info_t;

typedef struct
{
    ecs_scene_t scene;

    vector_t entities;
    vector_t recycled_entities;
    ecs_entity_t next_entities;
    itoi_map_t entity_to_index_map;

    component_info_t *components;
    int component_count;
    stoi_map_t component_name_to_index_map;

    vector_t systems;
    uiptrtoi_map_t system_to_index_map;

    vector_t on_init_system_indices;
    vector_t on_update_system_indices;
    vector_t on_end_system_indices;
} scene_info_t;

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------
static vector_t scenes;
static vector_t recycled_scene_ids;
static ecs_scene_t next_scene_id;
static itoi_map_t scene_to_index_map;
static scene_info_t *cs = NULL;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
static ecs_err_t remove_component_by_index(ecs_entity_t entity, uint8_t index);

//------------------------------------------------------------------------------
// Function Implementations
//------------------------------------------------------------------------------
ecs_err_t ecs_init()
{
    cs = NULL;
    next_scene_id = 1;
    vector_init(&scenes, sizeof(scene_info_t), 1);
    vector_init(&recycled_scene_ids, sizeof(ecs_scene_t), 1);
    itoi_map_init(&scene_to_index_map);

    return ECS_OK;
}

ecs_err_t ecs_terminate()
{
    for (int i = 0; i < scenes.size; ++i)
    {
        vector_get(&scenes, i, (void **)&cs);
        ecs_free_scene();
    }

    return ECS_OK;
}

ecs_err_t ecs_create_scene(ecs_scene_t *scene)
{
    ecs_err_t ret = ECS_OK;
    scene_info_t nscene = { 0 };
    if (recycled_scene_ids.size > 0)
    {
        vector_get_copy(&recycled_scene_ids, 0, &nscene.scene);
        vector_remove(&recycled_scene_ids, 0);
    }
    else
    {
        nscene.scene = next_scene_id++;
    }

    nscene.components = calloc(ECS_MAX_COMPONENTS, sizeof(*nscene.components) * ECS_MAX_COMPONENTS);

    ret |= nscene.components == NULL;
    ret |= vector_init(&nscene.entities, sizeof(entity_info_t), 1);
    ret |= vector_init(&nscene.recycled_entities, sizeof(ecs_entity_t), 1);

    ret |= vector_init(&nscene.systems, sizeof(system_info_t), 1);
    ret |= vector_init(&nscene.on_init_system_indices, sizeof(int), 1);
    ret |= vector_init(&nscene.on_update_system_indices, sizeof(int), 1);
    ret |= vector_init(&nscene.on_end_system_indices, sizeof(int), 1);
    if (ret != ECS_OK)
    {
        return ECS_ERR_MEM;
    }

    stoi_map_init(&nscene.component_name_to_index_map);
    uiptrtoi_map_init(&nscene.system_to_index_map);
    itoi_map_init(&nscene.entity_to_index_map);

    vector_push_back(&scenes, &nscene);
    itoi_map_insert(&scene_to_index_map, nscene.scene, scenes.size - 1);
    *scene = nscene.scene;

    return ECS_OK;
}

ecs_err_t ecs_free_scene()
{
    vector_free(&cs->entities);
    vector_free(&cs->recycled_entities);

    for (int i = 0; i < ECS_MAX_COMPONENTS; ++i)
    {
        vector_free(&cs->components[i].array);
        itoi_map_destroy(&cs->components[i].entity_to_index_map);
        itoi_map_destroy(&cs->components[i].index_to_entity_map);
    }
    free(cs->components);
    cs->components = NULL;

    for (int i = 0; i < cs->systems.size; ++i)
    {
        system_info_t *sys;
        vector_get(&cs->systems, i, (void **)&sys);
        vector_free(&sys->entities_v);
        set_free(&sys->entities_s);
    }
    vector_free(&cs->systems);
    vector_free(&cs->on_init_system_indices);
    vector_free(&cs->on_update_system_indices);
    vector_free(&cs->on_end_system_indices);

    stoi_map_destroy(&cs->component_name_to_index_map);
    uiptrtoi_map_destroy(&cs->system_to_index_map);
    itoi_map_destroy(&cs->entity_to_index_map);

    int del_scene_id;
    itoi_map_get(&scene_to_index_map, cs->scene, &del_scene_id);
    itoi_map_remove(&scene_to_index_map, cs->scene);
    vector_remove(&scenes, del_scene_id);
    vector_push_back(&recycled_scene_ids, &cs->scene);
    cs = NULL;

    return ECS_OK;
}

ecs_err_t ecs_bind_scene(ecs_scene_t scene)
{
    int ind;
    if (!itoi_map_get(&scene_to_index_map, scene, &ind))
    {
        cs = NULL;
        return ECS_ERR_NULL;
    }

    vector_get(&scenes, ind, (void **)&cs);

    return ECS_OK;
}

ecs_err_t ecs_register_component_by_name(const char *name, size_t size)
{
    if (stoi_map_get(&cs->component_name_to_index_map, name, NULL))
    {
        return ECS_ERR_EXISTS;
    }

    for (int i = 0; i < ECS_MAX_COMPONENTS; ++i)
    {
        if (cs->components[i].array.element_size == 0)
        {
            stoi_map_insert(&cs->component_name_to_index_map, name, i);
            cs->components[i].array.element_size = size;

            vector_init(&cs->components[i].array, size, 1);
            itoi_map_init(&cs->components[i].entity_to_index_map);
            itoi_map_init(&cs->components[i].index_to_entity_map);
            return ECS_OK;
        }
    }

    return ECS_ERR_MEM;
}

ecs_err_t ecs_unregister_component_by_name(const char *name)
{
    int comp_info_ind;
    if (!stoi_map_get(&cs->component_name_to_index_map, name, &comp_info_ind))
    {
        return ECS_ERR_NULL;
    }

    component_info_t *comp_info = &cs->components[comp_info_ind];

    vector_free(&comp_info->array);
    itoi_map_destroy(&comp_info->entity_to_index_map);
    itoi_map_destroy(&comp_info->index_to_entity_map);

    // TODO: edit mapping for the last component type


    stoi_map_remove(&cs->component_name_to_index_map, name);

    // Remove the component from all entities
    for (int i = 0; i < cs->entities.size; ++i)
    {
        entity_info_t *entity_info;
        vector_get(&cs->entities, i, (void **)&entity_info);

        entity_info->signature &= ~(1 << comp_info_ind);
    }

    return ECS_OK;
}

ecs_err_t ecs_create_entity(ecs_entity_t *entity)
{
    entity_info_t nentity = {  };
    if (cs->recycled_entities.size > 0)
    {
        vector_get_copy(&cs->recycled_entities, 0, &nentity.entity);
        vector_remove(&cs->recycled_entities, 0);
    }
    else
    {
        nentity.entity = cs->next_entities++;
    }

    vector_push_back(&cs->entities, &nentity);
    itoi_map_insert(&cs->entity_to_index_map, nentity.entity, cs->entities.size - 1);
    *entity = nentity.entity;

    return ECS_OK;
}

ecs_err_t ecs_delete_entity(ecs_entity_t entity)
{
    int del_entity_ind, last_entity_ind;
    if (!itoi_map_get(&cs->entity_to_index_map, entity, &del_entity_ind))
    {
        return ECS_ERR_NULL;
    }

    last_entity_ind = cs->entities.size - 1;

    entity_info_t *del_entity_info, *last_entity_info;
    vector_get(&cs->entities, del_entity_ind, (void **)&del_entity_info);

    // Remove all components from entity
    for (int i = 0; i < ECS_MAX_COMPONENTS; ++i) 
    {
        if (del_entity_info->signature & (1 << i))
        {
            remove_component_by_index(entity, i);
        }
    }

    if (del_entity_ind != last_entity_ind)
    {
        vector_get(&cs->entities, last_entity_ind, (void **)&last_entity_info);
        memcpy(del_entity_info, last_entity_info, sizeof(entity_info_t));
        itoi_map_insert(&cs->entity_to_index_map, last_entity_info->entity, del_entity_ind);
    }

    vector_remove(&cs->entities, last_entity_ind);
    itoi_map_remove(&cs->entity_to_index_map, entity);
    vector_push_back(&cs->recycled_entities, &entity);

    return ECS_OK;
}

ecs_err_t ecs_add_component_by_name(ecs_entity_t entity, const char *name, void *default_value)
{
    int entity_ind, comp_info_ind;
    if (!itoi_map_get(&cs->entity_to_index_map, entity, &entity_ind) || 
            !stoi_map_get(&cs->component_name_to_index_map, name, &comp_info_ind)) 
    {
        return ECS_ERR_NULL;
    }

    // Check if entity already has the component
    component_info_t *comp_info = &cs->components[comp_info_ind];
    if (itoi_map_get(&comp_info->entity_to_index_map, entity, NULL))
    {
        return ECS_ERR_EXISTS;
    }

    entity_info_t *entity_info;
    vector_get(&cs->entities, entity_ind, (void **)&entity_info);

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
    for (int i = 0; i < cs->systems.size; ++i)
    {
        system_info_t *sys_info;
        vector_get(&cs->systems, i, (void **)&sys_info);
        if ((sys_info->signature & entity_info->signature) == sys_info->signature)
        {
            if (!set_contains(&sys_info->entities_s, entity))
            {
                vector_push_back(&sys_info->entities_v, &entity);
                set_insert(&sys_info->entities_s, entity);
            }
        }
    }

    return ECS_OK;
}

ecs_err_t remove_component_by_index(ecs_entity_t entity, uint8_t index)
{
    int entity_ind;
    if (!itoi_map_get(&cs->entity_to_index_map, entity, &entity_ind))
    {
        return ECS_ERR_NULL;
    }

    entity_info_t *entity_info;
    vector_get(&cs->entities, entity_ind, (void **)&entity_info);

    // Check if entity has the component
    if (!(entity_info->signature & (1 << index)))
    {
        return ECS_ERR_NULL;
    }

    // Remove component from component array
    // Step 1 - get del comp ind and last comp ind
    component_info_t *comp_info = &cs->components[index];
    int del_comp_ind, last_comp_ind;
    itoi_map_get(&comp_info->entity_to_index_map, entity, &del_comp_ind);
    last_comp_ind = comp_info->array.size - 1;

    // Only perform shift if necessary
    int last_entity;
    if (del_comp_ind != last_comp_ind)
    {
        void *del_comp, *last_comp;
        vector_get(&comp_info->array, del_comp_ind, &del_comp);
        vector_get(&comp_info->array, last_comp_ind, &last_comp);
        memcpy(del_comp, last_comp, comp_info->array.element_size);

        // Update mappings
        itoi_map_get(&comp_info->index_to_entity_map, last_comp_ind, &last_entity);
        itoi_map_insert(&comp_info->entity_to_index_map, last_entity, del_comp_ind);
        itoi_map_insert(&comp_info->index_to_entity_map, del_comp_ind, last_entity);
    }

    // Remove last component and entity mapping
    vector_remove(&comp_info->array, last_comp_ind);
    itoi_map_remove(&comp_info->index_to_entity_map, last_comp_ind);
    itoi_map_remove(&comp_info->entity_to_index_map, entity);

    // Remove entity from the corresponding system
    for (int i = 0; i < cs->systems.size; ++i)
    {
        system_info_t *sys_info;
        vector_get(&cs->systems, i, (void **)&sys_info);
        if ((sys_info->signature & entity_info->signature) == sys_info->signature)
        {
            vector_remove(&sys_info->entities_v, i);
            set_delete(&sys_info->entities_s, entity);
        }
    }

    return ECS_OK;
}

inline ecs_err_t ecs_remove_component_by_name(ecs_entity_t entity, const char *name)
{
    int comp_info_ind;
    if (!stoi_map_get(&cs->component_name_to_index_map, name, &comp_info_ind)) 
    {
        return ECS_ERR_NULL;
    }

    return remove_component_by_index(entity, comp_info_ind);
}

ecs_err_t ecs_get_component_by_name(ecs_entity_t entity, const char *name, void **dest)
{
    int entity_ind, comp_info_ind;
    if (!itoi_map_get(&cs->entity_to_index_map, entity, &entity_ind) || 
            !stoi_map_get(&cs->component_name_to_index_map, name, &comp_info_ind)) 
    {
        return ECS_ERR_NULL;
    }

    component_info_t *comp_info = &cs->components[comp_info_ind];

    int comp_ind;
    itoi_map_get(&comp_info->entity_to_index_map, entity, &comp_ind);
    vector_get(&comp_info->array, comp_ind, dest);

    return ECS_OK;
}

bool ecs_entity_has_component_by_name(ecs_entity_t entity, const char *name)
{
    int entity_ind, comp_info_id;
    if (!itoi_map_get(&cs->entity_to_index_map, entity, &entity_ind) || 
            !stoi_map_get(&cs->component_name_to_index_map, name, &comp_info_id)) 
    {
        return false;
    }

    entity_info_t *entity_info;
    vector_get(&cs->entities, entity_ind, (void **)&entity_info);

    return (entity_info->signature & (1 << comp_info_id)) != 0;
}

ecs_err_t ecs_register_system(ecs_system_t system, ecs_signature_t signature, ecs_system_event_t event)
{
    if (uiptrtoi_map_get(&cs->system_to_index_map, (uintptr_t)system, NULL))
    {
        return ECS_ERR_EXISTS;
    }

    vector_push_back(&cs->systems, &(system_info_t){ .system=system, .event=event, .signature=signature });
    uiptrtoi_map_insert(&cs->system_to_index_map, (uintptr_t)system, cs->systems.size - 1);

    system_info_t *sys_info;
    vector_get(&cs->systems, cs->systems.size - 1, (void **)&sys_info);

    vector_init(&sys_info->entities_v, sizeof(ecs_entity_t), 1);
    set_init(&sys_info->entities_s);


    switch (event)
    {
        case ECS_SYSTEM_ON_INIT:
            vector_push_back(&cs->on_init_system_indices, &((int){ cs->systems.size - 1 }));
            break;
        case ECS_SYSTEM_ON_UPDATE:
            vector_push_back(&cs->on_update_system_indices, &((int){ cs->systems.size - 1 }));
            break;
        case ECS_SYSTEM_ON_END:
            vector_push_back(&cs->on_end_system_indices, &((int){ cs->systems.size - 1 }));
            break;
        default: break;
    }

    return ECS_OK;
}

ecs_err_t ecs_unregister_system(ecs_system_t system)
{
    int sys_info_id;
    if (!uiptrtoi_map_get(&cs->system_to_index_map, (uintptr_t)system, &sys_info_id))
    {
        return ECS_ERR_NULL;
    }

    system_info_t *sys_info;
    vector_get(&cs->systems, sys_info_id, (void **)&sys_info);

    vector_free(&sys_info->entities_v);
    set_free(&sys_info->entities_s);
    vector_remove(&cs->systems, sys_info_id);

    // Edit mappings
    // TODO
    uiptrtoi_map_remove(&cs->system_to_index_map, (uintptr_t)system);

    return ECS_OK;
}

ecs_err_t ecs_set_system_parameters(ecs_system_t system, void *args)
{
    int sys_info_id;
    if (!uiptrtoi_map_get(&cs->system_to_index_map, (uintptr_t)system, &sys_info_id))
    {
        return ECS_ERR_NULL;
    }

    system_info_t *sys_info;
    vector_get(&cs->systems, sys_info_id, (void **)&sys_info);

    sys_info->args = args;

    return ECS_OK;
}

ecs_err_t ecs_call_system(ecs_system_t system)
{
    int sys_info_id;
    if (!uiptrtoi_map_get(&cs->system_to_index_map, (uintptr_t)system, &sys_info_id))
    {
        return ECS_ERR_NULL;
    }

    system_info_t *sys_info;
    vector_get(&cs->systems, sys_info_id, (void **)&sys_info);

    sys_info->system((ecs_entity_t *)sys_info->entities_v.data, sys_info->entities_v.size, sys_info->args);

    return ECS_OK;
}

ecs_err_t ecs_listen_systems(ecs_system_event_t event)
{
    int sys_id;
    system_info_t *sys_info;
    vector_t *sys_indices;
    switch (event) 
    {
        case ECS_SYSTEM_ON_INIT:
            sys_indices = &cs->on_init_system_indices;
            break;
        case ECS_SYSTEM_ON_UPDATE:
            sys_indices = &cs->on_update_system_indices;
            break;
        case ECS_SYSTEM_ON_END:
            sys_indices = &cs->on_end_system_indices;
            break;
        default:
            return ECS_ERR_NULL;
    }

    for (int i = 0; i < sys_indices->size; ++i)
    {
        vector_get_copy(sys_indices, i, &sys_id);
        vector_get(&cs->systems, sys_id, (void **)&sys_info);
        sys_info->status = sys_info->system((ecs_entity_t *)sys_info->entities_v.data, sys_info->entities_v.size, sys_info->args);
    }

    return ECS_OK;
}

ecs_err_t ecs_create_signature_by_names(ecs_signature_t *signature, const char *names)
{
    char *names_cpy = strdup(names);
    if (names_cpy == NULL)
    {
        return ECS_ERR_NULL;
    }

    ecs_signature_t sign = 0;
    char *name = strtok(names_cpy, ",");
    while (name != NULL)
    {
        while (*name == ' ') name++;
        int ind;
        if (!stoi_map_get(&cs->component_name_to_index_map, name, &ind))
        {
            free(names_cpy);
            return ECS_ERR_NULL;
        }

        sign |= (1 << ind);
        name = strtok(NULL, ",");
    }

    *signature = sign;

    free(names_cpy);

    return ECS_OK;
}

ecs_err_t ecs_get_system_status(ecs_system_t system, ecs_err_t *ret)
{
    int sys_info_id;
    if (!uiptrtoi_map_get(&cs->system_to_index_map, (uintptr_t)system, &sys_info_id))
    {
        return ECS_ERR_NULL;
    }

    system_info_t *sys_info;
    vector_get(&cs->systems, sys_info_id, (void **)&sys_info);

    *ret = sys_info->status;

    return ECS_OK;
}
