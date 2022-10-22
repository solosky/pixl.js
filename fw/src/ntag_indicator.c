/*
 * ntag_indicator.c
 *
 *  Created on: 2021年9月2日
 *      Author: solos
 */

#include "ntag_indicator.h"
#include "nrfx.h"
uint32_t index = 0;
uint32_t *volatile card_index_ptr = &index;
//uint32_t *volatile card_index_ptr = &(NRF_POWER->GPREGRET);

void ntag_indicator_on() {
	/*uint32_t card_index = *card_index_ptr;
	 if (card_index >= 5) {
	 bsp_board_led_on(5);
	 bsp_board_led_on(card_index - 5);

	 } else {
	 bsp_board_led_on(card_index);
	 }*/
}
void ntag_indicator_off() {
	//bsp_board_leds_off();
}

uint8_t ntag_indicator_current() {
	return *card_index_ptr;
}

uint8_t ntag_indicator_set(uint8_t index){
	*card_index_ptr = index;
	return index;
}

uint8_t ntag_indicator_switch_next() {
	uint32_t card_index = *card_index_ptr;
	card_index++;
	if (card_index == 10) {
		card_index = 0;
	}
	*card_index_ptr = card_index;
	ntag_indicator_off();
	ntag_indicator_on();
	return card_index;
}

uint8_t ntag_indicator_switch_prev() {
	uint32_t card_index = *card_index_ptr;
	if (card_index == 0) {
		card_index = 9;
	} else {
		card_index--;
	}
	*card_index_ptr = card_index;
	ntag_indicator_off();
	ntag_indicator_on();
	return card_index;

}
