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

#define MAX_TAG_COUNT 50

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
	u8g2_SetFont(&u8g2, u8g2_font_siji_t_6x10);
	u8g2_DrawUTF8(&u8g2, 0, 8, "12:45");
	u8g2_DrawGlyph(&u8g2, 100, 8,  0xe1b5);
	u8g2_DrawGlyph(&u8g2, 110, 8,  0xe250);

	//u8g2_SetFontMode(&u8g2, 1); 


     u8g2_SetFont(&u8g2, u8g2_font_wqy12_t_gb2312a);

	char buff[64];
	
	// sprintf(buff, "---------%02d----------", card_index);
	// u8g2_DrawBox(&u8g2, 0, 12, 128, 12);
	// u8g2_SetDrawColor(&u8g2, 0);
	// u8g2_DrawUTF8(&u8g2,0, 22,  buff);

	ntag_t* ntag = ntag_emu_get_current_tag();

	sprintf(buff, "%02d %02x:%02x:%02x:%02x:%02x:%02x:%02x",
	card_index + 1,
			ntag->data[0], ntag->data[1], ntag->data[2], ntag->data[4],
			ntag->data[5], ntag->data[6], ntag->data[7]);
	u8g2_DrawBox(&u8g2, 0, 12, 128, 12);
	u8g2_SetDrawColor(&u8g2, 0);
	u8g2_DrawUTF8(&u8g2,2, 22,  buff);

	u8g2_SetDrawColor(&u8g2, 1);

	uint32_t head = to_little_endian_int32(&ntag->data[84]);
	uint32_t tail = to_little_endian_int32(&ntag->data[88]);

	const amiibo_data_t *amd = find_amiibo_data(head, tail);
	if (amd != NULL) {
		u8g2_DrawUTF8(&u8g2,0, 36,  amd->name);
		u8g2_DrawUTF8(&u8g2,0, 48,  amd->game_series);
		u8g2_DrawUTF8(&u8g2,0, 62,   amd->notes);
	}else{
		u8g2_DrawUTF8(&u8g2,0, 36,  "空标签");
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
	if (card_index == MAX_TAG_COUNT) {
		card_index = 0;
	}
	*card_index_ptr = card_index;
	return card_index;
}

uint8_t ntag_indicator_switch_prev() {
	uint32_t card_index = *card_index_ptr;
	if (card_index == 0) {
		card_index = MAX_TAG_COUNT - 1;
	} else {
		card_index--;
	}
	*card_index_ptr = card_index;
	return card_index;

}
