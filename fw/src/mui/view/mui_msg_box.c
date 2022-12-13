#include "mui_msg_box.h"
#include "mui_element.h"
#include "mui_mem.h"

static uint8_t mui_msg_box_get_button_num(mui_msg_box_t *p_mui_msg_box) {
    return (string_size(p_mui_msg_box->btn_left_text) > 0) + (string_size(p_mui_msg_box->btn_center_text) > 0) +
           (string_size(p_mui_msg_box->btn_right_text) > 0);
}

static void mui_msg_box_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_msg_box_t *p_mui_msg_box = p_view->user_data;

    uint8_t font_height = mui_canvas_current_font_height(p_canvas);
    uint8_t canvas_width = mui_canvas_get_width(p_canvas);
    uint8_t canvas_height = mui_canvas_get_height(p_canvas);

    mui_canvas_set_draw_color(p_canvas, 1);
    mui_canvas_draw_box(p_canvas, 0, 0, canvas_width, font_height + 1);
    mui_canvas_set_draw_color(p_canvas, 0);
    mui_canvas_draw_utf8(p_canvas, 2, font_height - 2, string_get_cstr(p_mui_msg_box->header));
    mui_canvas_set_draw_color(p_canvas, 1);

    mui_element_multiline_text(p_canvas, 0, font_height * 2 + 2, string_get_cstr(p_mui_msg_box->message));

    // Draw buttons
    if (string_size(p_mui_msg_box->btn_left_text) > 0) {
        mui_element_button(p_canvas, 0, canvas_height - font_height - 2, string_get_cstr(p_mui_msg_box->btn_left_text),
                           p_mui_msg_box->focus == 0);
    }

    if (string_size(p_mui_msg_box->btn_center_text) > 0) {
        const char *text = string_get_cstr(p_mui_msg_box->btn_center_text);
        mui_element_button(p_canvas, (canvas_width - mui_canvas_get_utf8_width(p_canvas, text)) / 2,
                           canvas_height - font_height - 2, text, p_mui_msg_box->focus == 1);
    }

    if (string_size(p_mui_msg_box->btn_right_text) > 0) {
        const char *text = string_get_cstr(p_mui_msg_box->btn_right_text);
        mui_element_button(p_canvas, canvas_width - mui_canvas_get_utf8_width(p_canvas, text) - 4,
                           canvas_height - font_height - 2, text, p_mui_msg_box->focus == 2);
    }
}

static void mui_msg_box_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    mui_msg_box_t *p_mui_msg_box = p_view->user_data;
    if (event->type == INPUT_TYPE_SHORT) {
        switch (event->key) {
        case INPUT_KEY_LEFT:
            if (mui_msg_box_get_button_num(p_mui_msg_box) > 1) {
                uint8_t state[3] = {string_size(p_mui_msg_box->btn_left_text) > 0,
                                    string_size(p_mui_msg_box->btn_center_text) > 0,
                                    string_size(p_mui_msg_box->btn_right_text) > 0};
                uint8_t focus = p_mui_msg_box->focus;
                do {
                    focus--;
                    if (focus == 0xFF) {
                        focus = 2;
                    }

                } while (!state[focus]);
                p_mui_msg_box->focus = focus;
            }
            break;
        case INPUT_KEY_RIGHT:
            if (mui_msg_box_get_button_num(p_mui_msg_box) > 1) {
                uint8_t state[3] = {string_size(p_mui_msg_box->btn_left_text) > 0,
                                    string_size(p_mui_msg_box->btn_center_text) > 0,
                                    string_size(p_mui_msg_box->btn_right_text) > 0};
                uint8_t focus = p_mui_msg_box->focus;
                do {
                    focus++;
                    if (focus == 3) {
                        focus = 0;
                    }

                } while (!state[focus]);
                p_mui_msg_box->focus = focus;
            }

            break;

        case INPUT_KEY_CENTER:
            if (p_mui_msg_box->event_cb) {
                p_mui_msg_box->event_cb(p_mui_msg_box->focus, p_mui_msg_box);
            }
            break;
        }
    }
}

static void mui_msg_box_on_enter(mui_view_t *p_view) {}

static void mui_msg_box_on_exit(mui_view_t *p_view) {}

mui_msg_box_t *mui_msg_box_create() {
    mui_msg_box_t *p_mui_msg_box = mui_mem_malloc(sizeof(mui_msg_box_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_mui_msg_box;
    p_view->draw_cb = mui_msg_box_on_draw;
    p_view->input_cb = mui_msg_box_on_input;
    p_view->enter_cb = mui_msg_box_on_enter;
    p_view->exit_cb = mui_msg_box_on_exit;

    p_mui_msg_box->p_view = p_view;
    string_init(p_mui_msg_box->header);
    string_init(p_mui_msg_box->message);
    string_init(p_mui_msg_box->btn_center_text);
    string_init(p_mui_msg_box->btn_left_text);
    string_init(p_mui_msg_box->btn_right_text);
    p_mui_msg_box->event_cb = NULL;
    p_mui_msg_box->focus = 0;

    return p_mui_msg_box;
}

void mui_msg_box_free(mui_msg_box_t *p_view) {
    string_clear(p_view->header);
    string_clear(p_view->message);
    string_clear(p_view->btn_center_text);
    string_clear(p_view->btn_left_text);
    string_clear(p_view->btn_right_text);
    mui_mem_free(p_view->p_view);
    mui_mem_free(p_view);
}

mui_view_t *mui_msg_box_get_view(mui_msg_box_t *p_view) { return p_view->p_view; }

void mui_msg_box_set_user_data(mui_msg_box_t *p_view, void *user_data) { p_view->user_data = user_data; }

void *mui_msg_box_get_user_data(mui_msg_box_t *p_view) { return p_view->user_data; }

//// view functions //
void mui_msg_box_set_event_cb(mui_msg_box_t *p_view, mui_msg_box_event_cb_t event_cb) { p_view->event_cb = event_cb; }
void mui_msg_box_set_header(mui_msg_box_t *p_view, const char *header) { string_set_str(p_view->header, header); }
void mui_msg_box_set_message(mui_msg_box_t *p_view, const char *message) { string_set_str(p_view->message, message); }
void mui_msg_box_set_btn_focus(mui_msg_box_t *p_view, uint8_t focus) { p_view->focus = focus; }
void mui_msg_box_set_btn_text(mui_msg_box_t *p_view, const char *left, const char *center, const char *right) {
    if (left != NULL) {
        string_set_str(p_view->btn_left_text, left);
    } else {
        string_reset(p_view->btn_left_text);
    }

    if (center != NULL) {
        string_set_str(p_view->btn_center_text, center);
    } else {
        string_reset(p_view->btn_center_text);
    }
    if (right != NULL) {
        string_set_str(p_view->btn_right_text, right);
    } else {
        string_reset(p_view->btn_right_text);
    }
}
void mui_msg_box_reset(mui_msg_box_t *p_view) {
    string_reset(p_view->header);
    string_reset(p_view->message);
    string_reset(p_view->btn_center_text);
    string_reset(p_view->btn_left_text);
    string_reset(p_view->btn_right_text);
}