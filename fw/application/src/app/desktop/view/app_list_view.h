#ifndef APP_LIST_VIEW_H
#define APP_LIST_VIEW_H

#include "mui_include.h"
#include "mini_app_defines.h"


struct app_list_view_s;
typedef struct app_list_view_s app_list_view_t;

typedef enum {
    APP_LIST_ITEM_SELECTED,
} app_list_view_event_t;

typedef void (*app_list_view_event_cb)(app_list_view_event_t event, app_list_view_t *p_view);

 struct app_list_view_s{
    mui_view_t* p_view;
    app_list_view_event_cb event_cb;
    uint8_t focus;
    ptr_array_t items;
    void* user_data;
    mui_anim_t icon_anim;
    int32_t icon_anim_value;
    mui_anim_t name_anim;
    int8_t name_anim_value;

    uint8_t canvas_height;
    uint8_t canvas_width;
} ;

app_list_view_t* app_list_view_create();
void app_list_view_free(app_list_view_t* p_view);
mui_view_t* app_list_view_get_view(app_list_view_t* p_view);

static inline void app_list_view_set_event_cb(app_list_view_t* p_view, app_list_view_event_cb event_cb){
    p_view->event_cb = event_cb;
}

static inline void app_list_view_add_app(app_list_view_t* p_view, mini_app_t* p_app){
    ptr_array_push_back(p_view->items, p_app);
}

static inline mini_app_t* app_list_view_get(app_list_view_t* p_view, uint8_t index){
    return (mini_app_t*) *ptr_array_get(p_view->items, index);
}
static inline uint8_t app_list_view_get_focus(app_list_view_t* p_view){
    return p_view->focus;
}

static inline void app_list_view_set_user_data(app_list_view_t* p_view, void* user_data){
    p_view->user_data = user_data;
}


#endif