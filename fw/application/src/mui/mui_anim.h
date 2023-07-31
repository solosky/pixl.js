#ifndef MUI_ANIM_H
#define MUI_ANIM_H

#include <stdint.h>


struct _mui_anim_t;

typedef void (*mui_anim_exec_cb_t)(void* var, int32_t value);
typedef int32_t (*mui_anim_path_cb_t)(const struct _mui_anim_t* p_anim);

typedef struct _mui_anim_t {
    void* var;
    int32_t start_value;
    int32_t end_value;
    int32_t current_value;
    mui_anim_exec_cb_t exec_cb;
    mui_anim_path_cb_t path_cb;
    uint32_t time; //in ms
    uint32_t act_time; //act time in ms
    uint16_t repeat_cnt; //
    uint16_t run_cnt; //
    void* user_data;
} mui_anim_t;



void mui_anim_core_init();

void mui_anim_init(mui_anim_t *p_anim);
void mui_anim_start(mui_anim_t* p_anim);
void mui_anim_stop(mui_anim_t* p_anim);

static inline void mui_anim_set_var(mui_anim_t* p_anim, void* var){
    p_anim->var = var;
}
static inline void mui_anim_set_exec_cb(mui_anim_t* p_anim, mui_anim_exec_cb_t exec_cb){
    p_anim->exec_cb = exec_cb;
}

static inline void mui_anim_set_path_cb(mui_anim_t* p_anim, mui_anim_path_cb_t path_cb){
    p_anim->path_cb = path_cb;
}

static inline void mui_anim_set_time(mui_anim_t* p_anim, uint32_t time){
    p_anim->time = time;
}

static inline void mui_anim_set_values(mui_anim_t* p_anim, int16_t start, int16_t end){
    p_anim->start_value = start;
    p_anim->end_value = end;
    p_anim->current_value = start;
}

static inline void mui_anim_set_user_data(mui_anim_t* p_anim, void* user_data){
    p_anim->user_data = user_data;
}
#endif 