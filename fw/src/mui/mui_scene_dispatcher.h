#ifndef MUI_SCENE_DISPATCHER_H
#define MUI_SCENE_DISPATCHER_H

#include <stdint.h>

typedef struct {
    uint32_t scene_id;
    mui_scene_enter_cb_t enter_cb;
    mui_scene_exit_cb_t exit_cb;
} mui_scene_t;

struct mui_scene_dispatcher_s;
typedef struct mui_scene_dispatcher_s mui_scene_dispatcher_t;

typedef void (* mui_scene_enter_cb_t)(mui_scene_dispatcher_t *p_dispatcher);
typedef void (* mui_scene_exit_cb_t)(mui_scene_dispatcher_t *p_dispatcher);


struct mui_scene_dispatcher_s {
    void* user_data;
    mui_scene_t* p_scene_defines;
    uint32_t scene_num;
};


mui_scene_dispatcher_t mui_scene_dispatcher_create();
void mui_scene_dispatcher_set_scene_defines(mui_scene_dispatcher_t* p_dispatcher, mui_scene_t* p_scene_defines, uint32_t scene_num);
void mui_scene_dispatcher_set_user_data(mui_scene_dispatcher_t* p_dispatcher, void* user_data);
void mui_scene_dispatcher_free(mui_scene_dispatcher_t* p_dispatcher);
void mui_scene_dispatcher_switch_scene(mui_scene_dispatcher_t* p_dispatcher, uint32_t scene_id);





#endif