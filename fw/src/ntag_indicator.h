/*
 * ntag_indicator.h
 *
 *  Created on: 2021年9月2日
 *      Author: solos
 */

#ifndef NTAG_INDICATOR_H_
#define NTAG_INDICATOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "bsp.h"
#include "bsp_nfc.h"

void ntag_indicator_on();
void ntag_indicator_off();
uint8_t ntag_indicator_current();
uint8_t ntag_indicator_switch_next();
uint8_t ntag_indicator_switch_prev();


#endif /* NTAG_INDICATOR_H_ */
