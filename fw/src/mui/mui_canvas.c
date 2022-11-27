#include "mui_canvas.h"

void mui_canvas_flush(mui_canvas_t *p_canvas) { u8g2_SendBuffer(p_canvas->fb); }

void mui_canvas_clear(mui_canvas_t *p_canvas) { u8g2_ClearBuffer(p_canvas->fb); }

void mui_canvas_set_font(mui_canvas_t *p_canvas, const uint8_t *font) {
    u8g2_SetFont(p_canvas->fb, font);
}

void mui_canvas_draw_utf8(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, const char *str) {
    if (!str) return;
    x += p_canvas->offset_x;
    y += p_canvas->offset_y;
    u8g2_DrawUTF8(p_canvas->fb, x, y, str);
}

void mui_canvas_set_frame(mui_canvas_t *p_canvas, uint8_t offset_x, uint8_t offset_y,
                          uint8_t width, uint8_t height) {
    p_canvas->offset_x = offset_x;
    p_canvas->offset_y = offset_y;
    p_canvas->width = width;
    p_canvas->height = height;
}

void mui_canvas_set_draw_color(mui_canvas_t *p_canvas, uint8_t color) {
    u8g2_SetDrawColor(p_canvas->fb, color);
}

void mui_canvas_draw_box(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint8_t w,
                         uint8_t h) {
    x += p_canvas->offset_x;
    y += p_canvas->offset_y;
    u8g2_DrawBox(p_canvas->fb, x, y, w, h);
}

uint8_t mui_canvas_get_width(mui_canvas_t *p_canvas){
    return p_canvas->width;
}

uint8_t mui_canvas_get_height(mui_canvas_t *p_canvas){
    return p_canvas->height;
}