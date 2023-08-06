#include "mui_anim.h"
#include "app_timer.h"
#include "m-array.h"
#include "mui_mlib.h"
#include "mui_core.h"
#include "mui_math.h"
#include <stdbool.h>
#include "nrf_log.h"

#define LV_ANIM_RESOLUTION 1024
#define LV_ANIM_RES_SHIFT 10

#define MUI_ANIM_TICK_INTERVAL_MS 50
#define MUI_ANIM_REPEAT_INFINITE 0xFFFF


ARRAY_DEF(mui_anim_ptr_array, mui_anim_t *, M_PTR_OPLIST);

APP_TIMER_DEF(m_anim_tick_tmr);
bool m_anim_tmr_started = false;
mui_anim_ptr_array_t m_anim_ptr_array;


static int32_t lv_anim_path_cubic_bezier(const mui_anim_t * a, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /*Calculate the current step*/
    uint32_t t = lv_map(a->act_time, 0, a->time, 0, LV_BEZIER_VAL_MAX);
    int32_t step = lv_cubic_bezier(t, x1, y1, x2, y2);

    int32_t new_value;
    new_value = step * (a->end_value - a->start_value);
    new_value = new_value >> LV_BEZIER_VAL_SHIFT;
    new_value += a->start_value;

    return new_value;
}


int32_t lv_anim_path_linear(const mui_anim_t * a)
{
    /*Calculate the current step*/
    int32_t step = lv_map(a->act_time, 0, a->time, 0, LV_ANIM_RESOLUTION);

    /*Get the new value which will be proportional to `step`
     *and the `start` and `end` values*/
    int32_t new_value;
    new_value = step * (a->end_value - a->start_value);
    new_value = new_value >> LV_ANIM_RES_SHIFT;
    new_value += a->start_value;

    return new_value;
}


int32_t lv_anim_path_ease_in(const mui_anim_t * a)
{
    return lv_anim_path_cubic_bezier(a, LV_BEZIER_VAL_FLOAT(0.42), LV_BEZIER_VAL_FLOAT(0),
                                     LV_BEZIER_VAL_FLOAT(1), LV_BEZIER_VAL_FLOAT(1));
}

int32_t lv_anim_path_ease_out(const mui_anim_t * a)
{
    return lv_anim_path_cubic_bezier(a, LV_BEZIER_VAL_FLOAT(0), LV_BEZIER_VAL_FLOAT(0),
                                     LV_BEZIER_VAL_FLOAT(0.58), LV_BEZIER_VAL_FLOAT(1));
}

int32_t lv_anim_path_ease_in_out(const mui_anim_t * a)
{
    return lv_anim_path_cubic_bezier(a, LV_BEZIER_VAL_FLOAT(0.42), LV_BEZIER_VAL_FLOAT(0),
                                     LV_BEZIER_VAL_FLOAT(0.58), LV_BEZIER_VAL_FLOAT(1));
}

int32_t lv_anim_path_overshoot(const mui_anim_t * a)
{
    return lv_anim_path_cubic_bezier(a, 341, 0, 683, 1300);
}

int32_t lv_anim_path_bounce(const mui_anim_t * a)
{
    /*Calculate the current step*/
    int32_t t = lv_map(a->act_time, 0, a->time, 0, LV_BEZIER_VAL_MAX);
    int32_t diff = (a->end_value - a->start_value);

    /*3 bounces has 5 parts: 3 down and 2 up. One part is t / 5 long*/

    if(t < 408) {
        /*Go down*/
        t = (t * 2500) >> LV_BEZIER_VAL_SHIFT; /*[0..1024] range*/
    }
    else if(t >= 408 && t < 614) {
        /*First bounce back*/
        t -= 408;
        t    = t * 5; /*to [0..1024] range*/
        t    = LV_BEZIER_VAL_MAX - t;
        diff = diff / 20;
    }
    else if(t >= 614 && t < 819) {
        /*Fall back*/
        t -= 614;
        t    = t * 5; /*to [0..1024] range*/
        diff = diff / 20;
    }
    else if(t >= 819 && t < 921) {
        /*Second bounce back*/
        t -= 819;
        t    = t * 10; /*to [0..1024] range*/
        t    = LV_BEZIER_VAL_MAX - t;
        diff = diff / 40;
    }
    else if(t >= 921 && t <= LV_BEZIER_VAL_MAX) {
        /*Fall back*/
        t -= 921;
        t    = t * 10; /*to [0..1024] range*/
        diff = diff / 40;
    }

    if(t > LV_BEZIER_VAL_MAX) t = LV_BEZIER_VAL_MAX;
    if(t < 0) t = 0;
    int32_t step = lv_bezier3(t, LV_BEZIER_VAL_MAX, 800, 500, 0);

    int32_t new_value;
    new_value = step * diff;
    new_value = new_value >> LV_BEZIER_VAL_SHIFT;
    new_value = a->end_value - new_value;

    return new_value;
}

