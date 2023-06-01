#ifndef AMIIBO_SCENE_H
#define AMIIBO_SCENE_H

#include "mui_scene_dispatcher.h"

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) BLE_SCENE_##id,
typedef enum {
#include "ble_scene_config.h"
    BLE_SCENE_MAX,
} ble_scene_id_t;
#undef ADD_SCENE

extern const mui_scene_t ble_scene_defines[];




#endif 