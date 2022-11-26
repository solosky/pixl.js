#include "desktop_view.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"

static void desktop_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    uint8_t app_total = mini_app_registry_get_app_num();
    mini_app_t *p_app_array = mini_app_registry_get_app_array();
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
}

static void desktop_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    desktop_view_t *p_desktop_view = p_view->user_data;
    mini_app_t *p_app_array = mini_app_registry_get_app_array();
    switch (event->key) {
    case INPUT_KEY_LEFT:
        if (p_desktop_view->focus_index > 0) {
            p_desktop_view->focus_index--;
        }
        break;
    case INPUT_KEY_RIGHT:
        if (p_desktop_view->focus_index < mini_app_registry_get_app_num()) {
            p_desktop_view->focus_index++;
        }
        break;
    case INPUT_KEY_CENTER:
        mini_app_launcher_run(mini_app_launcher(),
                              p_app_array[p_desktop_view->focus_index].id);
        break;
    }
}

static void desktop_view_on_enter(mui_view_t *p_view) {}

static void desktop_view_on_exit(mui_view_t *p_view) {}

desktop_view_t *desktop_view_create() {
    desktop_view_t *p_desktop_view = malloc(sizeof(desktop_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_desktop_view;
    p_view->draw_cb = desktop_view_on_draw;
    p_view->input_cb = desktop_view_on_input;
    p_view->enter_cb = desktop_view_on_enter;
    p_view->exit_cb = desktop_view_on_exit;

    p_desktop_view->p_view = p_view;
    p_desktop_view->focus_index = 0;

    return p_desktop_view;
}
void desktop_view_free(desktop_view_t *p_view) {
    free(p_view->p_view);
    free(p_view);
}
mui_view_t *desktop_view_get_view(desktop_view_t *p_view) { return p_view->p_view; }