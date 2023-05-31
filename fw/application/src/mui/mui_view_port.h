#ifndef MUI_VIEW_PORT_H
#define MUI_VIEW_PORT_H

#include "mui_defines.h"
#include "mui_canvas.h"
#include "mui_event.h"
#include "mui_input.h"
#include <stdint.h>

struct mui_view_port_s;
typedef struct mui_view_port_s mui_view_port_t;

typedef void (*mui_view_port_draw_cb_t)(mui_view_port_t *p_vp, mui_canvas_t *p_canvas);
typedef void (*mui_view_port_input_cb_t)(mui_view_port_t *p_vp,
                                         mui_input_event_t *p_event);

struct mui_view_port_s {
    bool enabled;
    void *user_data;

    mui_view_port_draw_cb_t draw_cb;
    mui_view_port_input_cb_t input_cb;
};

mui_view_port_t *mui_view_port_create();
void mui_view_port_free(mui_view_port_t* p_vp);

void mui_view_port_set_user_data(mui_view_port_t *p_vp, void *user_data);
void mui_view_port_set_draw_cb(mui_view_port_t *p_vp, mui_view_port_draw_cb_t draw_cb);
void mui_view_port_set_input_cb(mui_view_port_t *p_vp, mui_view_port_input_cb_t input_cb);

void mui_view_port_input(mui_view_port_t *p_vp, mui_input_event_t *p_event);
void mui_view_port_draw(mui_view_port_t *p_vp, mui_canvas_t *p_canvas);
void mui_view_port_enable_set(mui_view_port_t *p_vp, bool enabled);
void mui_view_port_update(mui_view_port_t *p_vp);

#endif