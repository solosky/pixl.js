#ifndef AMIIDB_SCENE_H
#define AMIIDB_SCENE_H

#include "mui_scene_dispatcher.h"

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) AMIIDB_SCENE_##id,
typedef enum {
#include "amiidb_scene_config.h"
    AMIIDB_SCENE_MAX,
} amiidb_scene_id_t;
#undef ADD_SCENE

extern const mui_scene_t amiidb_scene_defines[];




#endif 