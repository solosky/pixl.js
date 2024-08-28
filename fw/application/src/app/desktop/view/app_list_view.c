#include "app_list_view.h"
#include "i18n/language.h"
#include "nrf_log.h"
#include "settings2.h"

#define ICON_WIDTH 32
#define ICON_HEIGHT 32
#define ICON_MARGIN 3
#define ICON_TOP_MARGIN 3
#define ICON_GROUP_WIDTH (ICON_WIDTH + ICON_MARGIN)
#define FONT_HEIGHT 12

#define ICON_ANIM_LONG_TIME 500
#define ICON_ANIM_SHORT_TIME 200

static bool mui_list_view_anim_enabled() { return settings_get_data()->anim_enabled; }
static void app_list_view_icon_anim_exec(void *p, int32_t value) {
    app_list_view_t *p_view = (app_list_view_t *)p;
    p_view->icon_anim_value = value;
}

static void app_list_view_name_anim_exec(void *p, int32_t value) {
    app_list_view_t *p_view = (app_list_view_t *)p;
    p_view->name_anim_value = value;
}

static void app_list_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    app_list_view_t *p_list_view = p_view->user_data;

    p_list_view->canvas_height = p_canvas->height;
    p_list_view->canvas_width = p_canvas->width;

    mini_app_t *focus_app = *ptr_array_get(p_list_view->items, p_list_view->focus);

    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    uint8_t font_height = mui_canvas_current_font_height(p_canvas);
    uint8_t canvas_height = mui_canvas_get_height(p_canvas);
    uint8_t canvas_width = mui_canvas_get_width(p_canvas);
    uint8_t name_width = mui_canvas_get_utf8_width(p_canvas, getLangString(focus_app->name_i18n_key));
    uint8_t focus = p_list_view->focus;

    uint8_t first_offset = (canvas_width - ICON_WIDTH) / 2;
    int16_t scroll_offset = focus * ICON_GROUP_WIDTH + p_list_view->icon_anim_value;

    // icon
    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    mui_canvas_set_draw_color(p_canvas, 1);

    for (uint8_t i = 0; i < ptr_array_size(p_list_view->items); i++) {
        mini_app_t *app = *ptr_array_get(p_list_view->items, i);
        int16_t icon_x = i * ICON_GROUP_WIDTH - scroll_offset + first_offset;
        if (icon_x + ICON_GROUP_WIDTH > 0 && icon_x < canvas_width) {
            if (app->icon_32x32) {
                mui_canvas_draw_xbm(p_canvas, icon_x, ICON_TOP_MARGIN, ICON_WIDTH, ICON_HEIGHT, app->icon_32x32->data);
            }
        }
    }

    // focus
    mui_canvas_set_draw_color(p_canvas, 2);
    mui_canvas_draw_rbox(p_canvas, first_offset, ICON_TOP_MARGIN, 32, 32, 1);
    mui_canvas_set_draw_color(p_canvas, 1);

    // left selection
    // uint8_t fx = first_offset - 3;
    // mui_canvas_draw_line(p_canvas, fx + 1, ICON_TOP_MARGIN - 2, fx + 3, ICON_TOP_MARGIN - 2);
    // mui_canvas_draw_line(p_canvas, fx, ICON_TOP_MARGIN - 1, fx, ICON_TOP_MARGIN + ICON_HEIGHT);
    // mui_canvas_draw_line(p_canvas, fx + 1, ICON_TOP_MARGIN + ICON_HEIGHT + 1, fx + 3,
    //                      ICON_TOP_MARGIN + ICON_HEIGHT + 1);

    // // right selection
    // fx = first_offset + ICON_WIDTH + 2;
    // mui_canvas_draw_line(p_canvas, fx - 3, ICON_TOP_MARGIN - 2, fx - 1, ICON_TOP_MARGIN - 2);
    // mui_canvas_draw_line(p_canvas, fx, ICON_TOP_MARGIN - 1, fx, ICON_TOP_MARGIN + ICON_HEIGHT);
    // mui_canvas_draw_line(p_canvas, fx - 3, ICON_TOP_MARGIN + ICON_HEIGHT + 1, fx - 1,
    //                      ICON_TOP_MARGIN + ICON_HEIGHT + 1);

    // dash line
    mui_canvas_set_draw_color(p_canvas, 1);
    for (uint8_t i = 0; i < canvas_width; i += 6) {
        mui_canvas_draw_line(p_canvas, i, canvas_height - font_height, i + 3, canvas_height - font_height);
    }

    // app name
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_canvas_draw_utf8(p_canvas, (canvas_width - name_width) / 2, canvas_height - 1 + p_list_view->name_anim_value,
                         getLangString(focus_app->name_i18n_key));

    // left and right icon
    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    mui_canvas_draw_glyph(p_canvas, 0, canvas_height - 2, 0xe10b);
    mui_canvas_draw_glyph(p_canvas, canvas_width - 12, canvas_height - 2, 0xe10a);
}

