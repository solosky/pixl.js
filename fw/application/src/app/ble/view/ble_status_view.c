#include "ble_status_view.h"
#include "i18n/language.h"
#include "mui_qrcode_helper.h"

static void ble_status_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    ble_status_view_t* p_status_view = p_view->user_data;
    if (p_status_view->page == 1) {
        draw_qrcode(p_canvas, 39, 0, 50, "https://pixl.amiibo.xyz");
        return;
    }
    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    mui_canvas_draw_glyph(p_canvas, 20, 12, 0xe1b5);
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_canvas_draw_utf8(p_canvas, 30, 12, getLangString(_L_APP_BLE_TITLE));
    mui_canvas_draw_utf8(p_canvas, 18, 24, p_status_view->ble_addr);
    mui_canvas_draw_utf8(p_canvas, 6, 36, "https://pixl.amiibo.xyz");
}

static void ble_status_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    ble_status_view_t *p_ble_status_view = p_view->user_data;
    if (event->type == INPUT_TYPE_SHORT || event->type == INPUT_TYPE_REPEAT || event->type == INPUT_TYPE_LONG) {
        switch (event->key) {
        case INPUT_KEY_LEFT:
        case INPUT_KEY_RIGHT:
            if (p_ble_status_view->page == 1) {
                p_ble_status_view->page = 0;
            } else {
                p_ble_status_view->page = 1;
            }

            if (p_ble_status_view->event_cb) {
                p_ble_status_view->event_cb(BLE_STATUS_VIEW_EVENT_UPDATE, p_ble_status_view);
            }
            break;
        case INPUT_KEY_CENTER:
            if (p_ble_status_view->event_cb) {
                p_ble_status_view->event_cb(BLE_STATUS_VIEW_BLE_DISABLE, p_ble_status_view);
            }
            break;
        }
    }
}

static void ble_status_view_on_enter(mui_view_t *p_view) {}

static void ble_status_view_on_exit(mui_view_t *p_view) {}

ble_status_view_t *ble_status_view_create() {
    ble_status_view_t *p_ble_status_view = mui_mem_malloc(sizeof(ble_status_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_ble_status_view;
    p_view->draw_cb = ble_status_view_on_draw;
    p_view->input_cb = ble_status_view_on_input;
    p_view->enter_cb = ble_status_view_on_enter;
    p_view->exit_cb = ble_status_view_on_exit;

    p_ble_status_view->p_view = p_view;

    return p_ble_status_view;
}
void ble_status_view_free(ble_status_view_t *p_view) {
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}
mui_view_t *ble_status_view_get_view(ble_status_view_t *p_view) { return p_view->p_view; }