/*
 * ntag_indicator.c
 *
 *  Created on: 2021年9月2日
 *      Author: solos
 */

#include "ntag_indicator.h"
#include "ntag_emu.h"
#include "amiibo_data.h"
#include "nrfx.h"

#include "u8g2_drv.h"

uint32_t index = 0;
uint32_t *volatile card_index_ptr = &index;
//uint32_t *volatile card_index_ptr = &(NRF_POWER->GPREGRET);

void ntag_indicator_update() {
	uint32_t card_index = *card_index_ptr;
	 /*if (card_index >= 5) {
	 bsp_board_led_on(5);
	 bsp_board_led_on(card_index - 5);

	 } else {
	 bsp_board_led_on(card_index);
	 }*/

	 u8g2_ClearBuffer(&u8g2);
     u8g2_SetFont(&u8g2, u8g2_font_wqy12_t_gb2312a);


	char buff[64];
	sprintf(buff, "序号: %02d", card_index);
	u8g2_DrawUTF8(&u8g2,0, 15,  buff);

	ntag_t* ntag = ntag_emu_get_current_tag();

	sprintf(buff, "UID: %02x:%02x:%02x:%02x:%02x:%02x:%02x",
			ntag->data[0], ntag->data[1], ntag->data[2], ntag->data[4],
			ntag->data[5], ntag->data[6], ntag->data[7]);
	u8g2_DrawUTF8(&u8g2,0, 30,  buff);

	uint32_t head = to_little_endian_int32(&ntag->data[84]);
	uint32_t tail = to_little_endian_int32(&ntag->data[88]);

	const amiibo_data_t *amd = find_amiibo_data(head, tail);
	if (amd != NULL) {
		u8g2_DrawUTF8(&u8g2,0, 45,  amd->name);
		u8g2_DrawUTF8(&u8g2,0, 60,  amd->game_series);
	}else{
		u8g2_DrawUTF8(&u8g2,0, 45,  "空标签");
	}


    u8g2_SendBuffer(&u8g2);
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
	return card_index;

}
