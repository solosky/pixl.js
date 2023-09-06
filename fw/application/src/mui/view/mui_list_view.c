#include "mui_list_view.h"
#include "mui_element.h"
#include "settings.h"

#include "mui_math.h"
#include "mui_mem.h"

#include "nrf_log.h"

#define LIST_ITEM_HEIGHT 13

#define LIST_ANIM_SHORT_TIME 150
#define LIST_ANIM_LONG_TIME 300

static bool mui_list_view_anim_enabled() { return settings_get_data()->anim_enabled; }

static uint16_t mui_list_view_get_utf8_width(const char *str) { return u8g2_GetUTF8Width(&(mui()->u8g2), str); }

static void mui_list_view_start_text_anim(mui_list_view_t *p_view) {
    mui_list_item_t *p_item = mui_list_item_array_get(p_view->items, p_view->focus_index);
    uint32_t focus_text_width = mui_list_view_get_utf8_width(string_get_cstr(p_item->text));
    if (focus_text_width > p_view->canvas_width - 13) {
        p_view->text_offset = 0;
        int32_t overflowed_width = focus_text_width - p_view->canvas_width + 20;
        mui_anim_set_time(&p_view->text_anim, overflowed_width * 100);
        mui_anim_set_values(&p_view->text_anim, 0, -overflowed_width);
        mui_anim_set_auto_restart(&p_view->text_anim, true);
        mui_anim_start(&p_view->text_anim);
    } else {
        p_view->text_offset = 0;
        mui_anim_stop(&p_view->text_anim);
    }
}

static void mui_list_view_anim_exec(void *p, int32_t value) {
    mui_list_view_t *p_view = (mui_list_view_t *)p;
    p_view->anim_value = value;
}

static void mui_list_view_text_anim_exec(void *p, int32_t value) {
    mui_list_view_t *p_view = (mui_list_view_t *)p;
    p_view->text_offset = value;
}

static void mui_list_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_list_view_t *p_mui_list_view = p_view->user_data;
    p_mui_list_view->canvas_height = p_canvas->height;
    p_mui_list_view->canvas_width = p_canvas->width;

    mui_list_item_array_it_t it;

    mui_list_item_array_it(it, p_mui_list_view->items);

    uint32_t offset_y = p_mui_list_view->scroll_offset;
    if (p_mui_list_view->anim_type == LIST_ANIM_SCROLL) {
        offset_y += p_mui_list_view->anim_value;
    }
    uint32_t index = 0;
    uint32_t focus_y = 0;
    mui_rect_t clip_win_prev;
    mui_rect_t clip_win_cur;
    while (!mui_list_item_array_end_p(it)) {
        mui_list_item_t *item = mui_list_item_array_ref(it);
        int32_t y = index * LIST_ITEM_HEIGHT - offset_y;
        int32_t text_offset = index == p_mui_list_view->focus_index ? p_mui_list_view->text_offset : 0;
        if (y >= -LIST_ITEM_HEIGHT && y <= mui_canvas_get_height(p_canvas)) { // visible object
            mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
            mui_canvas_draw_glyph(p_canvas, 0, y + 10, item->icon);
            mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
            mui_canvas_get_clip_window(p_canvas, &clip_win_prev);
            clip_win_cur.x = 13;
            clip_win_cur.y = y;
            clip_win_cur.h = LIST_ITEM_HEIGHT, clip_win_cur.w = mui_canvas_get_width(p_canvas);
            mui_canvas_set_clip_window(p_canvas, &clip_win_cur);
            mui_canvas_draw_utf8_clip(p_canvas, 13 + text_offset, y + 10, string_get_cstr(item->text));
            mui_canvas_set_clip_window(p_canvas, &clip_win_prev);
        }

        mui_list_item_array_next(it);
        index++;
    }
    int32_t focus_yi = p_mui_list_view->focus_index * LIST_ITEM_HEIGHT - offset_y;
    if (p_mui_list_view->anim_type == LIST_ANIM_FOCUS) {
        focus_yi = focus_yi + p_mui_list_view->anim_value;
    }
    focus_yi = LV_MIN(LV_MAX(focus_yi, 0), mui_canvas_get_height(p_canvas) - LIST_ITEM_HEIGHT);
    uint32_t focus_h = 12;
    mui_canvas_set_draw_color(p_canvas, 2);
    mui_canvas_draw_box(p_canvas, 0, focus_yi, mui_canvas_get_width(p_canvas), focus_h);
    mui_canvas_set_draw_color(p_canvas, 1);

    mui_element_scrollbar(p_canvas, p_mui_list_view->focus_index, mui_list_item_array_size(p_mui_list_view->items));

    if (!p_mui_list_view->first_draw) {
        p_mui_list_view->first_draw = 1;
        mui_list_view_start_text_anim(p_mui_list_view);
    }
}

