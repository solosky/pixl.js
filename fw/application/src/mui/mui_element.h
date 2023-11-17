#ifndef MUI_ELEMENT_H
#define MUI_ELEMENT_H

#include "mui_defines.h"
#include "mui_canvas.h"

void mui_element_scrollbar(mui_canvas_t* p_canvas, uint32_t pos, uint16_t total);
void mui_element_multiline_text(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, const char *text);

void mui_element_button(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, const char *str, uint8_t selected);
void mui_element_autowrap_text(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *text);
uint16_t mui_element_autowrap_text_box(mui_canvas_t *p_canvas, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t offset_y, uint8_t square_r, const char *text);
uint16_t mui_element_text_height(mui_canvas_t *p_canvas, uint8_t w, const char *text);

#endif