/*
 * am_board.h
 *
 *  Created on: 2021年8月22日
 *      Author: solos
 */

#ifndef BOARD_DEFINES_H_
#define BOARD_DEFINES_H_

#ifdef __cplusplus
extern "C" {
#endif


#if defined(BOARD_OLED)
#include "boards/board_oled.h"
#elif defined(BOARD_DEFAULT)
#include "boards/board_default.h"
#else
#include "boards/board_default.h"
#endif 


#ifdef __cplusplus
}
#endif



#endif /* AM_BOARD_H_ */
