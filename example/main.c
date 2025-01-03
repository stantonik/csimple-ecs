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
    ecs_init();

    ecs_err_t ret;

    ecs_entity_t player;
    ecs_create_entity(&player);

    ret = ecs_register_component(transform_t);
    ret |= ecs_register_component(rigidbody_t);
    ECS_CHECK_ERROR(ret, "Failed to register components : ");
    
    ret |= ecs_add_component(player, transform_t, &((transform_t){ .x=1, .y=2, .z=3 }));
    ret |= ecs_add_component(player, rigidbody_t, &((rigidbody_t){ .vx=1 }));
    ECS_CHECK_ERROR(ret, "Failed to add components : ");

    transform_t *transform;
    rigidbody_t *rb;
    ret |= ecs_get_component(player, transform_t, &transform);
    ret |= ecs_get_component(player, rigidbody_t, &rb);
    ECS_CHECK_ERROR(ret, "Failed to get components : ");

    printf("Transform : x=%.2f, y=%.2f, z=%.2f\n", transform->x, transform->y, transform->z);
    printf("Rigidbody: vx=%.2f, vy=%.2f, vz=%.2f\n", rb->vx, rb->vy, rb->vz);

    ecs_signature_t signature;
    ret |= ecs_create_signature(&signature, transform_t, rigidbody_t);
    ECS_CHECK_ERROR(ret, "Failed to create signature : ");

    ret |= ecs_register_system(physics_system, ECS_SYSTEM_ON_UPDATE, signature);
    ECS_CHECK_ERROR(ret, "Failed to register physics system : ");

    float dt = 0.1f;
    ecs_set_system_parameters(physics_system, &dt);

    while (1)
    {
        ecs_listen_systems(ECS_SYSTEM_ON_UPDATE);
        ecs_get_system_status(physics_system, &ret);
        ECS_CHECK_ERROR(ret, "Error in the system : ");

        printf("Transform : x=%.2f, y=%.2f, z=%.2f\n", transform->x, transform->y, transform->z);
    }

    ecs_terminate();
    return 0;
}
