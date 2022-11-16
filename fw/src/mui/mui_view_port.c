#include "mui_view_port.h"

void mui_view_port_input(mui_view_port_t *p_vp, mui_input_event_t *p_event) {
    if (p_vp->input_cb) {
        p_vp->input_cb(p_vp, p_event);
    }
}
void mui_view_port_draw(mui_view_port_t *p_vp) {
    if (p_vp->draw_cb) {
        p_vp->draw_cb(p_vp, &p_vp->canvas);
    }
}