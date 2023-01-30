#include "mui_canvas.h"

void mui_canvas_flush(mui_canvas_t *p_canvas) { u8g2_SendBuffer(p_canvas->fb); }

void mui_canvas_clear(mui_canvas_t *p_canvas) { u8g2_ClearBuffer(p_canvas->fb); }

void mui_canvas_set_font(mui_canvas_t *p_canvas, const uint8_t *font) { u8g2_SetFont(p_canvas->fb, font); }

uint8_t mui_canvas_draw_utf8(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, const char *str) {
    if (!str) return;
    x += p_canvas->offset_x;
    y += p_canvas->offset_y;
    return u8g2_DrawUTF8(p_canvas->fb, x, y, str);
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

uint16_t mui_canvas_string_width(mui_canvas_t *p_canvas, const char *str) {
    if (!str) return 0;
    return u8g2_GetStrWidth(p_canvas->fb, str);
}

uint8_t mui_canvas_get_width(mui_canvas_t *p_canvas) { return p_canvas->width; }

uint8_t mui_canvas_get_height(mui_canvas_t *p_canvas) { return p_canvas->height; }



uint8_t mui_canvas_current_font_height(mui_canvas_t* p_canvas){
    return u8g2_GetMaxCharHeight(p_canvas->fb);
}

void mui_canvas_invert_color(mui_canvas_t* p_canvas){
     p_canvas->fb->draw_color = !p_canvas->fb->draw_color;
}


void mui_canvas_draw_line(mui_canvas_t* p_canvas, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    x1 += p_canvas->offset_x;
    y1 += p_canvas->offset_y;
    x2 += p_canvas->offset_x;
    y2 += p_canvas->offset_y;
    u8g2_DrawLine(p_canvas->fb, x1, y1, x2, y2);
}