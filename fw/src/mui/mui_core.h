#ifndef MUI_CORE_H
#define MUI_CORE_H

#include <stdint.h>
#include "u8g2.h"

#include "m-array.h"
#include "m-algo.h"
#include "mui_event.h"
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
    uint8_t screen_width;
    uint8_t screen_height;
    u8g2_t u8g2;
    mui_event_queue_t event_queue;
    mui_view_port_array_t layers[MUI_LAYER_MAX];
} mui_t;


mui_t* mui();

void mui_init(mui_t* p_mui);
void mui_post(mui_t* p_mui, mui_event_t*p_event);
void mui_tick(mui_t* p_mui);



#endif