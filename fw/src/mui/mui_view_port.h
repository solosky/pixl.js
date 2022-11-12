#ifndef MUI_VIEW_PORT_H
#define MUI_VIEW_PORT_H

#include <stdint.h>
#include "mui_canvas.h"


typedef void (* mui_view_port_draw_cb_t)();

typedef struct {
    uint8_t width;
    uint8_t height;
    mui_canvas_t canvas;
    bool enabled;
    
    mui_view_draw_cb_t draw_cb;
    void* draw_cb_context;
    

} mui_view_port_t;


void 




#endif