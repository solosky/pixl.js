#include "mui_text_input.h"

#define LIST_ITEM_HEIGHT 13

typedef struct {
    const char text;
    const uint8_t x;
    const uint8_t y;
} mui_text_input_key_t;

static const uint8_t keyboard_origin_x = 1;
static const uint8_t keyboard_origin_y = 29;
static const uint8_t keyboard_row_count = 3;

static const mui_text_input_key_t keyboard_keys_row_1[] = {
    {'q', 1, 8},  {'w', 10, 8}, {'e', 19, 8}, {'r', 28, 8}, {'t', 37, 8},  {'y', 46, 8},  {'u', 55, 8},
    {'i', 64, 8}, {'o', 73, 8}, {'p', 82, 8}, {'0', 91, 8}, {'1', 100, 8}, {'2', 110, 8}, {'3', 120, 8},
};

static const mui_text_input_key_t keyboard_keys_row_2[] = {
    {'a', 1, 20},   {'s', 10, 20},  {'d', 19, 20},  {'f', 28, 20}, {'g', 37, 20},
    {'h', 46, 20},  {'j', 55, 20},  {'k', 64, 20},  {'l', 73, 20}, {BACKSPACE_KEY, 82, 20},
    {'4', 100, 20}, {'5', 110, 20}, {'6', 120, 20},
};

static const mui_text_input_key_t keyboard_keys_row_3[] = {
    {'z', 1, 32},  {'x', 10, 32}, {'c', 19, 32},       {'v', 28, 32},  {'b', 37, 32},  {'n', 46, 32},  {'m', 55, 32},
    {'_', 64, 32}, {'.', 73, 32}, {ENTER_KEY, 82, 32}, {'7', 100, 32}, {'8', 110, 32}, {'9', 120, 32},
};

static bool char_is_lowercase(char letter) { return (letter >= 0x61 && letter <= 0x7A); }

static char char_to_uppercase(const char letter) {
    if (letter == '_') {
        return 0x20;
    } else if (isalpha(letter)) {
        return (letter - 0x20);
    } else {
        return letter;
    }
}

static uint8_t get_row_size(uint8_t row_index) {
    uint8_t row_size = 0;

    switch (row_index + 1) {
    case 1:
        row_size = sizeof(keyboard_keys_row_1) / sizeof(mui_text_input_key_t);
        break;
    case 2:
        row_size = sizeof(keyboard_keys_row_2) / sizeof(mui_text_input_key_t);
        break;
    case 3:
        row_size = sizeof(keyboard_keys_row_3) / sizeof(mui_text_input_key_t);
        break;
    }

    return row_size;
}

static const mui_text_input_key_t *get_row(uint8_t row_index) {
    const mui_text_input_key_t *row = NULL;

    switch (row_index + 1) {
    case 1:
        row = keyboard_keys_row_1;
        break;
    case 2:
        row = keyboard_keys_row_2;
        break;
    case 3:
        row = keyboard_keys_row_3;
        break;
    }

    return row;
}

