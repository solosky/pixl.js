#ifndef MUI_CANVAS_H
#define MUI_CANVAS_H

#include "mui_defines.h"
#include "u8g2.h"
#include <stdint.h>

typedef struct {
    u8g2_t *fb;
    uint8_t offset_x;
    uint8_t offset_y;
    uint8_t width;
    uint8_t height;
} mui_canvas_t;

void mui_canvas_flush(mui_canvas_t *p_canvas);
void mui_canvas_clear(mui_canvas_t *p_canvas);

void mui_canvas_set_font(mui_canvas_t *p_canvas, const uint8_t *font);
uint8_t mui_canvas_draw_utf8(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, const char *str);
uint8_t mui_canvas_draw_glyph(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint16_t encoding);
uint16_t mui_canvas_get_utf8_width(mui_canvas_t *p_canvas, const char *s);

void mui_canvas_set_frame(mui_canvas_t *p_canvas, uint8_t offset_x, uint8_t offset_y, uint8_t width, uint8_t height);

void mui_canvas_set_draw_color(mui_canvas_t *p_canvas, uint8_t color);
void mui_canvas_draw_box(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void mui_canvas_draw_dot(mui_canvas_t *p_canvas, uint8_t x, uint8_t y);
void mui_canvas_draw_rframe(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height,
                            uint8_t radius);

void mui_canvas_draw_xbm(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t *bitmap);

uint16_t mui_canvas_string_width(mui_canvas_t *p_canvas, const char *str);

uint8_t mui_canvas_get_width(mui_canvas_t *p_canvas);
uint8_t mui_canvas_get_height(mui_canvas_t *p_canvas);

uint8_t mui_canvas_current_font_height(mui_canvas_t *p_canvas);
void mui_canvas_invert_color(mui_canvas_t *p_canvas);
void mui_canvas_draw_line(mui_canvas_t *p_canvas, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
#endif