//put anim tick into event queue process to avoid race conditions
static void mui_anim_tick_tmr_cb(void* p_context) {
    mui_event_t mui_event = {.id = MUI_EVENT_ID_ANIM};
    mui_post(mui(), &mui_event);
}


static void mui_anim_tick_handler() {
    mui_anim_ptr_array_it_t it;
    int32_t err_code;
    bool mui_update_required = false;
    mui_anim_ptr_array_it(it, m_anim_ptr_array);

    while (!mui_anim_ptr_array_end_p(it)) {
        mui_anim_t *p_anim = *mui_anim_ptr_array_ref(it);

        p_anim->act_time += MUI_ANIM_TICK_INTERVAL_MS;
        if (p_anim->act_time > p_anim->time) {
            if (p_anim->repeat_cnt == MUI_ANIM_REPEAT_INFINITE || (++p_anim->run_cnt) > p_anim->repeat_cnt) {
                // reset
                p_anim->act_time = 0;
                p_anim->current_value = p_anim->start_value;
                p_anim->run_cnt = 0;
            } else {
                // reached end of animation
                mui_anim_ptr_array_remove(m_anim_ptr_array, it);
            }
        } else {
            int32_t new_value = p_anim->path_cb(p_anim);
            if (new_value != p_anim->current_value) {
                p_anim->current_value = new_value;
                p_anim->exec_cb(p_anim->var, new_value);
                mui_update_required = true;
            }
        }
        mui_anim_ptr_array_next(it);
    }

    // stop timer
    if (mui_anim_ptr_array_size(m_anim_ptr_array) <= 0 && m_anim_tmr_started) {
        m_anim_tmr_started = false;
        err_code = app_timer_stop(m_anim_tick_tmr);
        APP_ERROR_CHECK(err_code);
    }

    //redraw event
    if(mui_update_required){
        mui_update(mui());
    }
}

static mui_anim_t* mui_anim_remove_ptr(mui_anim_t* p_anim){
    mui_anim_ptr_array_it_t it;
    mui_anim_ptr_array_it(it, m_anim_ptr_array);

    while (!mui_anim_ptr_array_end_p(it)) {
        mui_anim_t *p_cur_anim = *mui_anim_ptr_array_ref(it);

        if(p_anim == p_cur_anim){
            mui_anim_ptr_array_remove(m_anim_ptr_array, it)
                ;
            return p_cur_anim;
        }
        mui_anim_ptr_array_next(it);
    }
}

void mui_anim_core_init() {
    int32_t err_code = app_timer_create(&m_anim_tick_tmr, APP_TIMER_MODE_REPEATED, mui_anim_tick_tmr_cb);
    APP_ERROR_CHECK(err_code);
}

void mui_anim_core_event(mui_event_t* p_event){
    mui_anim_tick_handler();
}

void mui_anim_init(mui_anim_t *p_anim){
    p_anim->start_value = 0;
    p_anim->end_value = 100;
    p_anim->current_value = 0;
    p_anim->time = 500;
    p_anim->var = NULL;
    p_anim->exec_cb = NULL;
    p_anim->repeat_cnt = 1;
    p_anim->run_cnt = 0;
    p_anim->path_cb = lv_anim_path_ease_in_out;
}

void mui_anim_start(mui_anim_t *p_anim) {
    // todo check var exists
    assert(p_anim);
    assert(p_anim->var != NULL);
    int32_t err_code;
    mui_anim_remove_ptr(p_anim);
    mui_anim_ptr_array_push_back(m_anim_ptr_array, p_anim);
    p_anim->run_cnt = 0;
    p_anim->act_time = 0;
    NRF_LOG_INFO("anim start");
    if (!m_anim_tmr_started) {
        err_code = app_timer_start(m_anim_tick_tmr, APP_TIMER_TICKS(MUI_ANIM_TICK_INTERVAL_MS), NULL);
        APP_ERROR_CHECK(err_code);
        m_anim_tmr_started = true;
    }
}



void mui_anim_stop(mui_anim_t *p_anim) {
    mui_anim_remove_ptr(p_anim);
    if (mui_anim_ptr_array_size(m_anim_ptr_array) <= 0 && m_anim_tmr_started) {
        m_anim_tmr_started = false;
        int32_t err_code = app_timer_stop(m_anim_tick_tmr);
        APP_ERROR_CHECK(err_code);
    }
}