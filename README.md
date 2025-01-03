# csimple-ecs

> A simple Entity-Component-System in pure C99.

**THE PROJECT IS STILL UNDER DEVELOPMENT AND MAY BE UNSTABLE**

Remaining task:
- Unregister system

## Example of Usage

Include the right headers
```C
#include "ecs/ecs.h"
#include "ecs/ecs_err.h"
```

Declare components.
```C
typedef struct
{
    float x, y, z;
} transform_t;

typedef struct
{
    float vx, vy, vz;
} rigidbody_t;
```

Define systems.
```C
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
        ...
    }

    return ECS_OK;
}
```

Before all, initialize the library with `ecs_init()`. Then, create a scene. Each scene has its own components, entities and systems.
```C
ecs_scene_t scene;
ecs_create_scene(&scene);
ecs_bind_scene(scene);

ecs_register_component(transform_t);
ecs_register_component(rigidbody_t);

ecs_signature_t signature;
ecs_create_signature(&signature, transform_t, rigidbody_t);
ecs_register_system(physics_system, ECS_SYSTEM_ON_UPDATE, signature);

float dt = 0.1f;
ecs_set_system_parameters(physics_system, &dt);

ecs_entity_t player;
ecs_create_entity(&player);
ecs_add_component(player, transform_t, &((transform_t){ .x=1, .y=2, .z=3 }));
ecs_add_component(player, rigidbody_t, &((rigidbody_t){ .vx=1 }));
```

You can retrieve back your entity's components with:
```C
transform_t *transform;
rigidbody_t *rb;
ecs_get_component(player, transform_t, &transform);
ecs_get_component(player, rigidbody_t, &rb);
```

Call systems that have `ECS_SYSTEM_ON_UPDATE` event in the gameloop.
```C
while (1)
{
    ecs_listen_systems(ECS_SYSTEM_ON_UPDATE);
    ecs_get_system_status(physics_system, &ret);
    ECS_CHECK_ERROR(ret, "Error in the system : ");

    printf("Transform : x=%.2f, y=%.2f, z=%.2f\n",
            transform->x, transform->y, transform->z);
}
```

Free the memory with `ecs_terminate()`

## License

This project is licensed under the MIT License. See the full license text [here](./LICENSE).tity-Component-System in pure C99.
