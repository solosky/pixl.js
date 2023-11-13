#include "amiibo_view.h"
#include "db_header.h"
#include "i18n/language.h"
#include "mui_element.h"
#include "mui_qrcode_helper.h"
#include "settings.h"

#define ICON_LEFT 0xe1ac
#define ICON_RIGHT 0xe1aa

static void amiibo_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    char buff[64];
    amiibo_view_t *p_amiibo_view = p_view->user_data;
    ntag_t *ntag = p_amiibo_view->ntag;

    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    sprintf(buff, "%02d %02x:%02x:%02x:%02x:%02x:%02x:%02x", p_amiibo_view->focus + 1, ntag->data[0], ntag->data[1],
            ntag->data[2], ntag->data[4], ntag->data[5], ntag->data[6], ntag->data[7]);

    uint8_t y = 0;
    mui_canvas_draw_box(p_canvas, 0, y, mui_canvas_get_width(p_canvas), 12);
    mui_canvas_set_draw_color(p_canvas, 0);

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    if (p_amiibo_view->focus > 0) {
        mui_canvas_draw_glyph(p_canvas, 0, y + 10, ICON_LEFT);
    }
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_canvas_draw_utf8(p_canvas, 10, y + 10, buff);

    mui_canvas_set_font(p_canvas, u8g2_font_siji_t_6x10);
    if (p_amiibo_view->focus < p_amiibo_view->max_ntags - 1) {
        mui_canvas_draw_glyph(p_canvas, mui_canvas_get_width(p_canvas) - 10, y + 10, ICON_RIGHT);
    }
    mui_canvas_set_draw_color(p_canvas, 1);
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);

    y += 12;

    uint32_t head = to_little_endian_int32(&ntag->data[84]);
    uint32_t tail = to_little_endian_int32(&ntag->data[88]);

    const db_amiibo_t *amd = get_amiibo_by_id(head, tail);
    if (amd != NULL) {
        const char *name =(getLanguage() == LANGUAGE_ZH_TW || getLanguage() == LANGUAGE_ZH_HANS) ? amd->name_cn : amd->name_en;
        mui_canvas_draw_utf8(p_canvas, 0, y += 13, name);

        uint16_t start_y = 27;
        mui_rect_t clip_win_prev;
        mui_rect_t clip_win_cur;
        mui_canvas_get_clip_window(p_canvas, &clip_win_prev);
        clip_win_cur.x = 0;
        clip_win_cur.y = start_y;
        clip_win_cur.w = mui_canvas_get_width(p_canvas);
        clip_win_cur.h = mui_canvas_get_height(p_canvas) - start_y;
        mui_canvas_set_clip_window(p_canvas, &clip_win_cur);

        // text+qrcode
        uint8_t square_r = 0;
        if (settings_get_data()->qrcode_enabled) {
            square_r = 37;
            sprintf(buff, "https://pixl.amiibo.xyz/a/%08X-%08X", head, tail);
            if (p_amiibo_view->desc_offset <= 33) {
                draw_qrcode(p_canvas, 91, start_y - p_amiibo_view->desc_offset, 33, buff);
            }
        }
        const db_link_t *link = get_link_by_id(p_amiibo_view->game_id, head, tail);
        if (strlen(ntag->notes) > 0) {
            mui_element_autowrap_text_box(p_canvas, clip_win_cur.x, clip_win_cur.y, clip_win_cur.w, clip_win_cur.h, p_amiibo_view->desc_offset, square_r, ntag->notes);
        } else if (link != NULL) {
            const char *notes = getLanguage() == LANGUAGE_ZH_HANS ? link->note_cn : link->note_en;
            mui_element_autowrap_text_box(p_canvas, clip_win_cur.x, clip_win_cur.y, clip_win_cur.w, clip_win_cur.h, p_amiibo_view->desc_offset, square_r, notes);
        }
        mui_canvas_set_clip_window(p_canvas, &clip_win_prev);
    } else if (head > 0 && tail > 0) {
        mui_canvas_draw_utf8(p_canvas, 0, y += 15, "Amiibo");
        sprintf(buff, "[%08x:%08x]", head, tail);
        mui_canvas_draw_utf8(p_canvas, 0, y += 15, buff);
        if (settings_get_data()->qrcode_enabled) {
            sprintf(buff, "https://pixl.amiibo.xyz/a/%08X-%08X", head, tail);
            draw_qrcode(p_canvas, 91, 27, 33, buff);
        }
    } else {
        mui_canvas_draw_utf8(p_canvas, 0, y += 13, getLangString(_L_BLANK_TAG));
    }
}

