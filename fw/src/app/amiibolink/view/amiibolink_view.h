#ifndef AMIIBOLINK_VIEW_H
#define AMIIBOLINK_VIEW_H

#include "mui_include.h"
#include "vfs.h"
#include "app_timer.h"

struct amiibolink_view_s;
typedef struct amiibolink_view_s amiibolink_view_t;

typedef enum {
    AMIIBOLINK_VIEW_EVENT_KEY_PRESSED,
    AMIIBOLINK_VIEW_EVENT_PLAY_STOPPED
} amiibolink_view_event_t;

typedef void (*amiibolink_view_event_cb)(amiibolink_view_event_t event, amiibolink_view_t *p_view);

 struct amiibolink_view_s{
    mui_view_t* p_view;
    amiibolink_view_event_cb event_cb;
    void* user_data;
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



#endif