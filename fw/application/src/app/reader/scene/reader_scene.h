#ifndef READER_SCENE_H
#define READER_SCENE_H

#include "mui_scene_dispatcher.h"

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) READER_SCENE_##id,
typedef enum {
#include "reader_scene_config.h"
    READER_SCENE_MAX,
} ble_scene_id_t;
#undef ADD_SCENE

extern const mui_scene_t reader_scene_defines[];




#endif 