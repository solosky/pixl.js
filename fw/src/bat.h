/*
 * bat.h
 *
 *  Created on: 2021年9月25日
 *      Author: solos
 */

#ifndef BAT_H_
#define BAT_H_
#include <stdint.h>
#include <stdbool.h>

typedef enum {
	EMPTY,
	LEVEL_1,
	LEVEL_2,
	LEVEL_3,
	FULL
} bat_level_t;

typedef void (*chrg_data_cb_t)(void);
typedef struct chrg {
	uint32_t stats;
	chrg_data_cb_t callback;
} chrg_data_t;

uint8_t bat_get_level(void);
void chrg_init(void);
bool get_stats(void);
void chrg_set_callback(void *cb);


#endif /* BAT_H_ */
