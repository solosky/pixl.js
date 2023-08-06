
#include "mui_scene_dispatcher.h"

#include "settings_scene.h"
#include "boards_defines.h"


#define ADD_SCENE(prefix, name, id) \
    void prefix##_scene_##name##_on_enter(void*); \
    void prefix##_scene_##name##_on_exit(void* context); 
#include "settings_scene_config.h"
#undef ADD_SCENE

const mui_scene_t settings_scene_defines[] = {

#define ADD_SCENE(prefix, name, id) \
    { \
        .scene_id = (SETTINGS_SCENE_##id), \
        .enter_cb = prefix##_scene_##name##_on_enter, \
        .exit_cb = prefix##_scene_##name##_on_exit \
    }, \

#include "settings_scene_config.h"
#undef ADD_SCENE

};