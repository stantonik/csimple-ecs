/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : main
 * @created     : Jeudi jan 02, 2025 01:14:14 CET
 */

#include "ecs/ecs.h"
#include "ecs/ecs_err.h"

typedef struct
{
    float x, y, z;
} transform_t;

typedef struct
{
    float vx, vy, vz;
} rigidbody_t;

ecs_err_t physics_system(ecs_entity_t *it, int count, void *args)
{
    float dt = ((float *)args)[0];

    for (int i = 0; i < count; ++i)
    {
        transform_t *transform;
        rigidbody_t *rb;
        ecs_get_component(it[i], transform_t, &transform);
        ecs_get_component(it[i], rigidbody_t, &rb);

        transform->x += rb->vx * dt;
        transform->y += rb->vy * dt;
        transform->z += rb->vz * dt;
    }

    return ECS_OK;
}

int main(void)
{
    ecs_err_t ret = ECS_OK;

    ecs_init(); // Has to be done first

    ecs_scene_t scene;
    ecs_create_scene(&scene);
    ecs_bind_scene(scene); // Has to be done before any operation

    ecs_register_component(transform_t);
    ecs_register_component(rigidbody_t);

    // Register physic system
    ecs_signature_t signature;
    ecs_create_signature(&signature, transform_t, rigidbody_t);
    ret |= ecs_register_system(physics_system, signature, ECS_SYSTEM_ON_UPDATE);
    ECS_CHECK_ERROR(ret, "Failed to register system : ");

    ecs_entity_t player;
    ecs_create_entity(&player);
    ecs_add_component(player, transform_t, &((transform_t){ .x=1, .y=2, .z=3 }));
    ecs_add_component(player, rigidbody_t, &((rigidbody_t){ .vx=1 }));

    // Get entity components
    transform_t *transform;
    rigidbody_t *rb;
    ret |= ecs_get_component(player, transform_t, &transform);
    ret |= ecs_get_component(player, rigidbody_t, &rb);
    ECS_CHECK_ERROR(ret, "Failed to get components : ");

    float dt = 0.1f;
    ecs_set_system_parameters(physics_system, &dt);
    // Gameloop
    while (1)
    {
        ecs_listen_systems(ECS_SYSTEM_ON_UPDATE);
        ecs_get_system_status(physics_system, &ret);
        ECS_CHECK_ERROR(ret, "Error in the system : ");

        printf("Transform : x=%.2f, y=%.2f, z=%.2f\n",
                transform->x, transform->y, transform->z);
        break;
    }

    ecs_listen_systems(ECS_SYSTEM_ON_END);

    ecs_free_scene();
    ecs_bind_scene(0);

    ecs_terminate(); // Free memory

    return 0;
}
