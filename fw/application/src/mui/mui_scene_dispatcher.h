#ifndef MUI_SCENE_DISPATCHER_H
#define MUI_SCENE_DISPATCHER_H

#include "mui_defines.h"
#include <stdint.h>

#include "m-array.h"
typedef void (*mui_scene_enter_cb_t)(void *user_data);
typedef void (*mui_scene_exit_cb_t)(void *user_data);

ARRAY_DEF(scene_id_stack, uint32_t, M_DEFAULT_OPLIST);

typedef struct {
    uint32_t scene_id;
    mui_scene_enter_cb_t enter_cb;
    mui_scene_exit_cb_t exit_cb;
} mui_scene_t;

typedef struct {
    void *user_data;
    const mui_scene_t *p_scene_defines;
    uint32_t scene_num;
    uint32_t default_scene_id;
    scene_id_stack_t scene_id_stack;
} mui_scene_dispatcher_t;

mui_scene_dispatcher_t* mui_scene_dispatcher_create();
void mui_scene_dispatcher_free(mui_scene_dispatcher_t* p_dispatcher);

void mui_scene_dispatcher_set_scene_defines(mui_scene_dispatcher_t *p_dispatcher, const mui_scene_t *p_scene_defines,
                                            uint32_t scene_num);
void mui_scene_dispatcher_set_user_data(mui_scene_dispatcher_t *p_dispatcher, void *user_data);
void mui_scene_dispatcher_next_scene(mui_scene_dispatcher_t *p_dispatcher, uint32_t scene_id);
void mui_scene_dispatcher_previous_scene(mui_scene_dispatcher_t *p_dispatcher);

uint32_t mui_scene_dispatcher_current_scene(mui_scene_dispatcher_t *p_dispatcher);

void inline mui_mui_scene_dispatcher_set_default_scene_id(mui_scene_dispatcher_t *p_dispatcher, uint32_t default_scene_id) {
    p_dispatcher->default_scene_id = default_scene_id;
}

#endif