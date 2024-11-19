#ifndef game_VIEW_H
#define game_VIEW_H

#include "mui_include.h"
#include "vfs.h"
#include "app_timer.h"
#include "tiny_lander.h"

struct game_view_s;
typedef struct game_view_s game_view_t;

typedef enum {
    game_VIEW_EVENT_KEY_PRESSED,
    game_VIEW_EVENT_PLAY_STOPPED
} game_view_event_t;

typedef void (*game_view_event_cb)(game_view_event_t event, game_view_t *p_view);

typedef int(*game_run_t)(void);


#define V_HEADER_MAGIC 0x50495856

 struct game_view_s{
    mui_view_t* p_view;
    game_view_event_cb event_cb;
    game_run_t game_run;
    void* user_data;
} ;

game_view_t* game_view_create();
void game_view_free(game_view_t* p_view);
mui_view_t* game_view_get_view(game_view_t* p_view);

uint8_t game_view_key_pressed(input_key_t key);

static inline void game_view_set_event_cb(game_view_t* p_view, game_view_event_cb event_cb){
    p_view->event_cb = event_cb;
}

static inline void game_view_set_user_data(game_view_t* p_view, void* user_data){
    p_view->user_data = user_data;
}


static inline void game_view_set_game_run(game_view_t *p_view, game_run_t game_run) { p_view->game_run = game_run; }
#endif