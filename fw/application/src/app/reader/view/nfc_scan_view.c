#include "nfc_scan_view.h"
#include "i18n/language.h"

static void nfc_scan_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    nfc_scan_view_t* p_status_view = p_view->user_data;
    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    mui_canvas_draw_glyph(p_canvas, 20, 12, 0xe1b5);
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_canvas_draw_utf8(p_canvas, 30, 12, "请靠近卡片..");
}

static void nfc_scan_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    nfc_scan_view_t *p_nfc_scan_view = p_view->user_data;
    if(p_nfc_scan_view->event_cb){
        p_nfc_scan_view->event_cb(NFC_SCAN_VIEW_MENU, p_nfc_scan_view);
    }
}

static void nfc_scan_view_on_enter(mui_view_t *p_view) {}

static void nfc_scan_view_on_exit(mui_view_t *p_view) {}

nfc_scan_view_t *nfc_scan_view_create() {
    nfc_scan_view_t *p_nfc_scan_view = mui_mem_malloc(sizeof(nfc_scan_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_nfc_scan_view;
    p_view->draw_cb = nfc_scan_view_on_draw;
    p_view->input_cb = nfc_scan_view_on_input;
    p_view->enter_cb = nfc_scan_view_on_enter;
    p_view->exit_cb = nfc_scan_view_on_exit;

    p_nfc_scan_view->p_view = p_view;

    return p_nfc_scan_view;
}
void nfc_scan_view_free(nfc_scan_view_t *p_view) {
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}
mui_view_t *nfc_scan_view_get_view(nfc_scan_view_t *p_view) { return p_view->p_view; }