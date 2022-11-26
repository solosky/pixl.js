/*
 * ntag_indicator.c
 *
 *  Created on: 2021年9月2日
 *      Author: solos
 */

#include "ntag_indicator.h"
#include "amiibo_data.h"
#include "nrfx.h"
#include "ntag_emu.h"

#include "nrf_log.h"

#include "bat.h"
#include "u8g2_drv.h"

#define MAX_TAG_COUNT 50

static void ntag_indicator_internal_set(uint8_t index) {
    sd_power_gpregret_clr(0, 0xFF);
    sd_power_gpregret_set(0, index);
}

static uint8_t ntag_indicator_internal_get() {
    uint32_t index = 0;
    sd_power_gpregret_get(0, &index);
    index &= 0xFF;
    if (index > MAX_TAG_COUNT) {
        ntag_indicator_internal_set(0);
        index = 0;
    }
    return index;
}

static void ntag_indicator_draw_header() {
    uint8_t bt = bat_get_level();
    if (bt == 0) {
        bt = 1;
    }
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_siji_t_6x10);
    u8g2_DrawUTF8(&u8g2, 0, 8, "12:45");
    u8g2_DrawGlyph(&u8g2, 100, 8, 0xe1b5);
    u8g2_DrawGlyph(&u8g2, 110, 8, 0xe24c + bt - 1); //只有9个电池字符
}

static void ntag_indicator_draw_ntag(ntag_t *ntag, uint8_t offset) {
    u8g2_SetFont(&u8g2, u8g2_font_wqy12_t_gb2312a);

    char buff[64];

    sprintf(buff, "%02d %02x:%02x:%02x:%02x:%02x:%02x:%02x", ntag->index + 1,
            ntag->data[0], ntag->data[1], ntag->data[2], ntag->data[4], ntag->data[5],
            ntag->data[6], ntag->data[7]);
    u8g2_DrawBox(&u8g2, offset, 12, 128, 12);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, offset + 2, 22, buff);

    u8g2_SetDrawColor(&u8g2, 1);

    uint32_t head = to_little_endian_int32(&ntag->data[84]);
    uint32_t tail = to_little_endian_int32(&ntag->data[88]);

    const amiibo_data_t *amd = find_amiibo_data(head, tail);
    if (amd != NULL) {
        u8g2_DrawUTF8(&u8g2, offset, 36, amd->name);
        u8g2_DrawUTF8(&u8g2, offset, 48, amd->game_series);
        u8g2_DrawUTF8(&u8g2, offset, 62, amd->notes);
    } else {
        u8g2_DrawUTF8(&u8g2, offset, 36, "空标签");
    }
}

void ntag_indicator_update() {
 

    ntag_t *ntag = ntag_emu_get_current_tag();

    NRF_LOG_INFO("tag index: %d", ntag->index);
	ntag_indicator_draw_header();

	ntag_indicator_draw_ntag(ntag, 0);
    u8g2_SendBuffer(&u8g2);

}

uint8_t ntag_indicator_current() { return ntag_indicator_internal_get(); }

uint8_t ntag_indicator_set(uint8_t index) {
    ntag_indicator_internal_set(index);
    return index;
}

uint8_t ntag_indicator_switch_next() {
    uint32_t card_index = ntag_indicator_internal_get();
    card_index++;
    if (card_index == MAX_TAG_COUNT) {
        card_index = 0;
    }
    ntag_indicator_internal_set(card_index);
    return card_index;
}

uint8_t ntag_indicator_switch_prev() {
    uint32_t card_index = ntag_indicator_internal_get();
    if (card_index == 0) {
        card_index = MAX_TAG_COUNT - 1;
    } else {
        card_index--;
    }
    ntag_indicator_internal_set(card_index);
    return card_index;
}
