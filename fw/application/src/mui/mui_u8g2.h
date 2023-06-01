#ifndef MUI_U8G2_H
#define MUI_U8G2_H

#include "mui_defines.h"
#include "u8g2.h"

void mui_u8g2_init(u8g2_t* p_u8g2);
void mui_u8g2_deinit(u8g2_t* p_u8g2);

void mui_u8g2_set_backlight(uint8_t bl);
uint8_t mui_u8g2_get_backlight();

void mui_u8g2_set_backlight_level(uint8_t bl);
int8_t mui_u8g2_get_backlight_level(void);

#endif