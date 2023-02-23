#ifndef PLAYER_VIEW_H
#define PLAYER_VIEW_H

#include "mui_include.h"
#include "vfs.h"
#include "app_timer.h"

struct player_view_s;
typedef struct player_view_s player_view_t;

typedef enum {
    PLAYER_VIEW_EVENT_KEY_PRESSED,
    PLAYER_VIEW_EVENT_PLAY_STOPPED
} player_view_event_t;

typedef void (*player_view_event_cb)(player_view_event_t event, player_view_t *p_view);

//   # magic + frame_count + fps + width + height + reserved
//     # 4b + 2b + 2b + 2b + 2b + 4b = 16 bytes
typedef struct {
    int32_t magic;
    uint16_t frame_count;
    uint16_t fps;
    uint16_t width;
    uint16_t height;
    uint32_t reserved;
} v_header_t;

#define V_HEADER_MAGIC 0x50495856

 struct player_view_s{
    mui_view_t* p_view;
    player_view_event_cb event_cb;
    vfs_file_t fd;
    vfs_driver_t* p_dirver;
    v_header_t header;
    uint16_t played_frames;
    bool is_playing;
    uint8_t* frame_buffer;
    size_t frame_buffer_size;
    app_timer_t frame_tick_timer_data;
    app_timer_id_t frame_tick_timer_id;

    void* user_data;


} ;

player_view_t* player_view_create();
void player_view_free(player_view_t* p_view);
mui_view_t* player_view_get_view(player_view_t* p_view);

void player_view_play(player_view_t* p_view, vfs_drive_t drive, char* path);
void player_view_stop(player_view_t *p_view);

static inline void player_view_set_event_cb(player_view_t* p_view, player_view_event_cb event_cb){
    p_view->event_cb = event_cb;
}

static inline void player_view_set_user_data(player_view_t* p_view, void* user_data){
    p_view->user_data = user_data;
}



#endif