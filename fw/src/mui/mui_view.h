#ifndef MUI_VIEW_H
#define MUI_VIEW_H

#include "mui_canvas.h"
#include "mui_input.h"


typedef struct {
    //properties
    void* model;
    void* context;

    //callbacks 
    mui_event_handler_t handler;
    
} mui_view_t;

#endif

