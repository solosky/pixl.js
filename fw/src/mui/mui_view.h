#ifndef MUI_VIEW_H
#define MUI_VIEW_H

#include "mui_canvas.h"
#include "mui_input.h"


typedef struct {
    //properties
    void* model;
    void* context;

    //callbacks 
    void (* on_draw)(mui_canvas_t* p_canvas);
    void (* on_input)(input_event_t* p_event);
    void (* on_enter)();
    void (* on_exit)();
    
} mui_view_t;

#endif

