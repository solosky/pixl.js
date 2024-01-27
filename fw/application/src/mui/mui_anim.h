#ifndef MUI_ANIM_H
#define MUI_ANIM_H

#include "mui_common.h"
#include "mui_event.h"
#include "mui_math.h"

struct _mui_anim_t;

typedef void (*mui_anim_exec_cb_t)(void* var, int32_t value);
typedef int32_t (*mui_anim_path_cb_t)(const struct _mui_anim_t* p_anim);

typedef struct _lv_anim_bezier3_para_t {
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} lv_anim_bezier3_para_t; /**< Parameter used when path is custom_bezier*/

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
    bool auto_restart;
    void* user_data;
    union _lv_anim_path_para_t {
        lv_anim_bezier3_para_t bezier3; /**< Parameter used when path is custom_bezier*/
    } parameter;
} mui_anim_t;



void mui_anim_core_init();
void mui_anim_core_event(mui_event_t* p_event);

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

static inline void mui_anim_set_auto_restart(mui_anim_t* p_anim, bool auto_restart){
    p_anim->auto_restart = auto_restart;
}

static inline void mui_anim_set_values(mui_anim_t* p_anim, int32_t start, int32_t end){
    p_anim->start_value = start;
    p_anim->end_value = end;
    p_anim->current_value = start;
}

static inline void mui_anim_set_user_data(mui_anim_t* p_anim, void* user_data){
    p_anim->user_data = user_data;
}


/**
 * Calculate the current value of an animation applying linear characteristic
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_linear(const mui_anim_t * a);

/**
 * Calculate the current value of an animation slowing down the start phase
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_ease_in(const mui_anim_t * a);

/**
 * Calculate the current value of an animation slowing down the end phase
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_ease_out(const mui_anim_t * a);

/**
 * Calculate the current value of an animation applying an "S" characteristic (cosine)
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_ease_in_out(const mui_anim_t * a);

/**
 * Calculate the current value of an animation with overshoot at the end
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_overshoot(const mui_anim_t * a);

/**
 * Calculate the current value of an animation with 3 bounces
 * @param a     pointer to an animation
 * @return      the current value to set
 */
int32_t lv_anim_path_bounce(const mui_anim_t * a);
#endif 