static void mui_text_input_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    mui_text_input_t *p_mui_text_input = p_view->user_data;

    const char *text = string_get_cstr(p_mui_text_input->input_text);
    uint8_t needed_string_width = mui_canvas_get_width(p_canvas) - 8;
    uint8_t start_pos = 4;
    uint8_t text_length = string_size(p_mui_text_input->input_text);

    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_canvas_set_draw_color(p_canvas, 1);
    mui_canvas_draw_utf8(p_canvas, 2, 10, string_get_cstr(p_mui_text_input->header));

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    mui_canvas_draw_rframe(p_canvas, 1, 12, 126, 15, 1);

    if (mui_canvas_string_width(p_canvas, text) > needed_string_width) {
        mui_canvas_draw_utf8(p_canvas, start_pos, 22, "...");
        start_pos += 6;
        needed_string_width -= 8;
    }

    while (text != 0 && mui_canvas_string_width(p_canvas, text) > needed_string_width) {
        text++;
    }

    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    if (p_mui_text_input->clear_default_text) {
        mui_canvas_draw_rframe(p_canvas, start_pos - 1, 14, mui_canvas_string_width(p_canvas, text) + 2, 10, 1);
    } else {
        mui_canvas_draw_utf8(p_canvas, start_pos + mui_canvas_string_width(p_canvas, text) + 1, 22, "_");
    }
    mui_canvas_draw_utf8(p_canvas, start_pos, 22, text);

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);

    for (uint8_t row = 0; row <= keyboard_row_count; row++) {
        const uint8_t column_count = get_row_size(row);
        const mui_text_input_key_t *keys = get_row(row);

        for (size_t column = 0; column < column_count; column++) {
            if (keys[column].text == ENTER_KEY) {
                mui_canvas_set_draw_color(p_canvas, 1);
                if (p_mui_text_input->focus_row == row && p_mui_text_input->focus_column == column) {
                    mui_canvas_set_draw_color(p_canvas, 1);
                    mui_canvas_draw_box(p_canvas, keyboard_origin_x + keys[column].x - 1,
                                        keyboard_origin_y + keys[column].y - 8, 14, 10);
                    mui_canvas_set_draw_color(p_canvas, 0);
                    mui_canvas_draw_utf8(p_canvas, keyboard_origin_x + keys[column].x,
                                         keyboard_origin_y + keys[column].y, "OK");
                    mui_canvas_set_draw_color(p_canvas, 1);
                } else {
                    mui_canvas_draw_rframe(p_canvas, keyboard_origin_x + keys[column].x - 2,
                                           keyboard_origin_y + keys[column].y - 8, 14, 10, 1);
                    mui_canvas_draw_utf8(p_canvas, keyboard_origin_x + keys[column].x,
                                         keyboard_origin_y + keys[column].y, "OK");
                }
            } else if (keys[column].text == BACKSPACE_KEY) {
                mui_canvas_set_draw_color(p_canvas, 1);
                if (p_mui_text_input->focus_row == row && p_mui_text_input->focus_column == column) {
                    mui_canvas_set_draw_color(p_canvas, 1);
                    mui_canvas_draw_box(p_canvas, keyboard_origin_x + keys[column].x - 1,
                                        keyboard_origin_y + keys[column].y - 8, 7, 10);
                    mui_canvas_set_draw_color(p_canvas, 0);
                    mui_canvas_draw_glyph(p_canvas, keyboard_origin_x + keys[column].x,
                                          keyboard_origin_y + keys[column].y, 0xe12e);
                    mui_canvas_set_draw_color(p_canvas, 1);
                } else {

                    mui_canvas_draw_glyph(p_canvas, keyboard_origin_x + keys[column].x,
                                          keyboard_origin_y + keys[column].y, 0xe12e);
                }
            } else {
                if (p_mui_text_input->focus_row == row && p_mui_text_input->focus_column == column) {
                    mui_canvas_set_draw_color(p_canvas, 1);
                    mui_canvas_draw_box(p_canvas, keyboard_origin_x + keys[column].x - 1,
                                        keyboard_origin_y + keys[column].y - 8, 7, 10);
                    mui_canvas_set_draw_color(p_canvas, 0);
                } else {
                    mui_canvas_set_draw_color(p_canvas, 1);
                }

                if (p_mui_text_input->clear_default_text ||
                    (text_length == 0 && char_is_lowercase(keys[column].text))) {
                    mui_canvas_draw_glyph(p_canvas, keyboard_origin_x + keys[column].x,
                                          keyboard_origin_y + keys[column].y, char_to_uppercase(keys[column].text));
                } else {
                    mui_canvas_draw_glyph(p_canvas, keyboard_origin_x + keys[column].x,
                                          keyboard_origin_y + keys[column].y, keys[column].text);
                }
            }
        }
    }
}

