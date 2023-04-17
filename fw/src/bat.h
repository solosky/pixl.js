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

typedef struct {
    bool charging;
    uint8_t level;
    float voltage;
} bat_state_t;

uint8_t bat_get_level(void);

void bat_get_state(bat_state_t *p_state);

#endif /* BAT_H_ */
