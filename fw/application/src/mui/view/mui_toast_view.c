//
// Created by solos on 9/9/2023.
//

#include "mui_toast_view.h"
#include "app_timer.h"

#define MUI_TOAST_DISPLAY_TIME 2000

APP_TIMER_DEF(m_toast_timer_id);
bool m_toast_timer_initialized = false;

static void mui_toast_timer_handler(void *p_context) {
    mui_toast_view_t *p_toast_view = (mui_toast_view_t *)p_context;
    p_toast_view->is_visible = false;
    mui_update(mui());
}

static void mui_toast_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_toast_view_t *p_toast_view = (mui_toast_view_t *)p_view->user_data;
    const char *msg = string_get_cstr(p_toast_view->message);
    if (p_toast_view->is_visible && string_size(p_toast_view->message) > 0) {
        uint8_t mw = mui_canvas_get_width(p_canvas) - 16;
        uint8_t th = mui_element_text_height(p_canvas, mw, msg);
        uint8_t tw = mui_canvas_get_utf8_width(p_canvas, msg);
        uint8_t ft = mui_canvas_current_font_height(p_canvas);

        uint8_t w = LV_MIN(mw, tw);
        uint8_t h = th;
        uint8_t x = 0;
        uint8_t y = 0;

        uint8_t mtw = LV_MIN(mw, tw); // min text width

        // add padding
        w += 6;
        h += 6;

        if (h < mui_canvas_get_height(p_canvas)) {
            x = (mui_canvas_get_width(p_canvas) - w) / 2;
            y = (mui_canvas_get_height(p_canvas) - h) / 2;
        }

        mui_canvas_set_draw_color(p_canvas, 0);
        mui_canvas_draw_box(p_canvas, x, y, w, h);
        mui_canvas_set_draw_color(p_canvas, 1);
        mui_canvas_draw_rframe(p_canvas, x, y, w, h, 3);

        mui_element_autowrap_text(p_canvas, x + 3, y + (ft + th) / 2, w, h,
                                  string_get_cstr(p_toast_view->message));
    }
}

static void mui_toast_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {}

static void mui_toast_view_on_enter(mui_view_t *p_view) {}

static void mui_toast_view_on_exit(mui_view_t *p_view) {}

mui_toast_view_t *mui_toast_view_create() {
    mui_toast_view_t *p_toast_view = (mui_toast_view_t *)mui_mem_malloc(sizeof(mui_toast_view_t));
    memset(p_toast_view, 0, sizeof(mui_toast_view_t));
    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_toast_view;
    p_view->draw_cb = mui_toast_view_on_draw;
    p_view->input_cb = mui_toast_view_on_input;
    p_view->enter_cb = mui_toast_view_on_enter;
    p_view->exit_cb = mui_toast_view_on_exit;

    p_toast_view->p_view = p_view;

    string_init(p_toast_view->message);

    if (!m_toast_timer_initialized) {
        ret_code_t err_code = app_timer_create(&m_toast_timer_id, APP_TIMER_MODE_SINGLE_SHOT, mui_toast_timer_handler);
        APP_ERROR_CHECK(err_code);
        m_toast_timer_initialized = true;
    }

    return p_toast_view;
}
void mui_toast_view_free(mui_toast_view_t *p_view) {
    app_timer_stop(m_toast_timer_id);
    string_clear(p_view->message);
    mui_mem_free(p_view->p_view);
    mui_mem_free(p_view);
}

mui_view_t *mui_toast_view_get_view(mui_toast_view_t *p_view) { return p_view->p_view; }

void mui_toast_view_show(mui_toast_view_t *p_view, const char *message) {
    string_set_str(p_view->message, message);
    p_view->is_visible = true;
    ret_code_t err_code = app_timer_stop(m_toast_timer_id);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(m_toast_timer_id, APP_TIMER_TICKS(MUI_TOAST_DISPLAY_TIME), p_view);
    APP_ERROR_CHECK(err_code);
}
