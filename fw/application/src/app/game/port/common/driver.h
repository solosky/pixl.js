// ===================================================================================
// Tiny Joypad Drivers for CH32V003                                           * v1.0 *
// ===================================================================================
//
// MCU abstraction layer.
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "game_view.h"
#include "nrf_delay.h"




// Pin assignments
#define PIN_ACT     PA2   // pin connected to fire button
#define PIN_BEEP    PA1   // pin connected to buzzer
#define PIN_PAD     PC4   // pin conected to direction buttons
#define PIN_SCL     PC2   // pin connected to OLED (I2C SCL)
#define PIN_SDA     PC1   // pin connected to OLED (I2C SDA)

// Joypad calibration values
#define JOY_N       197   // joypad UP
#define JOY_NE      259   // joypad UP + RIGHT
#define JOY_E       90    // joypad RIGHT
#define JOY_SE      388   // joypad DOWN + RIGHT
#define JOY_S       346   // joypad DOWN
#define JOY_SW      616   // joypad DOWN + LEFT
#define JOY_W       511   // joypad LEFT
#define JOY_NW      567   // JOYPAD UP + LEFT
#define JOY_DEV     20    // deviation

// Sound enable
#define JOY_SOUND   0     // 0: no sound, 1: with sound

// Game slow-down delay
#define JOY_SLOWDOWN()    DLY_ms(10)
#define JOY_init() //

// // Init driver
// static inline void JOY_init(void) {
//  //
// }

// OLED commands
#define JOY_OLED_init             //OLED_init
// #define JOY_OLED_end              //I2C_stop
// #define JOY_OLED_send(b)          //I2C_write(b)
// //#define JOY_OLED_send_command(c)  OLED_send_command(c)
// #define JOY_OLED_data_start(y)    {OLED_setpos(0,y);OLED_data_start();}



// Buttons
#define JOY_act_pressed()         game_view_key_pressed(INPUT_KEY_CENTER)
#define JOY_act_released()        !game_view_key_pressed(INPUT_KEY_CENTER)
#define JOY_pad_pressed()         false 
#define JOY_pad_released()        true
#define JOY_all_released()        (JOY_act_released() && JOY_pad_released())

#define JOY_left_pressed()        game_view_key_pressed(INPUT_KEY_LEFT)
#define JOY_right_pressed()       game_view_key_pressed(INPUT_KEY_RIGHT)

#define JOY_up_pressed()          false
#define JOY_down_pressed()        false


void JOY_OLED_data_start(uint8_t y);
void JOY_OLED_send(uint8_t b);
void JOY_OLED_end() ;

// Delays
#define JOY_DLY_ms    nrf_delay_ms
#define JOY_DLY_us    nrf_dely_us
#define DLY_ms nrf_delay_ms

// Additional Defines
#define abs(n) ((n>=0)?(n):(-(n)))

void JOY_sound(uint8_t freq, uint8_t dur); 
uint16_t JOY_random(void);
void JOY_idle();
long map(long x, long in_min, long in_max, long out_min, long out_max) ;

#ifdef __cplusplus
};
#endif
