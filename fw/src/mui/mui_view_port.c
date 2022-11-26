#include "mui_view_port.h"


mui_view_port_t *mui_view_port_create(){
    mui_view_port_t* p_vp = malloc(sizeof(mui_view_port_t));
    return p_vp;

}
void mui_view_port_free(mui_view_port_t* p_vp){
    free(p_vp);
}

void mui_view_port_input(mui_view_port_t *p_vp, mui_input_event_t *p_event) {
    if (p_vp->input_cb) {
        p_vp->input_cb(p_vp, p_event);
    }
}
void mui_view_port_draw(mui_view_port_t *p_vp, mui_canvas_t *p_canvas) {
    if (p_vp->draw_cb) {
        p_vp->draw_cb(p_vp, p_canvas);
    }
}

void mui_view_port_enable_set(mui_view_port_t* p_vp, bool enabled){
    p_vp->enabled = enabled;
}

void mui_view_port_update(mui_view_port_t* p_vp){
    if(p_vp->enabled){
        mui_update(mui());
    }
}
