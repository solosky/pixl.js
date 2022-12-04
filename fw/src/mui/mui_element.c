#include "mui_element.h"
#include "mui_defines.h"

void mui_element_scrollbar(mui_canvas_t *p_canvas, uint32_t pos, uint16_t total) {
    uint8_t width = mui_canvas_get_width(p_canvas);
    uint8_t height = mui_canvas_get_height(p_canvas);
    // prevent overflows
    mui_canvas_set_draw_color(p_canvas, 0);
    mui_canvas_draw_box(p_canvas, width - 4, 0, 4, height);
    // dot line
    mui_canvas_set_draw_color(p_canvas, 1);
    for (uint8_t i = 0; i < height; i += 2) {
        mui_canvas_draw_dot(p_canvas, width - 2, i);
    }
    // Position block
    if (total) {
        float block_h = ((float)height) / total;
        mui_canvas_draw_box(p_canvas, width - 3, block_h * pos, 3, block_h > 1 ? block_h: 1);
    }
}