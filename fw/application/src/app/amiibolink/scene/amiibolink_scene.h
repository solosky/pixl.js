#ifndef AMIIBOLINK_SCENE_H
#define AMIIBOLINK_SCENE_H

#include "mui_scene_dispatcher.h"

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) AMIIBOLINK_SCENE_##id,
typedef enum {
#include "amiibolink_scene_config.h"
    AMIIBOLINK_SCENE_MAX,
} ble_scene_id_t;
#undef ADD_SCENE

extern const mui_scene_t amiibolink_scene_defines[];




#endif 