static void mui_list_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    mui_list_view_t *p_mui_list_view = p_view->user_data;
    if (event->type == INPUT_TYPE_SHORT || event->type == INPUT_TYPE_REPEAT || event->type == INPUT_TYPE_LONG) {
        switch (event->key) {
        case INPUT_KEY_LEFT:

            if (p_mui_list_view->focus_index > 0) {
                p_mui_list_view->focus_index--;
                uint16_t focus_offset = p_mui_list_view->focus_index * LIST_ITEM_HEIGHT;
                if (focus_offset < p_mui_list_view->scroll_offset) { // scroll up
                    p_mui_list_view->scroll_offset -= LIST_ITEM_HEIGHT;
                    if (mui_list_view_anim_enabled()) {
                        p_mui_list_view->anim_type = LIST_ANIM_SCROLL;
                        p_mui_list_view->anim_value = LIST_ITEM_HEIGHT;
                        mui_anim_set_time(&p_mui_list_view->anim, LIST_ANIM_SHORT_TIME);
                        mui_anim_set_values(&p_mui_list_view->anim, LIST_ITEM_HEIGHT, 0);
                        mui_anim_start(&p_mui_list_view->anim);
                    }
                } else {
                    if (mui_list_view_anim_enabled()) {
                        p_mui_list_view->anim_value = LIST_ITEM_HEIGHT;
                        p_mui_list_view->anim_type = LIST_ANIM_FOCUS;
                        mui_anim_set_time(&p_mui_list_view->anim, LIST_ANIM_SHORT_TIME);
                        mui_anim_set_values(&p_mui_list_view->anim, LIST_ITEM_HEIGHT, 0);
                        mui_anim_start(&p_mui_list_view->anim);
                    }
                }
            } else {
                p_mui_list_view->focus_index = mui_list_item_array_size(p_mui_list_view->items) - 1;
                uint16_t focus_offset = p_mui_list_view->focus_index * LIST_ITEM_HEIGHT;
                uint16_t max_item_num = p_mui_list_view->canvas_height / LIST_ITEM_HEIGHT;
                if (focus_offset >
                    p_mui_list_view->scroll_offset + p_mui_list_view->canvas_height) { // scroll to bottom
                    uint32_t cur_scroll_offset = p_mui_list_view->scroll_offset;
                    p_mui_list_view->scroll_offset = (p_mui_list_view->focus_index - max_item_num) * LIST_ITEM_HEIGHT;
                    uint32_t diff_scroll_offset = p_mui_list_view->scroll_offset - cur_scroll_offset;
                    if (mui_list_view_anim_enabled()) {
                        p_mui_list_view->anim_type = LIST_ANIM_SCROLL;
                        mui_anim_set_time(&p_mui_list_view->anim, LIST_ANIM_LONG_TIME);
                        mui_anim_set_values(&p_mui_list_view->anim, -diff_scroll_offset, 0);
                        mui_anim_start(&p_mui_list_view->anim);
                    }
                } else {
                    if (mui_list_view_anim_enabled()) {
                        p_mui_list_view->anim_value = -p_mui_list_view->focus_index * LIST_ITEM_HEIGHT;
                        p_mui_list_view->anim_type = LIST_ANIM_FOCUS;
                        mui_anim_set_time(&p_mui_list_view->anim, LIST_ANIM_LONG_TIME);
                        mui_anim_set_values(&p_mui_list_view->anim, p_mui_list_view->anim_value, 0);
                        mui_anim_start(&p_mui_list_view->anim);
                    }
                }
            }

            mui_list_view_start_text_anim(p_mui_list_view);

            break;

        case INPUT_KEY_RIGHT:
            if (p_mui_list_view->focus_index < mui_list_item_array_size(p_mui_list_view->items) - 1) {
                p_mui_list_view->focus_index++;
                uint16_t focus_offset = p_mui_list_view->focus_index * LIST_ITEM_HEIGHT;
                if (focus_offset > p_mui_list_view->scroll_offset + p_mui_list_view->canvas_height) { // scroll down
                    p_mui_list_view->scroll_offset += LIST_ITEM_HEIGHT;

                    if (mui_list_view_anim_enabled()) {
                        p_mui_list_view->anim_type = LIST_ANIM_SCROLL;
                        p_mui_list_view->anim_value = -LIST_ITEM_HEIGHT;
                        mui_anim_set_time(&p_mui_list_view->anim, LIST_ANIM_SHORT_TIME);
                        mui_anim_set_values(&p_mui_list_view->anim, -LIST_ITEM_HEIGHT, 0);
                        mui_anim_start(&p_mui_list_view->anim);
                    }
                    // todo scroll anim
                } else {
                    if (mui_list_view_anim_enabled()) {
                        p_mui_list_view->anim_value = -LIST_ITEM_HEIGHT;
                        p_mui_list_view->anim_type = LIST_ANIM_FOCUS;
                        mui_anim_set_time(&p_mui_list_view->anim, LIST_ANIM_SHORT_TIME);
                        mui_anim_set_values(&p_mui_list_view->anim, -LIST_ITEM_HEIGHT, 0);
                        mui_anim_start(&p_mui_list_view->anim);
                    }
                }
            } else {
                // scroll to first
                uint16_t cur_focus_index = p_mui_list_view->focus_index;
                p_mui_list_view->focus_index = 0;
                if (p_mui_list_view->scroll_offset > 0) {
                    uint32_t cur_scroll_offset = p_mui_list_view->scroll_offset;
                    p_mui_list_view->scroll_offset = 0;

                    if (mui_list_view_anim_enabled()) {
                        p_mui_list_view->anim_value = cur_scroll_offset;
                        p_mui_list_view->anim_type = LIST_ANIM_SCROLL;
                        mui_anim_set_time(&p_mui_list_view->anim, LIST_ANIM_LONG_TIME);
                        mui_anim_set_values(&p_mui_list_view->anim, cur_scroll_offset, 0);
                        mui_anim_start(&p_mui_list_view->anim);
                    }
                } else {
                    if (mui_list_view_anim_enabled()) {
                        p_mui_list_view->anim_value = cur_focus_index * LIST_ITEM_HEIGHT;
                        p_mui_list_view->anim_type = LIST_ANIM_FOCUS;
                        mui_anim_set_time(&p_mui_list_view->anim, LIST_ANIM_LONG_TIME);
                        mui_anim_set_values(&p_mui_list_view->anim, p_mui_list_view->anim_value, 0);
                        mui_anim_start(&p_mui_list_view->anim);
                    }
                }
            }
            mui_list_view_start_text_anim(p_mui_list_view);
            break;

        case INPUT_KEY_CENTER:
            if (p_mui_list_view->selected_cb) {
                if (event->type == INPUT_TYPE_SHORT) {
                    p_mui_list_view->selected_cb(
                        MUI_LIST_VIEW_EVENT_SELECTED, p_mui_list_view,
                        mui_list_item_array_get(p_mui_list_view->items, p_mui_list_view->focus_index));
                } else if (event->type == INPUT_TYPE_LONG) {
                    p_mui_list_view->selected_cb(
                        MUI_LIST_VIEW_EVENT_LONG_SELECTED, p_mui_list_view,
                        mui_list_item_array_get(p_mui_list_view->items, p_mui_list_view->focus_index));
                }
            }
            break;
        }
    }
}

