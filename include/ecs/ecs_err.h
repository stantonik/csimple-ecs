/**
 * @author      : stanleyarn (stanleyarn@$HOSTNAME)
 * @file        : ecs_err
 * @created     : Jeudi jan 02, 2025 01:15:53 CET
 */

#ifndef ECS_ERR_H
#define ECS_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_OK 0
#define ECS_ERR -1
#define ECS_ERR_NULL 1
#define ECS_ERR_MEM 2
#define ECS_ERR_EXISTS 3

#define ECS_CHECK_ERROR(x, format, ...) \
    do { \
        int __ret = (x); \
        if (__ret != ECS_OK) { \
            printf(format"%i\n", ##__VA_ARGS__, __ret); \
            return __ret; \
        } \
    } while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* ECS_ERR_H */

