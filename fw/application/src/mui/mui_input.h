#ifndef MUI_INPUT_H
#define MUI_INPUT_H

#include "mui_common.h"
#include "bsp.h"


typedef enum {
    INPUT_KEY_LEFT,
    INPUT_KEY_CENTER,
    INPUT_KEY_RIGHT
} input_key_t;

typedef enum {
    INPUT_TYPE_PRESS,
    INPUT_TYPE_RELEASE,
    INPUT_TYPE_SHORT,
    INPUT_TYPE_LONG,
    INPUT_TYPE_REPEAT

} input_type_t;


typedef struct {
    input_key_t key;
    input_type_t type;
} mui_input_event_t;


void mui_input_init();

void mui_input_on_bsp_event(bsp_event_t evt);

#endif