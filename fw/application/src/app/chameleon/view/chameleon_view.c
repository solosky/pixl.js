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

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);

    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);

    uint8_t y = 0;
    mui_canvas_draw_box(p_canvas, 0, y, mui_canvas_get_width(p_canvas), 12);
    mui_canvas_set_draw_color(p_canvas, 0);


    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    sprintf(buff, "%02x:%02x:%02x:%02x:%02x:%02x:%02x", ntag->data[0], ntag->data[1], ntag->data[2], ntag->data[4],
            ntag->data[5], ntag->data[6], ntag->data[7]);
    mui_canvas_draw_utf8(p_canvas, 25, y + 10, buff);

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    mui_canvas_set_draw_color(p_canvas, 1);
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);

    y += 12;

    mui_canvas_draw_rframe(p_canvas, 0, y + 3, mui_canvas_get_width(p_canvas), mui_canvas_get_height(p_canvas) - y - 3,
                           3);

    uint32_t head = to_little_endian_int32(&ntag->data[84]);
    uint32_t tail = to_little_endian_int32(&ntag->data[88]);

    const db_amiibo_t *amd = get_amiibo_by_id(head, tail);
    if (amd != NULL) {
        mui_canvas_draw_utf8(p_canvas, 5, y += 15, amd->name_cn);
        if (strlen(ntag->notes) > 0) {
            mui_element_autowrap_text(p_canvas, 5, y += 15, mui_canvas_get_width(p_canvas), 24, ntag->notes);
        }
    } else {
        if (head > 0 && tail > 0) {
            mui_canvas_draw_utf8(p_canvas, 5, y += 15, "Amiibo");
            sprintf(buff, "[%08x:%08x]", head, tail);
            mui_canvas_draw_utf8(p_canvas, 5, y += 15, buff);
        } else {
            mui_canvas_draw_utf8(p_canvas, 5, y += 15, getLangString(_L_BLANK_TAG));
        }
    }
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

        if(event->key == INPUT_KEY_CENTER){
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
