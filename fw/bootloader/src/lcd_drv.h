#ifndef LCD_DRV_H
#define LCD_DRV_H

#include "stdint.h"

void lcd_init(void);
void lcd_uninit(void);
void lcd_clear(void);


void lcd_draw_str_0608(uint8_t x, uint8_t y, char *str);
void lcd_draw_str_1608(uint8_t x, uint8_t y, char *str);


void lcd_show_pic(uint8_t col, uint8_t row, uint8_t width, uint8_t hight, const uint8_t *data);

#endif //LCD_DRV_H

