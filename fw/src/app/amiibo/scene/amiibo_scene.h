#ifndef AMIIBO_SCENE_H
#define AMIIBO_SCENE_H

#include "mui_scene_dispatcher.h"

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) AMIIBO_SCENE_##id,
typedef enum {
#include "amiibo_scene_config.h"
    AMIIBO_SCENE_MAX,
} amiibo_scene_id_t;
#undef ADD_SCENE

extern const mui_scene_t amiibo_scene_defines[];




#endif 