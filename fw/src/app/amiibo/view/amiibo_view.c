#include "amiibo_view.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"

static void amiibo_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    amiibo_view_t *p_amiibo_view = p_view->user_data;
    mui_canvas_draw_utf8(p_canvas, 0, 10, "这是amiibo!!");
}

static void amiibo_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    amiibo_view_t *p_amiibo_view = p_view->user_data;
    switch (event->key) {
    case INPUT_KEY_LEFT:

        break;
    case INPUT_KEY_RIGHT:

        break;
    case INPUT_KEY_CENTER:
        mini_app_launcher_run(mini_app_launcher(), MINI_APP_ID_DESKTOP);

        break;
    }
}

static void amiibo_view_on_enter(mui_view_t *p_view) {}

static void amiibo_view_on_exit(mui_view_t *p_view) {}

amiibo_view_t *amiibo_view_create() {
    amiibo_view_t *p_amiibo_view = malloc(sizeof(amiibo_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_amiibo_view;
    p_view->draw_cb = amiibo_view_on_draw;
    p_view->input_cb = amiibo_view_on_input;
    p_view->enter_cb = amiibo_view_on_enter;
    p_view->exit_cb = amiibo_view_on_exit;

    p_amiibo_view->p_view = p_view;
    p_amiibo_view->focus_index = 0;

    return p_amiibo_view;
}
void amiibo_view_free(amiibo_view_t *p_view) {
    free(p_view->p_view);
    free(p_view);
}
mui_view_t *amiibo_view_get_view(amiibo_view_t *p_view) { return p_view->p_view; }