static void app_list_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    app_list_view_t *p_app_list_view = p_view->user_data;
    uint8_t size = ptr_array_size(p_app_list_view->items);
    if (event->type == INPUT_TYPE_SHORT) {
        switch (event->key) {
        case INPUT_KEY_RIGHT: {
            if (p_app_list_view->focus + 1 < size) {
                p_app_list_view->focus++;
                mui_anim_set_values(&p_app_list_view->icon_anim, -ICON_GROUP_WIDTH, 0);
                mui_anim_set_time(&p_app_list_view->icon_anim, ICON_ANIM_SHORT_TIME);
            } else {
                p_app_list_view->focus = 0;
                int32_t scroll_offset = 1 * size * ICON_GROUP_WIDTH;
                mui_anim_set_values(&p_app_list_view->icon_anim, scroll_offset, 0);
                mui_anim_set_time(&p_app_list_view->icon_anim, ICON_ANIM_LONG_TIME);
            }
            if (mui_list_view_anim_enabled()) {
                mui_anim_start(&p_app_list_view->icon_anim);
                mui_anim_set_values(&p_app_list_view->name_anim, FONT_HEIGHT, 0);
                mui_anim_start(&p_app_list_view->name_anim);
            }
        } break;

        case INPUT_KEY_LEFT: {
            if (p_app_list_view->focus - 1 >= 0) {
                p_app_list_view->focus--;
                mui_anim_set_values(&p_app_list_view->icon_anim, ICON_GROUP_WIDTH, 0);
                mui_anim_set_time(&p_app_list_view->icon_anim, ICON_ANIM_SHORT_TIME);

            } else {
                p_app_list_view->focus = size - 1;
                int32_t scroll_offset = -1 * size * ICON_GROUP_WIDTH;
                mui_anim_set_values(&p_app_list_view->icon_anim, scroll_offset, 0);
                mui_anim_set_time(&p_app_list_view->icon_anim, ICON_ANIM_LONG_TIME);
            }
            if (mui_list_view_anim_enabled()) {
                mui_anim_set_values(&p_app_list_view->name_anim, FONT_HEIGHT, 0);
                mui_anim_start(&p_app_list_view->icon_anim);
                mui_anim_start(&p_app_list_view->name_anim);
            }
        }

        break;
        case INPUT_KEY_CENTER:
            if (p_app_list_view->event_cb) {
                p_app_list_view->event_cb(APP_LIST_ITEM_SELECTED, p_app_list_view);
            }
            break;
        }
    }
}

static void app_list_view_on_enter(mui_view_t *p_view) {
    app_list_view_t *p_app_list_view = p_view->user_data;
    if (mui_list_view_anim_enabled()) {
        mui_anim_set_values(&p_app_list_view->icon_anim, -ICON_GROUP_WIDTH, 0);
        mui_anim_start(&p_app_list_view->icon_anim);
        mui_anim_set_values(&p_app_list_view->name_anim, -FONT_HEIGHT, 0);
        mui_anim_start(&p_app_list_view->name_anim);
    }
}

static void app_list_view_on_exit(mui_view_t *p_view) {
    app_list_view_t *p_app_list_view = p_view->user_data;
    if (mui_list_view_anim_enabled()) {
        mui_anim_stop(&p_app_list_view->icon_anim);
        mui_anim_stop(&p_app_list_view->name_anim);
    }
}

app_list_view_t *app_list_view_create() {
    app_list_view_t *p_app_list_view = mui_mem_malloc(sizeof(app_list_view_t));

    ptr_array_init(p_app_list_view->items);
    p_app_list_view->focus = 0;
    p_app_list_view->icon_anim_value = 0;
    p_app_list_view->name_anim_value = 0;

    mui_anim_init(&p_app_list_view->icon_anim);
    mui_anim_set_var(&p_app_list_view->icon_anim, p_app_list_view);
    mui_anim_set_path_cb(&p_app_list_view->icon_anim, lv_anim_path_ease_in_out);
    mui_anim_set_values(&p_app_list_view->icon_anim, 0, ICON_GROUP_WIDTH);
    mui_anim_set_exec_cb(&p_app_list_view->icon_anim, app_list_view_icon_anim_exec);
    mui_anim_set_time(&p_app_list_view->icon_anim, ICON_ANIM_SHORT_TIME);

    mui_anim_init(&p_app_list_view->name_anim);
    mui_anim_set_var(&p_app_list_view->name_anim, p_app_list_view);
    mui_anim_set_path_cb(&p_app_list_view->name_anim, lv_anim_path_ease_in_out);
    mui_anim_set_values(&p_app_list_view->name_anim, FONT_HEIGHT, 0);
    mui_anim_set_exec_cb(&p_app_list_view->name_anim, app_list_view_name_anim_exec);
    mui_anim_set_time(&p_app_list_view->name_anim, ICON_ANIM_SHORT_TIME);

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_app_list_view;
    p_view->draw_cb = app_list_view_on_draw;
    p_view->input_cb = app_list_view_on_input;
    p_view->enter_cb = app_list_view_on_enter;
    p_view->exit_cb = app_list_view_on_exit;

    p_app_list_view->p_view = p_view;

    return p_app_list_view;
}
void app_list_view_free(app_list_view_t *p_view) {
    ptr_array_clear(p_view->items);
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}
mui_view_t *app_list_view_get_view(app_list_view_t *p_view) { return p_view->p_view; }