#ifndef AMIIBOLINK_VIEW_H
#define AMIIBOLINK_VIEW_H

#include "mui_include.h"
#include "vfs.h"
#include "app_timer.h"
#include "ble_amiibolink.h"

struct amiibolink_view_s;
typedef struct amiibolink_view_s amiibolink_view_t;

typedef enum {
    AMIIBOLINK_VIEW_EVENT_MENU,
    AMIIBOLINK_VIEW_EVENT_UPDATE
} amiibolink_view_event_t;

typedef void (*amiibolink_view_event_cb)(amiibolink_view_event_t event, amiibolink_view_t *p_view);

 struct amiibolink_view_s{
    mui_view_t* p_view;
    amiibolink_view_event_cb event_cb;
    void* user_data;
    ble_amiibolink_mode_t amiibolink_mode;
    uint8_t index;
    uint8_t max_size;
} ;

amiibolink_view_t* amiibolink_view_create();
void amiibolink_view_free(amiibolink_view_t* p_view);
mui_view_t* amiibolink_view_get_view(amiibolink_view_t* p_view);

static inline void amiibolink_view_set_event_cb(amiibolink_view_t* p_view, amiibolink_view_event_cb event_cb){
    p_view->event_cb = event_cb;
}

static inline void amiibolink_view_set_user_data(amiibolink_view_t* p_view, void* user_data){
    p_view->user_data = user_data;
}

static inline void amiibolink_view_set_amiibolink_mode(amiibolink_view_t* p_view, ble_amiibolink_mode_t amiibolink_mode){
    p_view->amiibolink_mode = amiibolink_mode;
}

static inline void amiibolink_view_set_index(amiibolink_view_t* p_view, uint32_t index){
    p_view->index = index;
}

static inline uint8_t amiibolink_view_get_index(amiibolink_view_t* p_view){
    return p_view->index;
}

static inline void amiibolink_view_set_max_size(amiibolink_view_t* p_view, uint8_t max_size){
    p_view->max_size = max_size;
}



#endif