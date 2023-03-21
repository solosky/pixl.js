#include "amiibolink_view.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"

static void amiibolink_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    amiibolink_view_t *p_amiibolink_view = p_view->user_data;
    mui_canvas_draw_utf8(p_canvas, 0, 10, "<未播放动画>");
}

static void amiibolink_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    amiibolink_view_t *p_amiibolink_view = p_view->user_data;
    if (p_amiibolink_view->event_cb) {
        p_amiibolink_view->event_cb(AMIIBOLINK_VIEW_EVENT_KEY_PRESSED, p_amiibolink_view);
    }
}

static void amiibolink_view_on_enter(mui_view_t *p_view) {}

static void amiibolink_view_on_exit(mui_view_t *p_view) {}

amiibolink_view_t *amiibolink_view_create() {
    amiibolink_view_t *p_amiibolink_view = mui_mem_malloc(sizeof(amiibolink_view_t));
    memset(p_amiibolink_view, 0, sizeof(amiibolink_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_amiibolink_view;
    p_view->draw_cb = amiibolink_view_on_draw;
    p_view->input_cb = amiibolink_view_on_input;
    p_view->enter_cb = amiibolink_view_on_enter;
    p_view->exit_cb = amiibolink_view_on_exit;

    p_amiibolink_view->p_view = p_view;

    return p_amiibolink_view;
}
void amiibolink_view_free(amiibolink_view_t *p_view) {
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}
mui_view_t *amiibolink_view_get_view(amiibolink_view_t *p_view) { return p_view->p_view; }
