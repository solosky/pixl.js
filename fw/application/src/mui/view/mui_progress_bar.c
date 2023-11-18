#include "mui_progress_bar.h"

static void mui_progress_bar_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_progress_bar_t *p_mui_progress_bar = p_view->user_data;

    uint8_t font_height = mui_canvas_current_font_height(p_canvas);
    uint8_t canvas_width = mui_canvas_get_width(p_canvas);
    uint8_t canvas_height = mui_canvas_get_height(p_canvas);

    // mui_canvas_set_draw_color(p_canvas, 1);
    // mui_canvas_draw_box(p_canvas, 0, 0, canvas_width, font_height + 1);
    // mui_canvas_set_draw_color(p_canvas, 0);
    mui_canvas_draw_utf8(p_canvas, 2, font_height - 2, string_get_cstr(p_mui_progress_bar->header));
    // mui_canvas_set_draw_color(p_canvas, 1);

    float percent =
        p_mui_progress_bar->current_value / (float)(p_mui_progress_bar->max_value - p_mui_progress_bar->min_value);
    mui_canvas_draw_rframe(p_canvas, 0, (canvas_height - font_height) / 2, canvas_width, font_height, 1);
    mui_canvas_draw_box(p_canvas, 1, (canvas_height - font_height) / 2, ceil((canvas_width - 2) * percent),
                        font_height);

    char txt[32];

    uint8_t text_y = (canvas_height - font_height) / 2 + font_height * 2;
    sprintf(txt, "%d", p_mui_progress_bar->current_value);
    mui_canvas_draw_utf8(p_canvas, (canvas_width - mui_canvas_get_utf8_width(p_canvas, txt)) / 2, text_y, txt);
}

static void mui_progress_bar_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    mui_progress_bar_t *p_pb = p_view->user_data;
    uint8_t step = p_pb->max_value / 20;
    if (event->type == INPUT_TYPE_SHORT || event->type == INPUT_TYPE_REPEAT) {
        switch (event->key) {
        case INPUT_KEY_LEFT:
            if (p_pb->current_value > p_pb->min_value) {
                p_pb->current_value-=step;
            } else {
                p_pb->current_value = p_pb->max_value;
            }
            if (p_pb->event_cb) {
                p_pb->event_cb(MUI_PROGRESS_BAR_EVENT_DECREMENT, p_pb);
            }
            break;
        case INPUT_KEY_RIGHT:
            if (p_pb->current_value < p_pb->max_value) {
                p_pb->current_value+=step;
            } else {
                p_pb->current_value = p_pb->min_value;
            }
            if (p_pb->event_cb) {
                p_pb->event_cb(MUI_PROGRESS_BAR_EVENT_INCREMENT, p_pb);
            }
            break;
        case INPUT_KEY_CENTER:
            if (p_pb->event_cb) {
                p_pb->event_cb(MUI_PROGRESS_BAR_EVENT_CONFIRMED, p_pb);
            }
            break;
        }
    }
}

static void mui_progress_bar_on_enter(mui_view_t *p_view) {}

static void mui_progress_bar_on_exit(mui_view_t *p_view) {}

mui_progress_bar_t *mui_progress_bar_create() {
    mui_progress_bar_t *p_mui_progress_bar = mui_mem_malloc(sizeof(mui_progress_bar_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_mui_progress_bar;
    p_view->draw_cb = mui_progress_bar_on_draw;
    p_view->input_cb = mui_progress_bar_on_input;
    p_view->enter_cb = mui_progress_bar_on_enter;
    p_view->exit_cb = mui_progress_bar_on_exit;

    p_mui_progress_bar->p_view = p_view;
    p_mui_progress_bar->event_cb = NULL;
    p_mui_progress_bar->min_value = 0;
    p_mui_progress_bar->max_value = 100;
    p_mui_progress_bar->current_value = 0;
    string_init(p_mui_progress_bar->header);

    return p_mui_progress_bar;
}

void mui_progress_bar_free(mui_progress_bar_t *p_view) {
    string_clear(p_view->header);
    mui_mem_free(p_view->p_view);
    mui_mem_free(p_view);
}
void mui_progress_bar_reset(mui_progress_bar_t *p_view) {
    string_reset(p_view->header);
    p_view->event_cb = NULL;
}
