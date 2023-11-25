#ifndef BOARD_OLED_H
#define BOARD_OLED_H

#define LEDS_NUMBER    1

#define LED_START      31
#define LED_1          31
#define LED_STOP       31

#define LEDS_ACTIVE_STATE 0

#define LEDS_INV_MASK  LEDS_MASK

#define LEDS_LIST { LED_1 }

#define BSP_LED_0      LED_1


#define BUTTONS_NUMBER 3

#define BUTTON_START   5
#define BUTTON_1       5
#define BUTTON_2       6
#define BUTTON_3       7
#define BUTTON_STOP    7
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1, BUTTON_2, BUTTON_3}

#define BSP_BUTTON_0   BUTTON_1
#define BSP_BUTTON_1   BUTTON_2
#define BSP_BUTTON_2   BUTTON_3


// display config
#define OLED_SCREEN
#define OLED_SH1106
#define OLED_TYPE_SH1106
#define OLED_TYPE sh1106

//#define LCD
//#define LCD_ST7567


// screen drivers
#define LCD_CS_PIN 27
#define LCD_RESET_PIN 29
#define LCD_BL_PIN 30
#define LCD_DC_PIN 28 // green wire (DC)


// VFS driver
//#define VFS_LFS_ENABLE
#define VFS_SPIFFS_ENABLE

#define SPI_INSTANCE 0
#define NRFX_SPIM_SCK_PIN 26  // yellow wire (CLK)
#define NRFX_SPIM_MOSI_PIN 25 // blue wire (DIN)
#define NRFX_SPIM_MISO_PIN 19 // blue wire (DIN)

// flash
#define FLASH_CS_PIN 18

// Power
#define CHRG_PIN 3
#define ADC_PIN 2

// APP
#define APP_LEGLAMIIBO_ENABLE
// #define APP_PLAYER_ENABLE
#if defined(INTERNAL_ENABLE)
#undef APP_LEGLAMIIBO_ENABLE
#endif


#endif