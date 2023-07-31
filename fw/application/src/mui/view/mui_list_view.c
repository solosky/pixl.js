#include "mui_list_view.h"
#include "mui_element.h"

#include "mui_mem.h"

#define LIST_ITEM_HEIGHT 13

static void mui_list_view_anim_exec(void* p, int32_t value){
    mui_list_view_t *p_view = (mui_list_view_t *)p;
    p_view->anim_value = value;
}

static void mui_list_view_update_scroll_offset_y(mui_list_view_t *p_view, mui_canvas_t *p_canvas) {

    uint32_t index = p_view->focus_index;
    uint32_t total = mui_list_item_array_size(p_view->items);
    uint8_t scroll_direction = p_view->scroll_direction;
    uint32_t item_per_canvas = mui_canvas_get_height(p_canvas) / (LIST_ITEM_HEIGHT - 1);
    if (mui_list_item_array_size(p_view->items) <= item_per_canvas) {
        p_view->scroll_offset = 0;
        return;
    }

    if (scroll_direction == LIST_SCROLL_TOP) {
        p_view->scroll_offset = 0;
    } else if (scroll_direction == LIST_SCROLL_BOTTOM) {
        if (index >= item_per_canvas) {
            p_view->scroll_offset = (total - item_per_canvas) * LIST_ITEM_HEIGHT;
        } else {
            p_view->scroll_offset = 0;
        }
    } else {
        uint32_t offset_y = p_view->focus_index * LIST_ITEM_HEIGHT;
        if (offset_y < p_view->scroll_offset || offset_y > p_view->scroll_offset + mui_canvas_get_height(p_canvas)) {
            if (p_view->scroll_direction == LIST_SCROLL_UP) {
                p_view->scroll_offset = offset_y;
            } else {
                p_view->scroll_offset += LIST_ITEM_HEIGHT;
            }
        }
    }
}

static void mui_list_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_list_view_t *p_mui_list_view = p_view->user_data;

    mui_list_item_array_it_t it;

    mui_list_item_array_it(it, p_mui_list_view->items);
    mui_list_view_update_scroll_offset_y(p_mui_list_view, p_canvas);

    uint32_t offset_y = p_mui_list_view->scroll_offset;
    uint32_t index = 0;
    while (!mui_list_item_array_end_p(it)) {
        mui_list_item_t *item = mui_list_item_array_ref(it);
        uint32_t y = index * LIST_ITEM_HEIGHT - offset_y;

        if (y >= 0 && y <= mui_canvas_get_height(p_canvas)) {
            mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
            mui_canvas_draw_glyph(p_canvas, 0, y + 10, item->icon);
            mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
            mui_canvas_draw_utf8(p_canvas, 13, y + 10, string_get_cstr(item->text));
            if (index == p_mui_list_view->focus_index) {
                mui_canvas_set_draw_color(p_canvas, 2);
                mui_canvas_draw_box(p_canvas, 0, y - LIST_ITEM_HEIGHT + p_mui_list_view->anim_value, mui_canvas_get_width(p_canvas), 12);
                mui_canvas_set_draw_color(p_canvas, 1);
            }
        }

        mui_list_item_array_next(it);
        index++;
    }

    mui_element_scrollbar(p_canvas, p_mui_list_view->focus_index, mui_list_item_array_size(p_mui_list_view->items));
}

static void mui_list_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    mui_list_view_t *p_mui_list_view = p_view->user_data;
    if (event->type == INPUT_TYPE_SHORT || event->type == INPUT_TYPE_REPEAT || INPUT_TYPE_LONG) {
        switch (event->key) {
        case INPUT_KEY_LEFT:
            if (p_mui_list_view->focus_index > 0) {
                p_mui_list_view->focus_index--;
                p_mui_list_view->scroll_direction = LIST_SCROLL_UP;
            } else {
                p_mui_list_view->focus_index = mui_list_item_array_size(p_mui_list_view->items) - 1;
                p_mui_list_view->scroll_direction = LIST_SCROLL_BOTTOM;
            }
            p_mui_list_view->anim_value = LIST_ITEM_HEIGHT;
            mui_anim_set_values(&p_mui_list_view->anim, LIST_ITEM_HEIGHT, 0);
            mui_anim_start(&p_mui_list_view->anim);

            break;

        case INPUT_KEY_RIGHT:
            if (p_mui_list_view->focus_index < mui_list_item_array_size(p_mui_list_view->items) - 1) {
                p_mui_list_view->focus_index++;
                p_mui_list_view->scroll_direction = LIST_SCROLL_DOWN;
            } else {
                p_mui_list_view->focus_index = 0;
                p_mui_list_view->scroll_direction = LIST_SCROLL_TOP;
            }
            p_mui_list_view->anim_value = 0;
            mui_anim_set_values(&p_mui_list_view->anim, 0, LIST_ITEM_HEIGHT);
            mui_anim_start(&p_mui_list_view->anim);
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

static void mui_list_view_on_enter(mui_view_t *p_view) {}

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
    p_mui_list_view->scroll_direction = LIST_SCROLL_DOWN;
    p_mui_list_view->selected_cb = NULL;
    p_mui_list_view->anim_value = LIST_ITEM_HEIGHT;

    mui_anim_init(&p_mui_list_view->anim);
    mui_anim_set_var(&p_mui_list_view->anim, p_mui_list_view);
    mui_anim_set_exec_cb(&p_mui_list_view->anim, mui_list_view_anim_exec);
    mui_anim_set_values(&p_mui_list_view->anim, 0, LIST_ITEM_HEIGHT);
    mui_anim_set_time(&p_mui_list_view->anim, 200);

    mui_list_item_array_init(p_mui_list_view->items);

    return p_mui_list_view;
}

void mui_list_view_free(mui_list_view_t *p_view) {
    mui_list_item_array_it_t it;

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
    // TODO
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
