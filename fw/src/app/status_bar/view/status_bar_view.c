#include "status_bar_view.h"

static void status_bar_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_canvas_draw_utf8(p_canvas, 0, 0, "12:45");
}

static void status_bar_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {}

static void status_bar_view_on_enter(mui_view_t *p_view) {}

static void status_bar_view_on_exit(mui_view_t *p_view) {}

status_bar_view_t *status_bar_view_create() {
    status_bar_view_t *p_status_bar_view = malloc(sizeof(status_bar_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_status_bar_view;
    p_view->draw_cb = status_bar_view_on_draw;
    p_view->input_cb = status_bar_view_on_input;
    p_view->enter_cb = status_bar_view_on_enter;
    p_view->exit_cb = status_bar_view_on_exit;

    p_status_bar_view->p_view = p_view;

    return p_status_bar_view;
}
void status_bar_view_free(status_bar_view_t *p_view) {
    free(p_view->p_view);
    free(p_view);
}
mui_view_t *status_bar_view_get_view(status_bar_view_t *p_view) { return p_view->p_view; }