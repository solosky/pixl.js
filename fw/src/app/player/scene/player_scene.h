#ifndef PLAYER_SCENE_H
#define PLAYER_SCENE_H

#include "mui_scene_dispatcher.h"

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) PLAYER_SCENE_##id,
typedef enum {
#include "player_scene_config.h"
    PLAYER_SCENE_MAX,
} ble_scene_id_t;
#undef ADD_SCENE

extern const mui_scene_t player_scene_defines[];




#endif 