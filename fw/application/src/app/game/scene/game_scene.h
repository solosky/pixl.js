#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "mui_scene_dispatcher.h"

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) GAME_SCENE_##id,
typedef enum {
#include "game_scene_config.h"
    GAME_SCENE_MAX,
} ble_scene_id_t;
#undef ADD_SCENE

extern const mui_scene_t game_scene_defines[];




#endif 