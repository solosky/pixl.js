#ifndef MUI_PROGRESS_BAR_H
#define MUI_PROGRESS_BAR_H

#include "mui_include.h"

struct mui_progress_bar_s;
typedef struct mui_progress_bar_s mui_progress_bar_t;

typedef enum {
    MUI_PROGRESS_BAR_EVENT_INCREMENT,
    MUI_PROGRESS_BAR_EVENT_DECREMENT,
    MUI_PROGRESS_BAR_EVENT_CONFIRMED,
} mui_progress_bar_event_t;

typedef void (*mui_progress_bar_event_cb_t)(mui_progress_bar_event_t event, mui_progress_bar_t *p_progress_bar);

struct mui_progress_bar_s {
    mui_view_t *p_view;
    string_t header;
    int32_t max_value;
    int32_t min_value;
    int32_t current_value;
    mui_progress_bar_event_cb_t event_cb;
    void *user_data;
};

mui_progress_bar_t *mui_progress_bar_create();
void mui_progress_bar_free(mui_progress_bar_t *p_view);
void mui_progress_bar_reset(mui_progress_bar_t *p_view);

static inline mui_view_t *mui_progress_bar_get_view(mui_progress_bar_t *p_view){
    return p_view->p_view;
}


static inline void mui_progress_bar_set_user_data(mui_progress_bar_t *p_view, void *user_data){
    p_view->user_data = user_data;
}

static inline void* mui_progress_bar_get_user_data(mui_progress_bar_t *p_view){
    return p_view->user_data;
}

//// view functions //
static inline void mui_progress_bar_set_event_cb(mui_progress_bar_t *p_view, mui_progress_bar_event_cb_t event_cb){
    p_view->event_cb = event_cb;
}
static inline void mui_progress_bar_set_header(mui_progress_bar_t *p_view, const char* header){
    string_set_str(p_view->header, header);
}

static inline void mui_progress_bar_set_max_value(mui_progress_bar_t *p_view, int32_t max_value){
    p_view->max_value = max_value;
}

static inline void mui_progress_bar_set_min_value(mui_progress_bar_t *p_view, int32_t min_value){
    p_view->min_value = min_value;
}

static inline void mui_progress_bar_set_current_value(mui_progress_bar_t *p_view, int32_t current_value) {
    p_view->current_value = current_value;
}

static inline int32_t mui_progress_bar_get_current_value(mui_progress_bar_t *p_view) {
    return p_view->current_value;
}

#endif