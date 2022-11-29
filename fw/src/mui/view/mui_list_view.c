#include "mui_list_view.h"

#define LIST_ITEM_HEIGHT 12

static void mui_list_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_list_view_t *p_mui_list_view = p_view->user_data;

    mui_list_item_array_it_t it;

    mui_list_item_array_it(it, p_mui_list_view->items);
    uint32_t index = 0, y = 0;
    while (!mui_list_item_array_end_p(it)) {
        mui_list_item_t *item = mui_list_item_array_ref(it);



        if (index == p_mui_list_view->focus_index) {
            mui_canvas_draw_box(p_canvas, 0, y, mui_canvas_get_width(p_canvas), 12);
            mui_canvas_set_draw_color(p_canvas, 0);
            mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
            mui_canvas_draw_glyph(p_canvas, 2, y + 10, item->icon);
            mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
            mui_canvas_draw_utf8(p_canvas, 16, y + 10, string_get_cstr(item->text));
            mui_canvas_set_draw_color(p_canvas, 1);
        } else {
            mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
            mui_canvas_draw_glyph(p_canvas, 2, y + 10, item->icon);
            mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
            mui_canvas_draw_utf8(p_canvas, 16, y + 10, string_get_cstr(item->text));
        }

        mui_list_item_array_next(it);
        index++;
        y += 12;
    }
}

static void mui_list_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    mui_list_view_t *p_mui_list_view = p_view->user_data;
    switch (event->key) {
    case INPUT_KEY_LEFT:
        if (p_mui_list_view->focus_index > 0) {
            p_mui_list_view->focus_index--;
        }

        break;
    case INPUT_KEY_RIGHT:
        if (p_mui_list_view->focus_index <
            mui_list_item_array_size(p_mui_list_view->items) - 1) {
            p_mui_list_view->focus_index++;
        }

        break;
    case INPUT_KEY_CENTER:

        break;
    }
}

static void mui_list_view_on_enter(mui_view_t *p_view) {}

static void mui_list_view_on_exit(mui_view_t *p_view) {}

mui_list_view_t *mui_list_view_create() {
    mui_list_view_t *p_mui_list_view = malloc(sizeof(mui_list_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_mui_list_view;
    p_view->draw_cb = mui_list_view_on_draw;
    p_view->input_cb = mui_list_view_on_input;
    p_view->enter_cb = mui_list_view_on_enter;
    p_view->exit_cb = mui_list_view_on_exit;

    p_mui_list_view->p_view = p_view;
    p_mui_list_view->focus_index = 0;

    mui_list_item_array_init(p_mui_list_view->items);

    return p_mui_list_view;
}
void mui_list_view_free(mui_list_view_t *p_view) {
    mui_list_item_array_clear(p_view->items);
    free(p_view->p_view);
    free(p_view);
}
mui_view_t *mui_list_view_get_view(mui_list_view_t *p_view) { return p_view->p_view; }

void mui_list_view_set_user_data(mui_list_view_t *p_view, void *user_data) {
    p_view->user_data = user_data;
}

//// view functions //
void mui_list_view_add_item(mui_list_view_t *p_view, uint32_t icon, char *text,
                            void *user_data) {
    mui_list_item_t *p_item = mui_list_item_array_push_new(p_view->items);
    p_item->icon = icon;
    p_item->user_data = user_data;
    string_init(p_item->text);
    string_set_str(p_item->text, text);
}
void mui_list_view_set_selected_cb(mui_list_view_t *p_view,
                                   mui_list_view_selected_cb selected_cb) {
    p_view->selcted_cb = selected_cb;
}