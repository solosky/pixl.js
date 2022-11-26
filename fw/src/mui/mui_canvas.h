#ifndef MUI_CANVAS_H
#define MUI_CANVAS_H

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
void mui_canvas_draw_utf8(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, const char *str);

void mui_canvas_set_frame(mui_canvas_t *p_canvas, uint8_t offset_x, uint8_t offset_y,
                          uint8_t width, uint8_t height);
#endif