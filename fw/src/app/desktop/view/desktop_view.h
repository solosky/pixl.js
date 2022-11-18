#ifndef DESKTOP_VIEW_H
#define DESKTOP_VIEW_H
#include "mui_include.h"
#include <stdint.h>

typedef struct {
    mui_view_t* p_view;
    uint8_t focus_index;
} desktop_view_t;

desktop_view_t* desktop_view_create();
void desktop_view_free(desktop_view_t* p_view);
mui_view_t* desktop_view_get_view(desktop_view_t* p_view);


#endif