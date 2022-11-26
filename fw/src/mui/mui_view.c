#include "mui_view.h"


void mui_view_draw(mui_view_t* p_view, mui_canvas_t* p_canvas){
    if(p_view->draw_cb){
        p_view->draw_cb(p_view, p_canvas);
    }

}
void mui_view_input(mui_view_t* p_view, mui_input_event_t* p_event){
    if(p_view->input_cb){
        p_view->input_cb(p_view, p_event);
    }
}
void mui_view_enter(mui_view_t* p_view){
    if(p_view->enter_cb){
        p_view->enter_cb(p_view);
    }
}
void mui_view_exit(mui_view_t* p_view){
      if(p_view->exit_cb){
        p_view->exit_cb(p_view);
    }
}

mui_view_t* mui_view_create(){
    return malloc(sizeof(mui_view_t));
}


void mui_view_free(mui_view_t* p_view){
    free(p_view);
}
