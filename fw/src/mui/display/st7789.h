//
// Created by solos on 2021/11/21.
//

#ifndef FW_ST7789_H
#define FW_ST7789_H

typedef enum
{
    ST7789_EVENT_FLUSH_DONE
} st7789_evt_type_t;

typedef void (* st7789_event_handler_t)(st7789_evt_type_t evt, void* p_context);

ret_code_t st7789_init(void);
void st7789_display_area(uint8_t * data, uint16_t len, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                         st7789_event_handler_t handler, void* p_context);
void st7789_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
#endif //FW_ST7789_H