static void mui_text_input_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    mui_text_input_t *p_mui_text_input = p_view->user_data;
    if (event->type == INPUT_TYPE_SHORT || event->type == INPUT_TYPE_REPEAT || event->type == INPUT_TYPE_LONG) {
        switch (event->key) {
        case INPUT_KEY_LEFT:
            if (event->type == INPUT_TYPE_SHORT || event->type == INPUT_TYPE_REPEAT) {
                if (p_mui_text_input->focus_column > 0) {
                    p_mui_text_input->focus_column--;
                } else {
                    if (p_mui_text_input->focus_row > 0) {
                        p_mui_text_input->focus_row--;
                    } else {
                        p_mui_text_input->focus_row = 2;
                    }
                    p_mui_text_input->focus_column = get_row_size(p_mui_text_input->focus_row) - 1;
                }
            } else if (event->type == INPUT_TYPE_LONG) {
                if (p_mui_text_input->focus_row > 0) {
                    p_mui_text_input->focus_row--;
                } else {
                    p_mui_text_input->focus_row = 2;
                }
            }
            break;

        case INPUT_KEY_RIGHT:
            if (event->type == INPUT_TYPE_SHORT || event->type == INPUT_TYPE_REPEAT) {
                if (p_mui_text_input->focus_column < get_row_size(p_mui_text_input->focus_row) - 1) {
                    p_mui_text_input->focus_column++;
                } else {
                    if (p_mui_text_input->focus_row < 2) {
                        p_mui_text_input->focus_row++;
                    } else {
                        p_mui_text_input->focus_row = 0;
                    }
                    p_mui_text_input->focus_column = 0;
                }
            } else if (event->type == INPUT_TYPE_LONG) {
                if (p_mui_text_input->focus_row < 2) {
                    p_mui_text_input->focus_row++;
                } else {
                    p_mui_text_input->focus_row = 0;
                }
            }
            break;

        case INPUT_KEY_CENTER: {
            char c = get_row(p_mui_text_input->focus_row)[p_mui_text_input->focus_column].text;
            if (c == BACKSPACE_KEY) {
                string_t current;
                string_init_set(current, p_mui_text_input->input_text);
                string_set_strn(p_mui_text_input->input_text, string_get_cstr(current), string_size(current) - 1);
                string_clear(current);
            } else if (c == ENTER_KEY) {
                if (p_mui_text_input->event_cb) {
                    p_mui_text_input->event_cb(MUI_TEXT_INPUT_EVENT_CONFIRMED, p_mui_text_input);
                }
            } else {
                if (event->type == INPUT_TYPE_LONG) {
                    c = char_to_uppercase(c);
                }
                string_push_back(p_mui_text_input->input_text, c);
            }
        } break;
        }
    }
}

static void mui_text_input_on_enter(mui_view_t *p_view) {}

static void mui_text_input_on_exit(mui_view_t *p_view) {}

mui_text_input_t *mui_text_input_create() {
    mui_text_input_t *p_mui_text_input = mui_mem_malloc(sizeof(mui_text_input_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_mui_text_input;
    p_view->draw_cb = mui_text_input_on_draw;
    p_view->input_cb = mui_text_input_on_input;
    p_view->enter_cb = mui_text_input_on_enter;
    p_view->exit_cb = mui_text_input_on_exit;

    p_mui_text_input->p_view = p_view;
    p_mui_text_input->focus_row = 0;
    p_mui_text_input->focus_column = 0;
    p_mui_text_input->clear_default_text = false;
    string_init(p_mui_text_input->header);
    string_init(p_mui_text_input->input_text);
    p_mui_text_input->event_cb = NULL;

    return p_mui_text_input;
}

void mui_text_input_free(mui_text_input_t *p_view) {
    string_clear(p_view->header);
    string_clear(p_view->input_text);
    mui_mem_free(p_view->p_view);
    mui_mem_free(p_view);
}

mui_view_t *mui_text_input_get_view(mui_text_input_t *p_view) { return p_view->p_view; }

void mui_text_input_set_user_data(mui_text_input_t *p_view, void *user_data) { p_view->user_data = user_data; }

//// view functions //

void mui_text_input_set_event_cb(mui_text_input_t *p_view, mui_text_input_event_cb_t event_cb) {
    p_view->event_cb = event_cb;
}
void mui_text_input_clear_input_text(mui_text_input_t *p_view) { string_reset(p_view->input_text); }
void mui_text_input_set_input_text(mui_text_input_t *p_view, const char *input) {
    string_set_str(p_view->input_text, input);
}
void mui_text_input_set_header(mui_text_input_t *p_view, const char *title) { string_set_str(p_view->header, title); }
const char *mui_text_input_get_input_text(mui_text_input_t *p_view) { return string_get_cstr(p_view->input_text); }

void mui_text_input_reset(mui_text_input_t *p_view) {
    p_view->focus_row = 0;
    p_view->focus_column = 0;
    p_view->clear_default_text = false;
    string_reset(p_view->header);
    string_reset(p_view->input_text);
}

void mui_text_input_set_focus_key(mui_text_input_t *p_view, char key) {
    for (uint32_t row = 0; row < 3; row++) {
        mui_text_input_key_t *input_key = get_row(row);
        for (uint32_t col = 0; col < get_row_size(row); col++) {
            if (input_key[col].text == key) {
                p_view->focus_column = col;
                p_view->focus_row = row;
                return;
            }
        }
    }
}