#ifndef LCD_COMM_H
#define LCD_COMM_H

#include <stdint.h>
#include "boards_defines.h"

#define LED_1 31


enum{
	 OFF,
		ON,
};

#define		LCD_POWER_SET(ONOFF)					nrf_gpio_pin_write(LCD_BL_PIN, ONOFF)	
#define		LCD_RST_SET(ONOFF)						nrf_gpio_pin_write(LCD_RESET_PIN, ONOFF)

#define 	LCD_DC_RS_SET(RS_DC)					nrf_gpio_pin_write(LCD_DC_PIN, RS_DC)

enum{
		LCD_RS = 0,
		LCD_DC,
};

void lcd_comm_init(void);
void lcd_comm_uninit(void);
void lcd_reset(void);
void lcd_write_command(uint8_t data);
void lcd_write_data(uint8_t *data, uint8_t len);

void flash_write_read(uint8_t* tx_data, uint8_t tx_len, uint8_t* rx_data, uint32_t rx_len);
void flash_write_write(uint8_t* tx_data, uint8_t tx_len, uint8_t* rx_data, uint32_t rx_len);


#endif //LCD_COMM_H

