#ifndef MUI_CORE_H
#define MUI_CORE_H

#include "u8g2.h"

#include "m-array.h"
#include "m-algo.h"
#include "mui_view_port.h"

ARRAY_DEF(mui_view_port_array, mui_view_port_t*, M_PTR_OPLIST);

typedef enum {
    MUI_LAYER_DESKTOP, 
    MUI_LAYER_WINDOW,
    MUI_LAYER_STATUS_BAR, 
    MUI_LAYER_FULLSCREEN,
    MUI_LAYER_MAX
} mui_layer_t;

typedef struct {
    u8g2_t u8g2;
    mui_view_port_array_t layers[MUI_LAYER_MAX];
} mui_t;


void mui_init(mui_t* p_mui);
void mui_redraw(mui_t* p_mui);


#endif