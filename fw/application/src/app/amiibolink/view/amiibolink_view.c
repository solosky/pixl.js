#include "amiibolink_view.h"
#include "amiibo_helper.h"
#include "mui_element.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"
#include "ntag_def.h"
#include "ntag_emu.h"
#include "i18n/language.h"
#include "db_header.h"

#define ICON_RANDOM 0xe20d
#define ICON_AUTO 0xe1b2
#define ICON_NTAG 0xe1cf
#define ICON_LEFT 0xe1ac
#define ICON_RIGHT 0xe1aa

static void amiibolink_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {

    amiibolink_view_t *p_amiibolink_view = p_view->user_data;
    ntag_t *ntag = ntag_emu_get_current_tag();
    char buff[64];

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);

    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);

    uint8_t y = 0;
    mui_canvas_draw_box(p_canvas, 0, y, mui_canvas_get_width(p_canvas), 12);
    mui_canvas_set_draw_color(p_canvas, 0);

    // draw mode icon
    if (p_amiibolink_view->amiibolink_mode == BLE_AMIIBOLINK_MODE_RANDOM){
        mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
        mui_canvas_draw_glyph(p_canvas, 10, y + 10, ICON_RANDOM);
    }else if(p_amiibolink_view->amiibolink_mode ==BLE_AMIIBOLINK_MODE_RANDOM_AUTO_GEN) {
        mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
        mui_canvas_draw_glyph(p_canvas, 10, y + 10, ICON_AUTO);
    } else if (p_amiibolink_view->amiibolink_mode == BLE_AMIIBOLINK_MODE_CYCLE) {

        if (p_amiibolink_view->index > 0) {
            mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
            mui_canvas_draw_glyph(p_canvas, 0, y + 10, ICON_LEFT);
        }

        mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
        sprintf(buff, "%02d", p_amiibolink_view->index + 1);
        mui_canvas_draw_utf8(p_canvas, 10, y + 10, buff);

        if (p_amiibolink_view->index < p_amiibolink_view->max_size - 1) {
            mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
            mui_canvas_draw_glyph(p_canvas, mui_canvas_get_width(p_canvas) - 10, y + 10, ICON_RIGHT);
        }
    } else if (p_amiibolink_view->amiibolink_mode == BLE_AMIIBOLINK_MODE_NTAG) {
        mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
        mui_canvas_draw_glyph(p_canvas, 10, y + 10, ICON_NTAG);
    } else {
        // nothing to draw here
    }

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
        const char *name =(getLanguage() == LANGUAGE_ZH_TW || getLanguage() == LANGUAGE_ZH_HANS) && strlen(amd->name_cn) > 0 ? amd->name_cn : amd->name_en;
        mui_element_autowrap_text(p_canvas, 5, y += 15, mui_canvas_get_width(p_canvas), 24, name);
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

static void amiibolink_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    amiibolink_view_t *p_amiibolink_view = p_view->user_data;
    if(event->type != INPUT_TYPE_SHORT){
        return;
    }
    if(event->key == INPUT_KEY_CENTER){
        if (p_amiibolink_view->event_cb) {
            p_amiibolink_view->event_cb(AMIIBOLINK_VIEW_EVENT_MENU, p_amiibolink_view);
        }
        return;
    }
    if (p_amiibolink_view->amiibolink_mode == BLE_AMIIBOLINK_MODE_CYCLE) {
        if (event->key == INPUT_KEY_LEFT) {
            if (p_amiibolink_view->index > 0) {
                p_amiibolink_view->index--;
            } else {
                p_amiibolink_view->index = p_amiibolink_view->max_size - 1;
            }
        } else if (event->key == INPUT_KEY_RIGHT) {
            if (p_amiibolink_view->index < p_amiibolink_view->max_size - 1) {
                p_amiibolink_view->index++;
            } else {
                p_amiibolink_view->index = 0;
            }
        }

        if (p_amiibolink_view->event_cb) {
            p_amiibolink_view->event_cb(AMIIBOLINK_VIEW_EVENT_UPDATE, p_amiibolink_view);
        }
    }
}

static void amiibolink_view_on_enter(mui_view_t *p_view) {}

static void amiibolink_view_on_exit(mui_view_t *p_view) {}

amiibolink_view_t *amiibolink_view_create() {
    amiibolink_view_t *p_amiibolink_view = mui_mem_malloc(sizeof(amiibolink_view_t));
    memset(p_amiibolink_view, 0, sizeof(amiibolink_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_amiibolink_view;
    p_view->draw_cb = amiibolink_view_on_draw;
    p_view->input_cb = amiibolink_view_on_input;
    p_view->enter_cb = amiibolink_view_on_enter;
    p_view->exit_cb = amiibolink_view_on_exit;

    p_amiibolink_view->p_view = p_view;
    p_amiibolink_view->amiibolink_mode = BLE_AMIIBOLINK_MODE_RANDOM;
    p_amiibolink_view->index = 0;

    return p_amiibolink_view;
}
void amiibolink_view_free(amiibolink_view_t *p_view) {
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}
mui_view_t *amiibolink_view_get_view(amiibolink_view_t *p_view) { return p_view->p_view; }
