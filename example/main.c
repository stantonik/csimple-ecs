/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : main
 * @created     : Jeudi jan 02, 2025 01:14:14 CET
 */

#include "ecs/ecs.h"

int main(void)
{
    ecs_init();


    for (int i = 0; i < 10; ++i)
    {
        ecs_entity_t player;
        ecs_create_entity(&player);
    }

    for (int i = 0; i < 5; ++i)
    {
        ecs_delete_entity(i);
    }

    for (int i = 0; i < 10; ++i)
    {
        ecs_entity_t player;
        ecs_create_entity(&player);
    }

    ecs_terminate();
    return 0;
}