static void mui_list_view_on_enter(mui_view_t *p_view) {
    mui_list_view_t *p_mui_list_view = p_view->user_data;
    p_mui_list_view->first_draw = 0;
}

static void mui_list_view_on_exit(mui_view_t *p_view) {}

mui_list_view_t *mui_list_view_create() {
    mui_list_view_t *p_mui_list_view = mui_mem_malloc(sizeof(mui_list_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_mui_list_view;
    p_view->draw_cb = mui_list_view_on_draw;
    p_view->input_cb = mui_list_view_on_input;
    p_view->enter_cb = mui_list_view_on_enter;
    p_view->exit_cb = mui_list_view_on_exit;

    p_mui_list_view->p_view = p_view;
    p_mui_list_view->focus_index = 0;
    p_mui_list_view->scroll_offset = 0;
    p_mui_list_view->selected_cb = NULL;
    p_mui_list_view->anim_value = 0;
    p_mui_list_view->anim_type = LIST_ANIM_FOCUS;

    mui_anim_init(&p_mui_list_view->anim);
    mui_anim_set_var(&p_mui_list_view->anim, p_mui_list_view);
    mui_anim_set_exec_cb(&p_mui_list_view->anim, mui_list_view_anim_exec);
    mui_anim_set_values(&p_mui_list_view->anim, 0, LIST_ITEM_HEIGHT);
    mui_anim_set_time(&p_mui_list_view->anim, 200);

    p_mui_list_view->text_offset = 0;

    mui_anim_init(&p_mui_list_view->text_anim);
    mui_anim_set_var(&p_mui_list_view->text_anim, p_mui_list_view);
    mui_anim_set_path_cb(&p_mui_list_view->text_anim, lv_anim_path_linear);
    mui_anim_set_exec_cb(&p_mui_list_view->text_anim, mui_list_view_text_anim_exec);
    mui_anim_set_time(&p_mui_list_view->text_anim, 200);

    mui_list_item_array_init(p_mui_list_view->items);

    return p_mui_list_view;
}

void mui_list_view_free(mui_list_view_t *p_view) {
    mui_list_item_array_it_t it;

    mui_anim_stop(&p_view->anim);
    mui_anim_stop(&p_view->text_anim);

    mui_list_item_array_it(it, p_view->items);
    while (!mui_list_item_array_end_p(it)) {
        mui_list_item_t *item = mui_list_item_array_ref(it);
        string_clear(item->text);
        mui_list_item_array_next(it);
    }
    mui_list_item_array_clear(p_view->items);
    mui_mem_free(p_view->p_view);
    mui_mem_free(p_view);
}

mui_view_t *mui_list_view_get_view(mui_list_view_t *p_view) { return p_view->p_view; }

//// view functions //
void mui_list_view_add_item(mui_list_view_t *p_view, uint32_t icon, const char *text, void *user_data) {
    mui_list_item_t *p_item = mui_list_item_array_push_new(p_view->items);
    p_item->icon = icon;
    p_item->user_data = user_data;
    string_init(p_item->text);
    string_set_str(p_item->text, text);
}

void mui_list_view_set_item(mui_list_view_t *p_view, uint16_t index, uint32_t icon, char *text, void *user_data) {
    mui_list_item_t *p_item = mui_list_item_array_get(p_view->items, index);
    if (p_item != NULL) {
        p_item->icon = icon;
        p_item->user_data = user_data;
        string_set_str(p_item->text, text);
    }
}

uint32_t mui_list_view_item_size(mui_list_view_t *p_view) { return mui_list_item_array_size(p_view->items); }

void mui_list_view_clear_items(mui_list_view_t *p_view) {
    mui_list_item_array_it_t it;

    mui_list_item_array_it(it, p_view->items);
    while (!mui_list_item_array_end_p(it)) {
        mui_list_item_t *item = mui_list_item_array_ref(it);
        string_clear(item->text);
        mui_list_item_array_next(it);
    }
    mui_list_item_array_reset(p_view->items);
    mui_list_view_set_focus(p_view, 0);
}

void mui_list_view_clear_items_with_cb(mui_list_view_t *p_view, mui_list_view_item_clear_cb clear_cb) {
    mui_list_item_array_it_t it;

    mui_list_item_array_it(it, p_view->items);
    while (!mui_list_item_array_end_p(it)) {
        mui_list_item_t *item = mui_list_item_array_ref(it);
        string_clear(item->text);
        clear_cb(item);
        mui_list_item_array_next(it);
    }
    mui_list_item_array_reset(p_view->items);
    mui_list_view_set_focus(p_view, 0);
}

void mui_list_view_set_selected_cb(mui_list_view_t *p_view, mui_list_view_selected_cb selected_cb) {
    p_view->selected_cb = selected_cb;
}

void mui_list_view_set_user_data(mui_list_view_t *p_view, void *user_data) { p_view->user_data = user_data; }

void mui_list_view_set_focus(mui_list_view_t *p_view, uint16_t focus_index) {
    if (focus_index >= 0 && focus_index < mui_list_item_array_size(p_view->items)) {
        p_view->focus_index = focus_index;
        uint32_t offset_y = p_view->focus_index * LIST_ITEM_HEIGHT;
        p_view->scroll_offset = offset_y;
    } else {
        p_view->focus_index = 0;
        p_view->scroll_offset = 0;
    }
}

uint16_t mui_list_view_get_focus(mui_list_view_t *p_view) { return p_view->focus_index; }

void mui_list_view_sort(mui_list_view_t *p_view, mui_list_view_item_cmp_cb cmp_cb) {
    mui_list_item_array_special_sort(p_view->items, cmp_cb);
}
