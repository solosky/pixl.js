#ifndef game_VIEW_H
#define game_VIEW_H

#include "mui_include.h"
#include "vfs.h"
#include "app_timer.h"

struct game_view_s;
typedef struct game_view_s game_view_t;

typedef enum {
    game_VIEW_EVENT_KEY_PRESSED,
    game_VIEW_EVENT_PLAY_STOPPED
} game_view_event_t;

typedef void (*game_view_event_cb)(game_view_event_t event, game_view_t *p_view);


#define V_HEADER_MAGIC 0x50495856

 struct game_view_s{
    mui_view_t* p_view;
    game_view_event_cb event_cb;
    vfs_file_t fd;
    vfs_driver_t* p_dirver;
    uint16_t played_frames;
    bool is_playing;
    uint8_t* frame_buffer;
    size_t frame_buffer_size;
    app_timer_t frame_tick_timer_data;
    app_timer_id_t frame_tick_timer_id;

    void* user_data;


} ;

game_view_t* game_view_create();
void game_view_free(game_view_t* p_view);
mui_view_t* game_view_get_view(game_view_t* p_view);

void game_view_play(game_view_t* p_view, vfs_drive_t drive, char* path);
void game_view_stop(game_view_t *p_view);

static inline void game_view_set_event_cb(game_view_t* p_view, game_view_event_cb event_cb){
    p_view->event_cb = event_cb;
}

static inline void game_view_set_user_data(game_view_t* p_view, void* user_data){
    p_view->user_data = user_data;
}



#endif