#ifndef MUI_CORE_H
#define MUI_CORE_H

#include "mui_defines.h"
#include <stdint.h>
#include "u8g2.h"

#include "m-array.h"
#include "m-algo.h"
#include "mui_event.h"
#include "mui_input.h"
#include "mui_view_port.h"

#define SCREEN_HEIGHT 86
#define SCREEN_WIDTH 160
#define LAYER_STATUS_BAR_HEIGHT 13


ARRAY_DEF(mui_view_port_array, mui_view_port_t*, M_PTR_OPLIST);

typedef enum {
    MUI_LAYER_DESKTOP, 
    MUI_LAYER_WINDOW,
    MUI_LAYER_STATUS_BAR, 
    MUI_LAYER_FULLSCREEN,
    MUI_LAYER_MAX
} mui_layer_t;

typedef struct {
    uint8_t initialized;
    uint8_t screen_width;
    uint8_t screen_height;
    u8g2_t u8g2;
    mui_canvas_t canvas;
    mui_event_queue_t event_queue;
    mui_view_port_array_t layers[MUI_LAYER_MAX];
} mui_t;

mui_t* mui();

void mui_init(mui_t* p_mui);
void mui_post(mui_t* p_mui, mui_event_t*p_event);
void mui_tick(mui_t* p_mui);
void mui_deinit(mui_t *p_mui);
void mui_panic(mui_t *p_mui, char* err);


void mui_add_view_port(mui_t* p_mui, mui_view_port_t* p_vp, mui_layer_t layer);
void mui_remove_view_port(mui_t* p_mui, mui_view_port_t* p_vp, mui_layer_t layer);
void mui_update(mui_t* p_mui);


#endif