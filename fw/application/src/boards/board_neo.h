#ifndef BOARD_OLED_H
#define BOARD_OLED_H

#define LEDS_NUMBER    1

#define LED_START      5
#define LED_1          5
#define LED_2          7
#define LED_3          27
#define LED_STOP       27

#define LEDS_ACTIVE_STATE 0

#define LEDS_INV_MASK  LEDS_MASK

#define LEDS_LIST { LED_1,LED_2, LED_3 }

#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2
#define BSP_LED_2      LED_3


#define BUTTONS_NUMBER 3

#define BUTTON_START   13
#define BUTTON_1       33
#define BUTTON_2       13
#define BUTTON_3       15
#define BUTTON_STOP    33
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1, BUTTON_2, BUTTON_3}

#define BSP_BUTTON_0   BUTTON_1
#define BSP_BUTTON_1   BUTTON_2
#define BSP_BUTTON_2   BUTTON_3


// display config
//#define OLED_SCREEN
//#define OLED_SH1106
//#define OLED_TYPE_SH1106
//#define OLED_TYPE sh1106

#define LCD
#define LCD_ST7567


// screen drivers
#define LCD_CS_PIN 12
#define LCD_RESET_PIN 6
#define LCD_BL_PIN 43
#define LCD_DC_PIN 11 // green wire (DC)


// VFS driver
#define VFS_LFS_ENABLE
//#define VFS_SPIFFS_ENABLE

#define SPI_INSTANCE 3
#define NRFX_SPIM_SCK_PIN 8  
#define NRFX_SPIM_MOSI_PIN 41 
#define NRFX_SPIM_MISO_PIN -1

// flash 
#define QSPI_FLASH 1
//#define SPI_FLASH 1

#ifdef QSPI_FLASH
#define QSPI_SCK_PIN 20  
#define QSPI_CS_PIN 17 
#define QSPI_IO0_PIN 22 
#define QSPI_IO1_PIN 24
#define QSPI_IO2_PIN 25
#define QSPI_IO3_PIN 32 
#endif


#define FLASH_CS_PIN -1

// Power
#define CHRG_PIN -1
#define ADC_PIN 31

// APP 
//#define APP_PLAYER_ENABLED 
// APP
#define APP_LEGLAMIIBO_ENABLE
// #define APP_PLAYER_ENABLE
#if defined(INTERNAL_ENABLE)
#undef APP_LEGLAMIIBO_ENABLE
#endif


#endif

