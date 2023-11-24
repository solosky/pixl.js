#ifndef CHAMELEON_SCENE_H
#define CHAMELEON_SCENE_H

#include "mui_scene_dispatcher.h"

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) CHAMELEON_SCENE_##id,
typedef enum {
#include "chameleon_scene_config.h"
    CHAMELEON_SCENE_MAX,
} ble_scene_id_t;
#undef ADD_SCENE

extern const mui_scene_t chameleon_scene_defines[];




#endif 