/*
 * custom_board.h
 *
 *  Created on: 2021年8月22日
 *      Author: solos
 */
#ifndef CUSTOM_BOARD_H_
#define CUSTOM_BOARD_H_
#ifdef __cplusplus
extern "C" {
#endif

#if defined(BOARD_OLED)
#include "boards/board_oled.h"
#elif defined(BOARD_LCD)
#include "boards/board_lcd.h"
#else
#include "boards/board_lcd.h"
#endif

#ifdef __cplusplus
}
#endif

#endif // CUSTOM_BOARD_H_