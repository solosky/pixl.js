/*
 * bat.h
 *
 *  Created on: 2021年9月25日
 *      Author: solos
 */

#ifndef BAT_H_
#define BAT_H_
#include <stdint.h>

typedef enum {
	EMPTY,
	LEVEL_1,
	LEVEL_2,
	LEVEL_3,
	FULL
} bat_level_t;

uint8_t bat_get_level(void);


#endif /* BAT_H_ */
