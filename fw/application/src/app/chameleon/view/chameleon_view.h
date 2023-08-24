#ifndef CHAMELEON_VIEW_H
#define CHAMELEON_VIEW_H

#include "mui_include.h"
#include "vfs.h"
#include "app_timer.h"

struct chameleon_view_s;
typedef struct chameleon_view_s chameleon_view_t;

typedef enum {
    CHAMELEON_VIEW_EVENT_MENU,
    CHAMELEON_VIEW_EVENT_UPDATE
} chameleon_view_event_t;

typedef void (*chameleon_view_event_cb)(chameleon_view_event_t event, chameleon_view_t *p_view);

 struct chameleon_view_s{
    mui_view_t* p_view;
    chameleon_view_event_cb event_cb;
    void* user_data;
    uint8_t index;
    uint8_t max_size;
} ;

chameleon_view_t* chameleon_view_create();
void chameleon_view_free(chameleon_view_t* p_view);
mui_view_t* chameleon_view_get_view(chameleon_view_t* p_view);

static inline void chameleon_view_set_event_cb(chameleon_view_t* p_view, chameleon_view_event_cb event_cb){
    p_view->event_cb = event_cb;
}

static inline void chameleon_view_set_user_data(chameleon_view_t* p_view, void* user_data){
    p_view->user_data = user_data;
}

static inline void chameleon_view_set_index(chameleon_view_t* p_view, uint32_t index){
    p_view->index = index;
}

static inline uint8_t chameleon_view_get_index(chameleon_view_t* p_view){
    return p_view->index;
}

static inline void chameleon_view_set_max_size(chameleon_view_t* p_view, uint8_t max_size){
    p_view->max_size = max_size;
}



#endif