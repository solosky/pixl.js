#include "game_view.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"

#include "mui_core.h"

uint8_t key_state[3] = {0};
uint8_t key_repeat_cnt = 0;

uint8_t game_view_key_pressed(input_key_t key) { return key_state[key]; }

uint8_t game_view_center_key_repeat_cnt(){
    return key_repeat_cnt;
}

static void game_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    game_view_t *p_game_view = p_view->user_data;
    if (p_game_view->running == 0) {
        p_game_view->running = 1;
        mui_set_auto_update(mui(), 0);
        game_view_t *p_game_view = p_view->user_data;
        p_game_view->game_run();
        p_game_view->running = 0;
        key_repeat_cnt = 0;
        mui_set_auto_update(mui(), 1);
        
         if (p_game_view->event_cb) {
            p_game_view->event_cb(GAME_VIEW_EVENT_PLAY_STOPPED, p_game_view);
        }
    }
}

static void game_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    game_view_t *p_game_view = p_view->user_data;

    if (event->type == INPUT_TYPE_PRESS) {
        key_state[event->key] = 1;
    } else if (event->type == INPUT_TYPE_RELEASE) {
        key_state[event->key] = 0;
        key_repeat_cnt = 0;
    } else if (event->type == INPUT_TYPE_REPEAT) {
        if(event->key == INPUT_KEY_CENTER){
            key_repeat_cnt++;
        }
    }
}

static void game_view_on_enter(mui_view_t *p_view) {}

static void game_view_on_exit(mui_view_t *p_view) {}

game_view_t *game_view_create() {
    game_view_t *p_game_view = mui_mem_malloc(sizeof(game_view_t));
    memset(p_game_view, 0, sizeof(game_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_game_view;
    p_view->draw_cb = game_view_on_draw;
    p_view->input_cb = game_view_on_input;
    p_view->enter_cb = game_view_on_enter;
    p_view->exit_cb = game_view_on_exit;

    p_game_view->p_view = p_view;

    return p_game_view;
}
void game_view_free(game_view_t *p_view) {
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}
mui_view_t *game_view_get_view(game_view_t *p_view) { return p_view->p_view; }
