#ifndef MUI_ELEMENT_H
#define MUI_ELEMENT_H

#include "mui_defines.h"
#include "mui_canvas.h"

void mui_element_scrollbar(mui_canvas_t* p_canvas, uint32_t pos, uint16_t total);
void mui_element_multiline_text(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, const char *text);

void mui_element_button(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, const char *str, uint8_t selected);
void mui_element_autowrap_text(mui_canvas_t *p_canvas, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *text);

#endif