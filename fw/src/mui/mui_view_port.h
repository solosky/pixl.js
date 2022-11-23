#ifndef MUI_VIEW_PORT_H
#define MUI_VIEW_PORT_H

#include <stdint.h>
#include "mui_canvas.h"
#include "mui_input.h"
#include "mui_event.h"


struct mui_view_port_s;
typedef struct mui_view_port_s mui_view_port_t;


typedef void (* mui_view_port_draw_cb_t)(mui_view_port_t* p_vp, mui_canvas_t* p_canvas);
typedef void (* mui_view_port_input_cb_t)(mui_view_port_t* p_vp, mui_input_event_t* p_event);

struct mui_view_port_s{
    uint8_t width;
    uint8_t height;
    mui_canvas_t canvas;
    bool enabled;
    void* user_data;
    
    mui_view_port_draw_cb_t draw_cb;
    mui_view_port_input_cb_t input_cb;
};



mui_view_port_t* mui_view_port_create();
void mui_view_port_input(mui_view_port_t * p_vp, mui_input_event_t* p_event);
void mui_view_port_draw(mui_view_port_t * p_vp);
void mui_view_port_free(mui_view_port_t* p_vp);





#endif