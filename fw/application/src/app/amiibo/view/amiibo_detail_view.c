#include "amiibo_detail_view.h"
#include "amiibo_helper.h"
#include "db_header.h"
#include "i18n/language.h"
#include "mui_element.h"

#define ICON_LEFT 0xe1ac
#define ICON_RIGHT 0xe1aa
#define ICON_INFO 0xe0ae

static void amiibo_detail_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    char buff[64];
    amiibo_detail_view_t *p_amiibo_detail_view = p_view->user_data;
    ntag_t *ntag = p_amiibo_detail_view->ntag;

    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    sprintf(buff, "%02d %02x:%02x:%02x:%02x:%02x:%02x:%02x", p_amiibo_detail_view->focus + 1, ntag->data[0],
            ntag->data[1], ntag->data[2], ntag->data[4], ntag->data[5], ntag->data[6], ntag->data[7]);

    uint8_t y = 0;
    mui_canvas_draw_box(p_canvas, 0, y, mui_canvas_get_width(p_canvas), 12);
    mui_canvas_set_draw_color(p_canvas, 0);

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    if (p_amiibo_detail_view->focus > 0) {
        mui_canvas_draw_glyph(p_canvas, 0, y + 10, ICON_LEFT);
    }
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_canvas_draw_utf8(p_canvas, 10, y + 10, buff);

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    if (p_amiibo_detail_view->focus < p_amiibo_detail_view->max_ntags - 1) {
        mui_canvas_draw_glyph(p_canvas, mui_canvas_get_width(p_canvas) - 10, y + 10, ICON_RIGHT);
    }
    mui_canvas_set_draw_color(p_canvas, 1);
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);

    y += 12;

    if (ntag->read_only) {
        mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
        mui_canvas_draw_glyph(p_canvas, 0, y += 12, ICON_INFO);
        mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
        mui_canvas_draw_utf8(p_canvas, 12, y, string_get_cstr(p_amiibo_detail_view->file_name));
    } else {
        mui_canvas_draw_utf8(p_canvas, 0, y += 12, string_get_cstr(p_amiibo_detail_view->file_name));
    }

    uint32_t head = to_little_endian_int32(&ntag->data[84]);
    uint32_t tail = to_little_endian_int32(&ntag->data[88]);

    const db_amiibo_t *amd = get_amiibo_by_id(head, tail);
    if (amd != NULL) {
        const char *name =
            (getLanguage() == LANGUAGE_ZH_TW || getLanguage() == LANGUAGE_ZH_HANS) ? amd->name_cn : amd->name_en;
        mui_element_autowrap_text(p_canvas, 0, y += 15, mui_canvas_get_width(p_canvas), 24, name);
        if (strlen(ntag->notes) > 0) {
            mui_element_autowrap_text(p_canvas, 0, y += 13, mui_canvas_get_width(p_canvas), 24, ntag->notes);
        }
    } else if (head > 0 && tail > 0) {
        mui_canvas_draw_utf8(p_canvas, 0, y += 15, "Amiibo");
        sprintf(buff, "[%08x:%08x]", head, tail);
        mui_canvas_draw_utf8(p_canvas, 0, y += 15, buff);
    } else {
        mui_canvas_draw_utf8(p_canvas, 0, y += 13, getLangString(_L_BLANK_TAG));
    }
}

static void amiibo_detail_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    amiibo_detail_view_t *p_amiibo_detail_view = p_view->user_data;
    if (event->type == INPUT_TYPE_SHORT || event->type == INPUT_TYPE_REPEAT || event->type == INPUT_TYPE_LONG) {
        switch (event->key) {
        case INPUT_KEY_LEFT:
            if (p_amiibo_detail_view->focus > 0) {
                p_amiibo_detail_view->focus--;
            } else {
                p_amiibo_detail_view->focus = p_amiibo_detail_view->max_ntags - 1;
            }

            if (p_amiibo_detail_view->event_cb) {
                p_amiibo_detail_view->event_cb(AMIIBO_DETAIL_VIEW_EVENT_UPDATE, p_amiibo_detail_view);
            }
            break;
        case INPUT_KEY_RIGHT:
            if (p_amiibo_detail_view->focus < p_amiibo_detail_view->max_ntags - 1) {
                p_amiibo_detail_view->focus++;
            } else {
                p_amiibo_detail_view->focus = 0;
            }

            if (p_amiibo_detail_view->event_cb) {
                p_amiibo_detail_view->event_cb(AMIIBO_DETAIL_VIEW_EVENT_UPDATE, p_amiibo_detail_view);
            }
            break;
        case INPUT_KEY_CENTER:

            if (p_amiibo_detail_view->event_cb) {
                p_amiibo_detail_view->event_cb(AMIIBO_DETAIL_VIEW_EVENT_MENU, p_amiibo_detail_view);
            }
            break;
        }
    }
}

static void amiibo_detail_view_on_enter(mui_view_t *p_view) {}

static void amiibo_detail_view_on_exit(mui_view_t *p_view) {}

amiibo_detail_view_t *amiibo_detail_view_create() {
    amiibo_detail_view_t *p_amiibo_detail_view = mui_mem_malloc(sizeof(amiibo_detail_view_t));
    memset(p_amiibo_detail_view, 0, sizeof(amiibo_detail_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_amiibo_detail_view;
    p_view->draw_cb = amiibo_detail_view_on_draw;
    p_view->input_cb = amiibo_detail_view_on_input;
    p_view->enter_cb = amiibo_detail_view_on_enter;
    p_view->exit_cb = amiibo_detail_view_on_exit;

    p_amiibo_detail_view->p_view = p_view;

    string_init(p_amiibo_detail_view->file_name);

    return p_amiibo_detail_view;
}

void amiibo_detail_view_free(amiibo_detail_view_t *p_view) {
    string_clear(p_view->file_name);
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}

mui_view_t *amiibo_detail_view_get_view(amiibo_detail_view_t *p_view) { return p_view->p_view; }

void amiibo_detail_view_set_user_data(amiibo_detail_view_t *p_view, void *user_data) { p_view->user_data = user_data; }
void amiibo_detail_view_set_ntag(amiibo_detail_view_t *p_view, ntag_t *ntag) { p_view->ntag = ntag; }