#include "amiibo_view.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"

static void amiibo_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    amiibo_view_t *p_amiibo_view = p_view->user_data;
    for (uint32_t i = 0, y = 0; i < 6; i++, y += 12) {
        if (i == p_amiibo_view->focus_index) {
            mui_canvas_draw_box(p_canvas, 0, y, mui_canvas_get_width(p_canvas), 12);
            mui_canvas_set_draw_color(p_canvas, 0);
            mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
            mui_canvas_draw_glyph(p_canvas, 2, y + 10, 0xe1d6);
            mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
            mui_canvas_draw_utf8(p_canvas, 16, y + 10, p_amiibo_view->amiibo_name[i]);
            mui_canvas_set_draw_color(p_canvas, 1);
        } else {
            mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
            mui_canvas_draw_glyph(p_canvas, 2, y + 10, 0xe1ed);
            mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
            mui_canvas_draw_utf8(p_canvas, 16, y + 10,  p_amiibo_view->amiibo_name[i]);
        }
    }
}

static void amiibo_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    amiibo_view_t *p_amiibo_view = p_view->user_data;
    switch (event->key) {
    case INPUT_KEY_LEFT:
        if (p_amiibo_view->focus_index > 0) {
            p_amiibo_view->focus_index--;
        }

        break;
    case INPUT_KEY_RIGHT:
        if (p_amiibo_view->focus_index < 5) {
            p_amiibo_view->focus_index++;
        }

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

    strcpy(p_amiibo_view->amiibo_name[0], "Link - Ocarina of Time");
    strcpy(p_amiibo_view->amiibo_name[1], "Link - Link's Awakening");
    strcpy(p_amiibo_view->amiibo_name[2], "Toon Link - The Wind Waker");
    strcpy(p_amiibo_view->amiibo_name[3], "Midna & Wolf Link");
    strcpy(p_amiibo_view->amiibo_name[4], "Zelda & Loftwing");
    strcpy(p_amiibo_view->amiibo_name[5], "Guardian");

    return p_amiibo_view;
}
void amiibo_view_free(amiibo_view_t *p_view) {
    free(p_view->p_view);
    free(p_view);
}
mui_view_t *amiibo_view_get_view(amiibo_view_t *p_view) { return p_view->p_view; }