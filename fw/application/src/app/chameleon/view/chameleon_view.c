#include "chameleon_view.h"
#include "amiibo_helper.h"
#include "db_header.h"
#include "i18n/language.h"
#include "mui_element.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"
#include "ntag_def.h"
#include "ntag_emu.h"

#define ICON_RANDOM 0xe20d
#define ICON_NTAG 0xe1cf
#define ICON_LEFT 0xe1ac
#define ICON_RIGHT 0xe1aa

static void chameleon_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {

    chameleon_view_t *p_chameleon_view = p_view->user_data;
    ntag_t *ntag = ntag_emu_get_current_tag();
    char buff[64];

    mui_canvas_set_font(p_canvas, MUI_FONT_NORMAL);

    uint8_t x = 0, y = 0;
    mui_canvas_draw_box(p_canvas, 0, y, mui_canvas_get_width(p_canvas), 12);
    mui_canvas_set_draw_color(p_canvas, 0);

    sprintf(buff, "[%02d] %02x:%02x:%02x:%02x:%02x:%02x:%02x", 0, 11, 22, 33, 0xAA, 0xBB, 0xCC, 0xDD);
    mui_canvas_draw_utf8(p_canvas, 0, y + 10, buff);

    mui_canvas_set_draw_color(p_canvas, 1);
    const char *name = "我的卡1";
    y = 13 + (mui_canvas_get_height(p_canvas) - 20) / 2;
    x = (mui_canvas_get_width(p_canvas) - mui_canvas_get_utf8_width(p_canvas, name)) / 2;
    mui_canvas_draw_utf8(p_canvas, x, y, name);
    mui_canvas_draw_utf8(p_canvas, 0, y, "<");
    mui_canvas_draw_utf8(p_canvas, mui_canvas_get_width(p_canvas) - 8, y, ">");

    mui_canvas_set_font(p_canvas, MUI_FONT_SMALL);
    mui_canvas_draw_utf8(p_canvas, 0, mui_canvas_get_height(p_canvas), "N215 <00/44 00> G2A");

    mui_canvas_set_font(p_canvas, MUI_FONT_ICON);
    mui_canvas_draw_glyph(p_canvas, mui_canvas_get_width(p_canvas) - 10, mui_canvas_get_height(p_canvas),  ICON_NTAG);

    mui_canvas_set_font(p_canvas, MUI_FONT_NORMAL);
}

static void chameleon_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    chameleon_view_t *p_chameleon_view = p_view->user_data;
    switch (event->type) {
    case INPUT_TYPE_LONG: {
        if (p_chameleon_view->event_cb) {
            p_chameleon_view->event_cb(CHAMELEON_VIEW_EVENT_MENU, p_chameleon_view);
        }
        break;
    }
    case INPUT_TYPE_SHORT: {

        if (event->key == INPUT_KEY_CENTER) {
            if (p_chameleon_view->event_cb) {
                p_chameleon_view->event_cb(CHAMELEON_VIEW_EVENT_MENU, p_chameleon_view);
            }
            return;
        }
    }
    }
}

static void chameleon_view_on_enter(mui_view_t *p_view) {}

static void chameleon_view_on_exit(mui_view_t *p_view) {}

chameleon_view_t *chameleon_view_create() {
    chameleon_view_t *p_chameleon_view = mui_mem_malloc(sizeof(chameleon_view_t));
    memset(p_chameleon_view, 0, sizeof(chameleon_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_chameleon_view;
    p_view->draw_cb = chameleon_view_on_draw;
    p_view->input_cb = chameleon_view_on_input;
    p_view->enter_cb = chameleon_view_on_enter;
    p_view->exit_cb = chameleon_view_on_exit;

    p_chameleon_view->p_view = p_view;
    p_chameleon_view->index = 0;

    return p_chameleon_view;
}
void chameleon_view_free(chameleon_view_t *p_view) {
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}
mui_view_t *chameleon_view_get_view(chameleon_view_t *p_view) { return p_view->p_view; }
