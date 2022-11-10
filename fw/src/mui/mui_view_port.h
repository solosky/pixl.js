#ifndef MUI_VIEW_PORT_H
#define MUI_VIEW_PORT_H

#include <stdint.h>
#include "mui_canvas.h"

typedef struct {
    uint8_t width;
    uint8_t height;
    mui_canvas_t canvas;
    bool enabled;
} mui_view_port_t;

#endif