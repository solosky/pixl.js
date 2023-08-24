#include "mui_canvas.h"
#include <string.h>

void mui_canvas_flush(mui_canvas_t *p_canvas) { u8g2_SendBuffer(p_canvas->fb); }

void mui_canvas_clear(mui_canvas_t *p_canvas) { u8g2_ClearBuffer(p_canvas->fb); }

void mui_canvas_set_font(mui_canvas_t *p_canvas, const uint8_t *font) { u8g2_SetFont(p_canvas->fb, font); }

uint8_t mui_canvas_get_utf8_bytes(const char *p) {
    char c = *p;
    if (c >> 7 == 0) { // 0xxxxxxx (一位的情况,为ASCII)
        return 1;
    } else if (c >> 5 == 0x6) { // 110xxxxx 10xxxxxx (110开头,代表两位)
        return 2;
    } else if (c >> 4 == 0xE) { // 1110xxxx 10xxxxxx 10xxxxxx (1110开头代表三位)
        return 3;
    } else {
        return 4;
    }
}

uint8_t mui_canvas_draw_utf8(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, const char *str) {
    if (!str) return 0;
    x += p_canvas->offset_x;
    y += p_canvas->offset_y;
    return u8g2_DrawUTF8(p_canvas->fb, x, y, str);
}

int32_t mui_canvas_draw_utf8_clip(mui_canvas_t *p_canvas, int32_t x, int32_t y, const char *text) {
    int32_t xi = x;
    int32_t yi = y;
    int32_t w = 0;

    char *p = text;
    char utf8[5];

    while (*p != 0) {
        uint8_t utf8_size = mui_canvas_get_utf8_bytes(p);
        memcpy(utf8, p, utf8_size);
        utf8[utf8_size] = '\0';
        if (xi >= 0 && xi <= p_canvas->width) {
            uint8_t utf8_w = mui_canvas_draw_utf8(p_canvas, xi, yi, utf8);
            xi += utf8_w;
            w += utf8_w;
        } else {
            uint8_t utf8_x = mui_canvas_get_utf8_width(p_canvas, utf8);
            xi += utf8_x + 1; //1 pix for margin
            w += utf8_x + 1;
        }
        p += utf8_size;
    }

    return w;
}

uint8_t mui_canvas_draw_glyph(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint16_t encoding) {
    x += p_canvas->offset_x;
    y += p_canvas->offset_y;
    u8g2_DrawGlyph(p_canvas->fb, x, y, encoding);
}

uint16_t mui_canvas_get_utf8_width(mui_canvas_t *p_canvas, const char *s) { return u8g2_GetUTF8Width(p_canvas->fb, s); }

void mui_canvas_set_frame(mui_canvas_t *p_canvas, uint8_t offset_x, uint8_t offset_y, uint8_t width, uint8_t height) {
    p_canvas->offset_x = offset_x;
    p_canvas->offset_y = offset_y;
    p_canvas->width = width;
    p_canvas->height = height;
    u8g2_SetClipWindow(p_canvas->fb, offset_x, offset_y, offset_x + width, offset_y + height);
}

void mui_canvas_get_clip_window(mui_canvas_t *p_canvas, mui_rect_t *p_rect) {
    p_rect->x = p_canvas->fb->clip_x0 - p_canvas->offset_x;
    p_rect->y = p_canvas->fb->clip_y0 - p_canvas->offset_y;
    p_rect->w = p_canvas->width;
    p_rect->h = p_canvas->height;
}
void mui_canvas_set_clip_window(mui_canvas_t *p_canvas, mui_rect_t *p_rect) {
    u8g2_SetClipWindow(p_canvas->fb, p_rect->x + p_canvas->offset_x, p_rect->y + p_canvas->offset_y,
                       p_rect->x + p_rect->w, p_rect->y + p_rect->w);
}

void mui_canvas_set_draw_color(mui_canvas_t *p_canvas, uint8_t color) { u8g2_SetDrawColor(p_canvas->fb, color); }

void mui_canvas_draw_box(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    x += p_canvas->offset_x;
    y += p_canvas->offset_y;
    u8g2_DrawBox(p_canvas->fb, x, y, w, h);
}

void mui_canvas_draw_dot(mui_canvas_t *p_canvas, uint8_t x, uint8_t y) {
    x += p_canvas->offset_x;
    y += p_canvas->offset_y;
    u8g2_DrawPixel(p_canvas->fb, x, y);
}

void mui_canvas_draw_rframe(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height,
                            uint8_t radius) {
    x += p_canvas->offset_x;
    y += p_canvas->offset_y;
    u8g2_DrawRFrame(p_canvas->fb, x, y, width, height, radius);
}

void mui_canvas_draw_xbm(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t *bitmap) {
    x += p_canvas->offset_x;
    y += p_canvas->offset_y;
    u8g2_DrawXBM(p_canvas->fb, x, y, width, height, bitmap);
}

uint16_t mui_canvas_string_width(mui_canvas_t *p_canvas, const char *str) {
    if (!str) return 0;
    return u8g2_GetStrWidth(p_canvas->fb, str);
}

uint8_t mui_canvas_get_width(mui_canvas_t *p_canvas) { return p_canvas->width; }

uint8_t mui_canvas_get_height(mui_canvas_t *p_canvas) { return p_canvas->height; }

uint8_t mui_canvas_current_font_height(mui_canvas_t *p_canvas) { return u8g2_GetMaxCharHeight(p_canvas->fb); }

void mui_canvas_invert_color(mui_canvas_t *p_canvas) { p_canvas->fb->draw_color = !p_canvas->fb->draw_color; }

void mui_canvas_draw_line(mui_canvas_t *p_canvas, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    x1 += p_canvas->offset_x;
    y1 += p_canvas->offset_y;
    x2 += p_canvas->offset_x;
    y2 += p_canvas->offset_y;
    u8g2_DrawLine(p_canvas->fb, x1, y1, x2, y2);
}