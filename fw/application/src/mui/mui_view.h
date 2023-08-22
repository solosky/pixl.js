#ifndef MUI_VIEW_H
#define MUI_VIEW_H

#include "mui_defines.h"
#include "mui_canvas.h"
#include "mui_input.h"

/** Hides drawing view_port */
#define VIEW_NONE 0xFFFFFFFF

struct mui_view_s;
typedef struct mui_view_s mui_view_t;


typedef void (*mui_view_draw_cb_t)(mui_view_t* p_view, mui_canvas_t* p_canvas);
typedef void (*mui_view_input_cb_t)(mui_view_t* p_view, mui_input_event_t* event);
typedef void (*mui_view_action_cb_t)(mui_view_t* p_view);

struct mui_view_s {
    void* user_data;
    mui_view_draw_cb_t draw_cb;
    mui_view_input_cb_t input_cb;
    mui_view_action_cb_t enter_cb;
    mui_view_action_cb_t exit_cb;
};

mui_view_t* mui_view_create();
void mui_view_draw(mui_view_t* p_view, mui_canvas_t* p_canvas);
void mui_view_input(mui_view_t* p_view, mui_input_event_t* p_event);
void mui_view_enter(mui_view_t* p_view);
void mui_view_exit(mui_view_t* p_view);
void mui_view_free(mui_view_t* p_view);

#endif

