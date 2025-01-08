/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : ecs
 * @created     : Jeudi jan 02, 2025 00:58:36 CET
 */

#ifndef ECS_H
#define ECS_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define ECS_MAX_COMPONENTS 32

#define ecs_register_component(component) \
    (ecs_register_component_by_name(#component, sizeof(component)) && ((void)sizeof(component), true))

#define ecs_add_component(entity, component, default_value) \
    (ecs_add_component_by_name(entity, #component, (void *)(default_value)) && ((void)sizeof(component), true))

#define ecs_get_component(entity, component, dest) \
    (ecs_get_component_by_name(entity, #component, (void **)(dest)) && ((void)sizeof(component), true))

#define ecs_remove_component(entity, component) \
    (ecs_remove_component_by_name(entity, #component) && ((void)sizeof(component), true))

#define ecs_create_signature(signature, ...) \
    ecs_create_signature_by_names(signature, #__VA_ARGS__)

#define ecs_entity_has_component(entity, component) \
    (ecs_entity_has_component_by_name(entity, #component) && ((void)sizeof(component), true))

//------------------------------------------------------------------------------
// Typedefs and Enums
//------------------------------------------------------------------------------
typedef int ecs_err_t;
typedef uint32_t ecs_entity_t;
typedef uint32_t ecs_scene_t;
typedef uint32_t ecs_signature_t;
typedef ecs_err_t (*ecs_system_t)(ecs_entity_t *, int count, void *args[]);

typedef enum 
{
    ECS_SYSTEM_ON_INIT,
    ECS_SYSTEM_ON_UPDATE,
    ECS_SYSTEM_ON_END,

    ECS_SYSTEM_EVENT_COUNT
} ecs_system_event_t;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
extern ecs_err_t ecs_init();
extern ecs_err_t ecs_terminate();

extern ecs_err_t ecs_create_scene(ecs_scene_t *scene);
extern ecs_err_t ecs_bind_scene(ecs_scene_t scene);
extern ecs_err_t ecs_free_scene();

// Memory optimisations
extern ecs_err_t ecs_reserve_entities(ecs_entity_t max_entities);
extern ecs_err_t ecs_shrink_entities();

extern ecs_err_t ecs_create_entity(ecs_entity_t *entity);
extern ecs_err_t ecs_delete_entity(ecs_entity_t entity);

extern ecs_err_t ecs_register_component_by_name(const char *name, size_t size);
extern ecs_err_t ecs_unregister_component_by_name(const char *name);
extern ecs_err_t ecs_add_component_by_name(ecs_entity_t entity, const char *name, void *default_value);
extern inline ecs_err_t ecs_remove_component_by_name(ecs_entity_t entity, const char *name);
extern ecs_err_t ecs_get_component_by_name(ecs_entity_t entity, const char *name, void **dest);
extern bool ecs_entity_has_component_by_name(ecs_entity_t entity, const char *name);

extern ecs_err_t ecs_create_signature_by_names(ecs_signature_t *signature, const char *names);

extern ecs_err_t ecs_register_system(ecs_system_t system, ecs_signature_t signature, ecs_system_event_t event);
extern ecs_err_t ecs_unregister_system(ecs_system_t system);
extern ecs_err_t ecs_set_system_parameters(ecs_system_t system, int argc, void *args[]);
extern ecs_err_t ecs_call_system(ecs_system_t system);
extern ecs_err_t ecs_listen_systems(ecs_system_event_t event);
extern ecs_err_t ecs_get_system_status(ecs_system_t system, ecs_err_t *ret);

//------------------------------------------------------------------------------
// Inline Functions
//------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* ECS_H */

