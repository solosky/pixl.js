#include "mui_anim.h"
#include "app_timer.h"
#include "m-array.h"
#include "mui_mlib.h"
#include "mui_core.h"
#include <stdbool.h>

#define LV_ANIM_RESOLUTION 1024
#define LV_ANIM_RES_SHIFT 10
#define MUI_ANIM_TICK_INTERVAL_MS 50
#define MUI_ANIM_REPEAT_INFINITE 0xFFFF

ARRAY_DEF(mui_anim_ptr_array, mui_anim_t *, M_PTR_OPLIST);

APP_TIMER_DEF(m_anim_tick_tmr);
bool m_anim_tmr_started = false;
mui_anim_ptr_array_t m_anim_ptr_array;


/**
 * Get the mapped of a number given an input and output range
 * @param x integer which mapped value should be calculated
 * @param min_in min input range
 * @param max_in max input range
 * @param min_out max output range
 * @param max_out max output range
 * @return the mapped number
 */
int32_t lv_map(int32_t x, int32_t min_in, int32_t max_in, int32_t min_out, int32_t max_out)
{
    if(x >= max_in) return max_out;
    if(x <= min_in) return min_out;

    /**
     * The equation should be:
     *   ((x - min_in) * delta_out) / delta in) + min_out
     * To avoid rounding error reorder the operations:
     *   (x - min_in) * (delta_out / delta_min) + min_out
     */

    int32_t delta_in = max_in - min_in;
    int32_t delta_out = max_out - min_out;

    return ((x - min_in) * delta_out) / delta_in + min_out;
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

// TODO critial section????
static void mui_anim_tick_handler(void *p_context) {
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

void mui_anim_core_init() {
    int32_t err_code = app_timer_create(&m_anim_tick_tmr, APP_TIMER_MODE_REPEATED, mui_anim_tick_handler);
    APP_ERROR_CHECK(err_code);
}

void mui_anim_init(mui_anim_t *p_anim){
    p_anim->start_value = 0;
    p_anim->end_value = 100;
    p_anim->current_value = 0;
    p_anim->time = 500;
    p_anim->var = NULL;
    p_anim->exec_cb = NULL;
    p_anim->repeat_cnt = 1;
    p_anim->path_cb = lv_anim_path_linear;
}

void mui_anim_start(mui_anim_t *p_anim) {
    // todo check var exists
    assert(p_anim);
    assert(p_anim->var != NULL);
    int32_t err_code;

    mui_anim_ptr_array_push_back(m_anim_ptr_array, p_anim);
    if (!m_anim_tmr_started) {
        err_code = app_timer_start(m_anim_tick_tmr, APP_TIMER_TICKS(MUI_ANIM_TICK_INTERVAL_MS), NULL);
        APP_ERROR_CHECK(err_code);
        m_anim_tmr_started = true;
    }
}
void mui_anim_stop(mui_anim_t *p_anim) {
    mui_anim_ptr_array_remove(m_anim_ptr_array, p_anim);
    if (mui_anim_ptr_array_size(m_anim_ptr_array) <= 0 && m_anim_tmr_started) {
        m_anim_tmr_started = false;
        int32_t err_code = app_timer_stop(m_anim_tick_tmr);
        APP_ERROR_CHECK(err_code);
    }
}