static void amiibo_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    amiibo_view_t *p_amiibo_view = p_view->user_data;
    if (event->type == INPUT_TYPE_REPEAT || event->type == INPUT_TYPE_LONG) {
        switch (event->key) {
        case INPUT_KEY_LEFT:
            if (p_amiibo_view->desc_offset > 0) {
                p_amiibo_view->desc_offset -= 8;
            }

            if (p_amiibo_view->event_cb) {
                p_amiibo_view->event_cb(AMIIBO_VIEW_EVENT_DESC_UPDATE, p_amiibo_view);
            }
            break;
        case INPUT_KEY_RIGHT:
            p_amiibo_view->desc_offset += 8;

            if (p_amiibo_view->event_cb) {
                p_amiibo_view->event_cb(AMIIBO_VIEW_EVENT_DESC_UPDATE, p_amiibo_view);
            }
            break;
        }
    } else if (event->type == INPUT_TYPE_SHORT) {
        switch (event->key) {
        case INPUT_KEY_LEFT:
            p_amiibo_view->desc_offset = 0;
            if (p_amiibo_view->max_ntags > 1) {
                if (p_amiibo_view->focus > 0) {
                    p_amiibo_view->focus--;
                } else {
                    p_amiibo_view->focus = p_amiibo_view->max_ntags - 1;
                }

                if (p_amiibo_view->event_cb) {
                    p_amiibo_view->event_cb(AMIIBO_VIEW_EVENT_UPDATE, p_amiibo_view);
                }
            }
            break;
        case INPUT_KEY_RIGHT:
            p_amiibo_view->desc_offset = 0;
            if (p_amiibo_view->max_ntags > 1) {
                if (p_amiibo_view->focus < p_amiibo_view->max_ntags - 1) {
                    p_amiibo_view->focus++;
                } else {
                    p_amiibo_view->focus = 0;
                }

                if (p_amiibo_view->event_cb) {
                    p_amiibo_view->event_cb(AMIIBO_VIEW_EVENT_UPDATE, p_amiibo_view);
                }
            }
            break;
        case INPUT_KEY_CENTER:

            if (p_amiibo_view->event_cb) {
                p_amiibo_view->event_cb(AMIIBO_VIEW_EVENT_MENU, p_amiibo_view);
            }
            break;
        }
    }
}

static void amiibo_view_on_enter(mui_view_t *p_view) {}

static void amiibo_view_on_exit(mui_view_t *p_view) {}

amiibo_view_t *amiibo_view_create() {
    amiibo_view_t *p_amiibo_view = mui_mem_malloc(sizeof(amiibo_view_t));
    memset(p_amiibo_view, 0, sizeof(amiibo_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_amiibo_view;
    p_view->draw_cb = amiibo_view_on_draw;
    p_view->input_cb = amiibo_view_on_input;
    p_view->enter_cb = amiibo_view_on_enter;
    p_view->exit_cb = amiibo_view_on_exit;
    p_amiibo_view->p_view = p_view;
    return p_amiibo_view;
}

void amiibo_view_free(amiibo_view_t *p_view) {
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}

mui_view_t *amiibo_view_get_view(amiibo_view_t *p_view) { return p_view->p_view; }
