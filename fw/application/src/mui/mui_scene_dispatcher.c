#include "mui_scene_dispatcher.h"
#include "mui_mem.h"

mui_scene_dispatcher_t *mui_scene_dispatcher_create() {
    mui_scene_dispatcher_t *p_dispatcher = mui_mem_malloc(sizeof(mui_scene_dispatcher_t));
    scene_id_stack_init(p_dispatcher->scene_id_stack);
    p_dispatcher->p_scene_defines = NULL;
    p_dispatcher->scene_num = 0;
    p_dispatcher->user_data = NULL;
    p_dispatcher->default_scene_id = 0;
    return p_dispatcher;
}

void mui_scene_dispatcher_free(mui_scene_dispatcher_t *p_dispatcher) {
    //call last sence exit to free resources
    // if (scene_id_stack_size(p_dispatcher->scene_id_stack) > 0) {
    //     uint32_t cur_scene_id = *scene_id_stack_back(p_dispatcher->scene_id_stack);
    //     p_dispatcher->p_scene_defines[cur_scene_id].exit_cb(p_dispatcher->user_data);
    // }
    scene_id_stack_clear(p_dispatcher->scene_id_stack);
    mui_mem_free(p_dispatcher);
}

void mui_scene_dispatcher_exit(mui_scene_dispatcher_t* p_dispatcher){
    //call last sence exit to free resources
    if (scene_id_stack_size(p_dispatcher->scene_id_stack) > 0) {
        uint32_t cur_scene_id = *scene_id_stack_back(p_dispatcher->scene_id_stack);
        p_dispatcher->p_scene_defines[cur_scene_id].exit_cb(p_dispatcher->user_data);
    }
}

void mui_scene_dispatcher_set_scene_defines(mui_scene_dispatcher_t *p_dispatcher, const mui_scene_t *p_scene_defines,
                                            uint32_t scene_num) {
    p_dispatcher->p_scene_defines = p_scene_defines;
    p_dispatcher->scene_num = scene_num;
}

void mui_scene_dispatcher_set_user_data(mui_scene_dispatcher_t *p_dispatcher, void *user_data) {
    p_dispatcher->user_data = user_data;
}

void mui_scene_dispatcher_next_scene(mui_scene_dispatcher_t *p_dispatcher, uint32_t scene_id) {
    if (scene_id_stack_size(p_dispatcher->scene_id_stack) > 0) {
        uint32_t cur_scene_id = *scene_id_stack_back(p_dispatcher->scene_id_stack);
        p_dispatcher->p_scene_defines[cur_scene_id].exit_cb(p_dispatcher->user_data);
    }
    scene_id_stack_push_back(p_dispatcher->scene_id_stack, scene_id);
    p_dispatcher->p_scene_defines[scene_id].enter_cb(p_dispatcher->user_data);
}

void mui_scene_dispatcher_previous_scene(mui_scene_dispatcher_t *p_dispatcher) {
    if (scene_id_stack_size(p_dispatcher->scene_id_stack) > 0) {
        uint32_t cur_scene_id;
        scene_id_stack_pop_back(&cur_scene_id, p_dispatcher->scene_id_stack);

        // last scene
        if (scene_id_stack_size(p_dispatcher->scene_id_stack) == 0) {
            p_dispatcher->p_scene_defines[cur_scene_id].exit_cb(p_dispatcher->user_data);
            p_dispatcher->p_scene_defines[p_dispatcher->default_scene_id].enter_cb(p_dispatcher->user_data);
        } else {
            uint32_t prev_scene_id = *scene_id_stack_back(p_dispatcher->scene_id_stack);
            p_dispatcher->p_scene_defines[cur_scene_id].exit_cb(p_dispatcher->user_data);
            p_dispatcher->p_scene_defines[prev_scene_id].enter_cb(p_dispatcher->user_data);
        }
    }
}

uint32_t mui_scene_dispatcher_current_scene(mui_scene_dispatcher_t *p_dispatcher){
    uint32_t* p_scene_id = scene_id_stack_back(p_dispatcher->scene_id_stack);
    if(p_scene_id){
        return *p_scene_id;
    }
    return 0;
}