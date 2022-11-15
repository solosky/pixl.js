#ifndef MUI_VIEW_PORT_H
#define MUI_VIEW_PORT_H

#include <stdint.h>
#include "mui_canvas.h"
#include "mui_input.h"


struct mui_view_port_s;
typedef struct mui_view_port_s mui_view_port_t;


typedef void (* mui_view_port_draw_cb_t)(mui_view_port_t* p_vp, mui_canvas_t* p_canvas);
typedef void (* mui_view_port_input_cb_t)(mui_view_port_t* p_vp, mui_input_event_t* p_event);

typedef struct {
    uint8_t width;
    uint8_t height;
    mui_canvas_t canvas;
    bool enabled;
    
    mui_view_draw_cb_t draw_cb;
    mui_view_port_input_cb_t input_cb;
} mui_view_port_t;




void 




#endif