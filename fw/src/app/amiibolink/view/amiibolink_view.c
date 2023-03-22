#include "amiibolink_view.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"
#include "ntag_def.h"
#include "ntag_emu.h"
#include "amiibo_data.h"
#include "amiibo_helper.h"
#include "mui_element.h"

static void amiibolink_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {

    amiibolink_view_t *p_amiibolink_view = p_view->user_data;
    ntag_t *ntag = ntag_emu_get_current_tag();
    char buff[64];

    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    sprintf(buff, "%02d %02x:%02x:%02x:%02x:%02x:%02x:%02x", 0, ntag->data[0],
            ntag->data[1], ntag->data[2], ntag->data[4], ntag->data[5], ntag->data[6], ntag->data[7]);

    uint8_t y = 0;
    mui_canvas_draw_box(p_canvas, 0, y, mui_canvas_get_width(p_canvas), 12);
    mui_canvas_set_draw_color(p_canvas, 0);

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
//    if (p_amiibolink_view->focus > 0) {
//        mui_canvas_draw_glyph(p_canvas, 0, y + 10, ICON_LEFT);
//    }
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_canvas_draw_utf8(p_canvas, 10, y + 10, buff);

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
//    if (p_amiibolink_view->focus < p_amiibolink_view->max_ntags - 1) {
//        mui_canvas_draw_glyph(p_canvas, mui_canvas_get_width(p_canvas) - 10, y + 10, ICON_RIGHT);
//    }
    mui_canvas_set_draw_color(p_canvas, 1);
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);

    y += 12;

    mui_canvas_draw_rframe(p_canvas, 3, y + 3, mui_canvas_get_width(p_canvas) - 3, mui_canvas_get_height(p_canvas) - y - 3, 3);



//    mui_canvas_draw_utf8(p_canvas, 0, y += 12, string_get_cstr(p_amiibolink_view->file_name));

    uint32_t head = to_little_endian_int32(&ntag->data[84]);
    uint32_t tail = to_little_endian_int32(&ntag->data[88]);

    const amiibo_data_t *amd = find_amiibo_data(head, tail);
    if (amd != NULL) {
        mui_canvas_draw_utf8(p_canvas, 5, y += 15, amd->name);
        if (strlen(ntag->notes) > 0) {
            mui_element_autowrap_text(p_canvas, 5, y += 15, mui_canvas_get_width(p_canvas), 24, ntag->notes);
        } else {
            mui_element_autowrap_text(p_canvas, 5, y += 15, mui_canvas_get_width(p_canvas), 24, amd->notes);
        }
    } else {
        mui_canvas_draw_utf8(p_canvas, 5, y += 15, "空标签");
    }
}

static void amiibolink_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    amiibolink_view_t *p_amiibolink_view = p_view->user_data;
    switch (event->type) {
    case INPUT_TYPE_LONG: {
        if (p_amiibolink_view->event_cb) {
            p_amiibolink_view->event_cb(AMIIBOLINK_VIEW_EVENT_KEY_LONG_PRESSED, p_amiibolink_view);
        }
        break;
    }
    case INPUT_TYPE_PRESS: {
        if (p_amiibolink_view->event_cb) {
            p_amiibolink_view->event_cb(AMIIBOLINK_VIEW_EVENT_KEY_PRESSED, p_amiibolink_view);
        }
        break;
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

    return p_amiibolink_view;
}
void amiibolink_view_free(amiibolink_view_t *p_view) {
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}
mui_view_t *amiibolink_view_get_view(amiibolink_view_t *p_view) { return p_view->p_view; }
