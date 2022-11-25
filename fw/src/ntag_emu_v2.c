/*
 * t2t_emu.c
 *
 *  Created on: 2021年5月30日
 *      Author: solos
 */

#include "ntag_emu.h"

#ifdef NTAG_EMU_V2
#include "app_scheduler.h"
#include "app_timer.h"
#include "boards.h"
#include "nfc_platform.h"
#include "nrf52.h"
#include "nrf52_bitfields.h"
#include "nrf_log.h"
#include "nrfx.h"

#include "hal_nfc_t2t.h"

#include "ntag_indicator.h"

typedef enum
{
    NFC_T2T_PARAM_TESTING,      ///< Used for unit tests.
    NFC_T2T_PARAM_NFCID1,       /**< NFCID1 value, data can be 4, 7, or 10 bytes long (single, double, or triple size).
                                     To use default NFCID1 of specific length pass one byte containing requested length.
                                     Default 7-byte NFCID1 will be used if this parameter was not set. This parameter can be
                                     set before nfc_t2t_setup() to set initial NFCID1 and it can be changed later. */
} nfc_t2t_param_id_t;

typedef struct {
    ntag_t ntag;
    uint8_t dirty;
    uint8_t busy;
} ntag_emu_t;

ntag_emu_t ntag_emu = { 0 };

static const uint8_t NTAG215_Version[8] = {0x00, 0x04, 0x04, 0x02,
                                           0x01, 0x00, 0x11, 0x03};

static const uint8_t NTAG215_Signature[32] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const uint8_t NTAG215_PwdOK[2] = {
    0x80,
    0x80,
};

#define NFC_CMD_READ 0x30
#define NFC_CMD_WRITE 0xA2
#define NFC_CMD_GET_VERSION 0x60
#define NFC_CMD_READ_SIG 0x3C
#define NFC_CMD_PWD_AUTH 0x1B
#define NFC_CMD_FAST_READ 0x3A

static void update_ntag_handler(void *p_event_data, uint16_t event_size);

static void nfc_received_process(const uint8_t *p_data, size_t data_length,
                                 uint8_t *plain) {
    // NRF_LOG_INFO("NFC COMMAND %d", p_data[0]);

	nrf_pwr_mgmt_feed();

    uint8_t command = p_data[0];
    uint8_t block_num = p_data[1];

    // NRF_LOG_DEBUG("")

    switch (command) {
    case NFC_CMD_READ:
        NRF_LOG_INFO("NFC Read Block %d", block_num);
        if (block_num < 135)
            hal_nfc_send(&plain[block_num * 4], 16);
        else
            hal_send_ack_nack(0x0);
        break;
    case NFC_CMD_WRITE:
        NRF_LOG_INFO("NFC Write Block %d", block_num);
        if (data_length == 6) {
			ntag_emu.dirty = true;
            if (block_num == 133 || block_num == 134) {

            } else if (block_num == 2) {
                plain[block_num * 4 + 2] = p_data[4];
                plain[block_num * 4 + 3] = p_data[5];
            } else {

                for (int i = 0; i < 4; i++) {
                    plain[block_num * 4 + i] = p_data[i + 2];
                }
            }
            hal_send_ack_nack(0xA);
        } else {

            hal_send_ack_nack(0x0);
        }
        break;
    case NFC_CMD_GET_VERSION:
        NRF_LOG_INFO("NFC Get Version");
        hal_nfc_send(NTAG215_Version, 8);
        break;

    case NFC_CMD_READ_SIG:
        NRF_LOG_INFO("NFC Read Signature");
        hal_nfc_send(NTAG215_Signature, 32);
        break;

    case NFC_CMD_FAST_READ:
        NRF_LOG_INFO("NFC Fast Read %d to %d", block_num, p_data[2]);
        hal_nfc_send(&plain[block_num * 4], (p_data[2] - block_num + 1) * 4);
        break;

    case NFC_CMD_PWD_AUTH:
        NRF_LOG_INFO("NFC Password: %x %x %x %x", p_data[1], p_data[2], p_data[3],
                     p_data[4]);
        hal_nfc_send(NTAG215_PwdOK, 2);
        break;

    default:
        NRF_LOG_INFO("NFC CMD %x", p_data[0]);
        break;
    }
}

/**
 * @brief Callback function for handling NFC events.
 */

static void nfc_callback(void *p_context, hal_nfc_event_t event, const uint8_t *p_data,
                         size_t data_length) {
    (void)p_context;

    switch (event) {
    case HAL_NFC_EVENT_FIELD_ON:
        bsp_board_led_on(BSP_BOARD_LED_0);
        break;
    case HAL_NFC_EVENT_FIELD_OFF:
        bsp_board_led_off(BSP_BOARD_LED_0);
		if (ntag_emu.dirty) {
			app_sched_event_put(NULL, 0, update_ntag_handler);
		}
        break;
    case HAL_NFC_EVENT_COMMAND:
        // NRF_LOG_INFO("NFC Command Received: %x", p_data[0]);
        nfc_received_process(p_data, data_length, ntag_emu.ntag.data);
        break;

    case HAL_NFC_EVENT_DATA_TRANSMITTED:
        // NRF_LOG_INFO("NFC EVENT Data Transmitted");
        break;

    default:
        // NRF_LOG_INFO("NFC EVENT %d, CMD %x", event, p_data[0]);
        break;
    }
}



ntag_t *ntag_emu_get_current_tag() { return &(ntag_emu.ntag); }

static void update_ntag_handler(void *p_event_data, uint16_t event_size) {
    ntag_emu.busy = true;

    ntag_emu_set_tag(&(ntag_emu.ntag));

    uint8_t index = ntag_indicator_current();
    NRF_LOG_DEBUG("Pesist ntag begin: %d", index);
    ret_code_t err_code = ntag_store_write_with_gc(index, &(ntag_emu.ntag));
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("Pesist ntag end: %d", index);
    ntag_indicator_update();

    ntag_emu.dirty = false;
    ntag_emu.busy = false;
}

ret_code_t ntag_emu_init(ntag_t *ntag) {
    uint32_t err_code;

    err_code = hal_nfc_setup(nfc_callback, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = hal_nfc_start();
    APP_ERROR_CHECK(err_code);

    ntag_emu_set_tag(ntag);
    return NRF_SUCCESS;
}

void ntag_emu_uninit(ntag_t *ntag) {
    //TODO
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

    hal_nfc_parameter_set(NFC_T2T_PARAM_NFCID1, uid1, sizeof(uid1));
}

void ntag_emu_set_uuid_only(ntag_t *ntag) {
    uint8_t uid1[7];
    uid1[0] = ntag->data[0];
    uid1[1] = ntag->data[1];
    uid1[2] = ntag->data[2];
    uid1[3] = ntag->data[4];
    uid1[4] = ntag->data[5];
    uid1[5] = ntag->data[6];
    uid1[6] = ntag->data[7];

  hal_nfc_parameter_set(NFC_T2T_PARAM_NFCID1, uid1, sizeof(uid1));
}

#endif