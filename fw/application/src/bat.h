/*
 * bat.h
 *
 *  Created on: 2021年9月25日
 *      Author: solos
 */

#ifndef BAT_H_
#define BAT_H_
#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool charging;
    uint8_t level;
    float voltage;
} bat_state_t;

typedef void (*chrg_data_cb_t)(void);
typedef struct chrg {
	uint32_t stats;
	chrg_data_cb_t callback;
} chrg_data_t;

uint8_t bat_get_level(void);
void chrg_init(void);
bool get_stats(void);
void chrg_set_callback(void *cb);

void bat_get_state(bat_state_t *p_state);

#endif /* BAT_H_ */
