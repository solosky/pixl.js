#ifndef BOARD_NEO_H
#define BOARD_NEO_H

#define LEDS_NUMBER    3

#define LED_START      11
#define LED_1          11
#define LED_2          12
#define LED_3          33
#define LED_STOP       33

#define LEDS_ACTIVE_STATE 0

#define LEDS_INV_MASK  LEDS_MASK

#define LEDS_LIST { LED_1,LED_2, LED_3 }

#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2
#define BSP_LED_2      LED_3


#define BUTTONS_NUMBER 3

#define BUTTON_START   13
#define BUTTON_1       15
#define BUTTON_2       17
#define BUTTON_3       13
#define BUTTON_STOP    17
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

// #define LCD
// #define LCD_ST7567


// screen drivers
#define LCD_CS_PIN 3
#define LCD_RESET_PIN 28
#define LCD_BL_PIN 23 //NOT USED
#define LCD_DC_PIN 30 // green wire (DC)


// VFS driver
#define VFS_LFS_ENABLE
//#define VFS_SPIFFS_ENABLE

#define SPI_BUS_INSTANCE 3
#define NRFX_SPIM_SCK_PIN 29  
#define NRFX_SPIM_MOSI_PIN 31
#define NRFX_SPIM_MISO_PIN -1

// flash 
#define QSPI_FLASH 1
//#define SPI_FLASH 1

#ifdef QSPI_FLASH
#define QSPI_SCK_PIN 26  
#define QSPI_CS_PIN  4 
#define QSPI_IO0_PIN 7 
#define QSPI_IO1_PIN 6
#define QSPI_IO2_PIN 27
#define QSPI_IO3_PIN 8 
#endif


#define FLASH_CS_PIN -1

// Power
#define CHRG_PIN 40
#define ADC_PIN 5
#define ADC_INPUT 3


//READER

#define HF_ANT_SEL 36
#define RD_PWR  34
#define RC522_SPI_SELECT 42
#define RC522_SPI_MOSI 47
#define RC522_SPI_SCK  45
#define RC522_SPI_MISO 2

// APP 
//#define APP_PLAYER_ENABLED 
// APP
#define APP_LEGLAMIIBO_ENABLE
// #define APP_PLAYER_ENABLE
#if defined(INTERNAL_ENABLE)
#undef APP_LEGLAMIIBO_ENABLE
#endif


#endif

