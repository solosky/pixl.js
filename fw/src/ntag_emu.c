/*
 * t2t_emu.c
 *
 *  Created on: 2021年5月30日
 *      Author: solos
 */
#include "nrf52.h"
#include "nrf52_bitfields.h"
#include "nrf_log.h"
#include "nrfx.h"
#include "nrf_nfct.h"
#include "nrfx_nfct.h"
#include "nfc_platform.h"
#include "boards.h"
#include "app_timer.h"
#include "app_scheduler.h"

#include "ntag_emu.h"
#include "ntag_indicator.h"

typedef struct {
	ntag_t ntag;
	uint8_t dirty;
	uint8_t busy;
} ntag_emu_t;

uint8_t rx_buf[64];
uint8_t tx_buf[64];
ntag_emu_t ntag_emu = { 0 };
static uint8_t volatile comp_write_pending = false;
static uint8_t volatile comp_write_pages = 0;
nrfx_nfct_data_desc_t rx = { .data_size = 64, .p_data = rx_buf };


static void update_ntag_handler(void * p_event_data, uint16_t event_size);

ntag_t* ntag_emu_get_current_tag(){
	return &(ntag_emu.ntag);
}

nrfx_err_t nrfx_nfct_tx_ack_nak1(uint8_t val) {
	nrfx_err_t err;
	nrfx_nfct_data_desc_t tx = { .p_data = tx_buf, .data_size = 1 };
	tx_buf[0] = val;
	return nrfx_nfct_tx_ext(&tx, 0, 4, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
}

nrfx_err_t nrfx_nfct_tx_ack_nak(uint8_t ack_nack_code) {
	static uint8_t Ack;
	Ack = ack_nack_code;

	nrf_nfct_event_clear(NRF_NFCT_EVENT_TXFRAMEEND);

	NRF_NFCT->PACKETPTR = (uint32_t) (&Ack);
	NRF_NFCT->TXD.AMOUNT = 4;

	uint32_t reg = 0;
	/* reg |= (NFCT_TXD_FRAMECONFIG_PARITY_Parity << NFCT_TXD_FRAMECONFIG_PARITY_Pos); */
	reg |= (NFCT_TXD_FRAMECONFIG_SOF_SoF << NFCT_TXD_FRAMECONFIG_SOF_Pos);
	//NRF_LOG_DEBUG("ack_nak: %x", reg);
	NRF_NFCT->TXD.FRAMECONFIG = reg;
	NRF_NFCT->INTENSET = (NFCT_INTENSET_TXFRAMEEND_Enabled
			<< NFCT_INTENSET_TXFRAMEEND_Pos);
	NRF_NFCT->TASKS_STARTTX = 1;

	return NRF_SUCCESS;
}

void nrfx_event_callback(nrfx_nfct_evt_t const *p_event) {
	nrfx_err_t err;
	nrfx_nfct_data_desc_t tx;
	nfc_platform_event_handler(p_event);
	switch (p_event->evt_id) {
	case NRFX_NFCT_EVT_SELECTED: {
		//NRF_LOG_DEBUG("T2T: SELETED");
		nrfx_nfct_rx(&rx);
		break;
	}

	case NRFX_NFCT_EVT_RX_FRAMEEND: {
		//NRF_LOG_DEBUG("T2T: RX END");

		//COMP WRITE DATA
		if (comp_write_pending) {
			if (comp_write_pages * 4 < ntag_emu.ntag.size) {
				memcpy(ntag_emu.ntag.data + comp_write_pages * 4, p_event->params.rx_frameend.rx_data.p_data, 4);
				ntag_emu.dirty = true;
				NRF_LOG_DEBUG("cwde: 0x%0X", comp_write_pages);

				err = nrfx_nfct_tx_ack_nak(0xA);
				APP_ERROR_CHECK(err);

			} else if (comp_write_pages * 4 < (ntag_emu.ntag.size + ECC_SIZE)) {
				uint8_t offset = comp_write_pages * 4 - ntag_emu.ntag.size;
				memcpy(ntag_emu.ntag.ecc + offset, p_event->params.rx_frameend.rx_data.p_data, 4);
				ntag_emu.dirty = true;
				NRF_LOG_DEBUG("cwee: 0x%0X", offset / 4);
				err = nrfx_nfct_tx_ack_nak(0xA);
				APP_ERROR_CHECK(err);
			} else {
				NRF_LOG_DEBUG("comp write overflow", comp_write_pages);
				err = nrfx_nfct_tx_ack_nak(0);
				APP_ERROR_CHECK(err);
			}

			comp_write_pending = false;
			comp_write_pages = 0;

			return;
		}

		uint8_t cmd = p_event->params.rx_frameend.rx_data.p_data[0];
		if (cmd == 0x60) {
			//GET_VERSION
			tx_buf[0] = 0;
			tx_buf[1] = 4;
			tx_buf[2] = 4;
			tx_buf[3] = 2;
			tx_buf[4] = 1;
			tx_buf[5] = 0;
			tx_buf[6] = 0x11;
			tx_buf[7] = 3;

			tx.data_size = 8;
			tx.p_data = tx_buf;

			err = nrfx_nfct_tx(&tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
			APP_ERROR_CHECK(err);
		} else if (cmd == 0x30) { //read
			uint8_t pages = p_event->params.rx_frameend.rx_data.p_data[1];
			if (pages * 4 < ntag_emu.ntag.size) {
				memcpy(tx_buf, ntag_emu.ntag.data + pages * 4, 16);
				tx.data_size = 16;
				tx.p_data = tx_buf;
				err = nrfx_nfct_tx(&tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
				APP_ERROR_CHECK(err);
				NRF_LOG_DEBUG("send page 0x%0X", pages);

			} else {
				NRF_LOG_DEBUG("page overflow");
			}
		} else if (cmd == 0x50) {
			nrfx_nfct_init_substate_force(NRFX_NFCT_ACTIVE_STATE_SLEEP);
			NRF_LOG_DEBUG("go sleep");
		} else if (cmd == 0x3c) { //get sig
			tx.data_size = 32;
			tx.p_data = ntag_emu.ntag.ecc;
			err = nrfx_nfct_tx(&tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
			//err = nrfx_nfct_tx_ack_nak(0);
			APP_ERROR_CHECK(err);
			NRF_LOG_DEBUG("send sig");
		} else if (cmd == 0x1b) { //auth
			tx_buf[0] = 0x80;
			tx_buf[1] = 0x80;
			tx.data_size = 2;
			tx.p_data = tx_buf;
			err = nrfx_nfct_tx(&tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
			APP_ERROR_CHECK(err);
			NRF_LOG_DEBUG("send PACK");
		} else if (cmd == 0x3a) { //fast read
			uint8_t startPage = p_event->params.rx_frameend.rx_data.p_data[1];
			uint8_t endPage = p_event->params.rx_frameend.rx_data.p_data[2];

			tx.data_size = (endPage - startPage + 1) * 4;
			tx.p_data = ntag_emu.ntag.data + startPage * 4;
			err = nrfx_nfct_tx(&tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
			APP_ERROR_CHECK(err);
			NRF_LOG_DEBUG("send fast read 0x%02X,0x%02X", startPage, endPage);
		} else if (cmd == 0xa2) { // write
			uint8_t page = p_event->params.rx_frameend.rx_data.p_data[1];
			uint8_t *data = p_event->params.rx_frameend.rx_data.p_data + 2;

			if (page * 4 >= ntag_emu.ntag.size) {
				NRF_LOG_DEBUG("write pages overflow: 0x%02X", page);
				err = nrfx_nfct_tx_ack_nak(0);
				tx_buf[0] = 0;
				tx.data_size = 1;
				tx.p_data = tx_buf;
				err = nrfx_nfct_tx(&tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
				APP_ERROR_CHECK(err);
				break;
			}
			if (page == 133 || page == 134) {
				NRF_LOG_DEBUG("PWD and pack ignore");
			} else if (page == 2) {
				memcpy(ntag_emu.ntag.data + page * 4 + 2, data + 2, 2); //Writing static lock bytes
			} else {
				memcpy(ntag_emu.ntag.data + page * 4, data, 4);
			}
			tx_buf[0] = 0xA;
			tx.data_size = 1;
			tx.p_data = tx_buf;
			//err = nrfx_nfct_tx(&tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
			err = nrfx_nfct_tx_ack_nak(0xA);
			APP_ERROR_CHECK(err);

			ntag_emu.dirty = true;

			NRF_LOG_DEBUG("write pages done: 0x%02X", page);
		} else if (cmd == 0xa0) { //  COMPATIBILITY_WRITE
			uint8_t page = p_event->params.rx_frameend.rx_data.p_data[1];

			if (page * 4 >= ntag_emu.ntag.size + ECC_SIZE) {
				NRF_LOG_DEBUG("write pages overflow: 0x%02X", page);
				err = nrfx_nfct_tx_ack_nak(0);
				APP_ERROR_CHECK(err);
				break;
			}
			NRF_LOG_DEBUG("cwb: 0x%02X", page);
			comp_write_pages = page;
			comp_write_pending = true;
			err = nrfx_nfct_tx_ack_nak(0xA);
			APP_ERROR_CHECK(err);
		} else {
			NRF_LOG_DEBUG("unknown cmd: 0x%02X", cmd);
			err = nrfx_nfct_tx_ack_nak(0);
			APP_ERROR_CHECK(err);
		}
		break;
	}

	case NRFX_NFCT_EVT_ERROR: {
		NRF_LOG_DEBUG("T2T: ERROR: %d", p_event->params.error.reason);
		nrfx_nfct_init_substate_force(NRFX_NFCT_ACTIVE_STATE_SLEEP);
		break;
	}

	case NRFX_NFCT_EVT_TX_FRAMEEND: {
		nrfx_nfct_rx(&rx);
		break;
	}

	case NRFX_NFCT_EVT_FIELD_LOST: {
		NRF_LOG_DEBUG("T2T: field lost");
		if (ntag_emu.dirty) {
			app_sched_event_put(NULL, 0, update_ntag_handler);
		}
		bsp_board_led_off(0);
		break;
	}

	case NRFX_NFCT_EVT_FIELD_DETECTED: {
		NRF_LOG_DEBUG("T2T: field detected");
		bsp_board_led_on(0);
		break;
	}
	default: {
		NRF_LOG_DEBUG("T2T: UNKNOWN: %d", p_event->evt_id);
		nrfx_nfct_init_substate_force(NRFX_NFCT_ACTIVE_STATE_SLEEP);
		break;
	}
	}

}

static void update_ntag_handler(void * p_event_data, uint16_t event_size) {
	ntag_emu.busy = true;

	// ui_header_show_status(ST_SAVE);
	// ntag_emu_set_tag(&(ntag_emu.ntag));
	// uint8_t index = ui_amiibo_get_index();
	// NRF_LOG_DEBUG("Pesist ntag begin: %d", index);
	// ret_code_t err_code = ntag_store_write_with_gc(index, &(ntag_emu.ntag));
	// APP_ERROR_CHECK(err_code);
	// NRF_LOG_DEBUG("Pesist ntag end: %d", index);
	// ui_amiibo_set_ntag(&(ntag_emu.ntag));
	// ui_header_hide_status(ST_SAVE);

	ntag_emu.dirty = false;
	ntag_emu.busy = false;

}

ret_code_t ntag_emu_init(ntag_t *ntag) {

	nfc_platform_setup();

	nrfx_nfct_config_t config = { .rxtx_int_mask = NRFX_NFCT_EVT_SELECTED
			| NRFX_NFCT_EVT_RX_FRAMEEND | NRFX_NFCT_EVT_ERROR
			| NRFX_NFCT_EVT_TX_FRAMEEND | NRFX_NFCT_EVT_FIELD_LOST
			| NRFX_NFCT_EVT_FIELD_DETECTED, .cb = nrfx_event_callback };

	nrfx_err_t nrfx_error = nrfx_nfct_init(&config);
	APP_ERROR_CHECK(nrfx_error);

	ntag_emu_set_tag(ntag);

	nrfx_nfct_param_t param = { .id = NRFX_NFCT_PARAM_ID_SEL_RES,
			param.data.sel_res_protocol = NRF_NFCT_SELRES_PROTOCOL_T2T };

	nrfx_nfct_parameter_set(&param);

	nrfx_nfct_autocolres_enable();

	nrfx_nfct_enable();


	return NRF_SUCCESS;
}

void ntag_emu_uninit(ntag_t *ntag) {
	nrfx_nfct_disable();
	nrfx_nfct_uninit();
}

void ntag_emu_set_tag(ntag_t *ntag) {
	memcpy(&(ntag_emu.ntag), ntag, sizeof(ntag_t));
	ntag_emu.dirty = 0;

	uint8_t uid1[7];
	uid1[0] = ntag->data[0];
	uid1[1] = ntag->data[1];
	uid1[2] = ntag->data[2];
	uid1[3] = ntag->data[4];
	uid1[4] = ntag->data[5];
	uid1[5] = ntag->data[6];
	uid1[6] = ntag->data[7];

	nrfx_nfct_param_t param = { .id = NRFX_NFCT_PARAM_ID_NFCID1,
			.data.nfcid1.id_size = 7, .data.nfcid1.p_id = uid1 };
	nrfx_nfct_parameter_set(&param);
}

void ntag_emu_set_uuid_only(ntag_t* ntag){
	uint8_t uid1[7];
		uid1[0] = ntag->data[0];
		uid1[1] = ntag->data[1];
		uid1[2] = ntag->data[2];
		uid1[3] = ntag->data[4];
		uid1[4] = ntag->data[5];
		uid1[5] = ntag->data[6];
		uid1[6] = ntag->data[7];

		nrfx_nfct_param_t param = { .id = NRFX_NFCT_PARAM_ID_NFCID1,
				.data.nfcid1.id_size = 7, .data.nfcid1.p_id = uid1 };
		nrfx_nfct_parameter_set(&param);
}

