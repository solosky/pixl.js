/**
 * Copyright (c) 2017 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @file
 *
 * @defgroup nfc_writable_ndef_msg_example_main main.c
 * @{
 * @ingroup nfc_writable_ndef_msg_example
 * @brief The application main file of NFC writable NDEF message example.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "app_error.h"
#include "app_scheduler.h"
#include "boards.h"
#include "nfc_t4t_lib.h"
#include "nrf_log_ctrl.h"
#include "nfc_ndef_msg.h"
#include "nrf_error.h"
#include "nordic_common.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_button.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "bsp.h"
#include "bsp_nfc.h"

#include "ntag_emu.h"
#include "ntag_store.h"
#include "ntag_indicator.h"


#include "amiibo_data.h"

#include "bat.h"
#include "ble_main.h"

#include "nrf_crypto.h"
#include "mem_manager.h"

#include "nrf_pwr_mgmt.h"

#include "u8g2_drv.h"

#include "amiibo_helper.h"

#define APP_SCHED_MAX_EVENT_SIZE 4                  /**< Maximum size of scheduler events. */
#define APP_SCHED_QUEUE_SIZE     16                  /**< Maximum number of events in the scheduler queue. */

#define BTN_ID_SLEEP                1 /**< ID of button used to put the application into sleep mode. */
#define BTN_ACTION_KEY1_LONGPUSH  BSP_EVENT_KEY_LAST + 9

#define APP_SHUTDOWN_HANDLER_PRIORITY 1


/**
 *@brief Function for initializing logging.
 */
static void log_init(void) {
	ret_code_t err_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(err_code);

	NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void bsp_evt_execute(void * p_event_data, uint16_t event_size) {

	bsp_event_t* evt_p = (bsp_event_t*) p_event_data;
	bsp_event_t evt = *evt_p;
	NRF_LOG_DEBUG("bsp event: %d\n", evt);
	switch (evt) {

	case BSP_EVENT_KEY_0: {

		nrf_pwr_mgmt_feed();

		NRF_LOG_DEBUG("Key 0 press");
		ntag_t ntag;
		ret_code_t err_code = NRF_SUCCESS;

		uint8_t index = ntag_indicator_switch_prev();
		err_code = ntag_store_read_default(index, &ntag);
		APP_ERROR_CHECK(err_code);
		ntag_emu_set_tag(&ntag);
		APP_ERROR_CHECK(err_code);

		ntag_indicator_update();

		

		break;
	}

	case BSP_EVENT_KEY_1: {

		nrf_pwr_mgmt_feed();

		NRF_LOG_DEBUG("Key 1 press");

		//regenerate

		ret_code_t err_code;
	ntag_t ntag_new;
	ntag_t *ntag_current = ntag_emu_get_current_tag();
	memcpy(&ntag_new, ntag_current, sizeof(ntag_t));

	uint32_t head = to_little_endian_int32(&ntag_current->data[84]);
	uint32_t tail = to_little_endian_int32(&ntag_current->data[88]);

	const amiibo_data_t *amd = find_amiibo_data(head, tail);
	if(amd == NULL){
		return;
	}

    NRF_LOG_INFO("reset uuid begin");

	err_code = ntag_store_uuid_rand(&ntag_new);
    APP_ERROR_CHECK(err_code);


    //sign new
    err_code = amiibo_helper_sign_new_ntag(ntag_current, &ntag_new);
	if (err_code == NRF_SUCCESS) {
		//ntag_emu_set_uuid_only(&ntag_new);
		ntag_emu_set_tag(&ntag_new);
		ntag_indicator_update();

		NRF_LOG_INFO("reset uuid success");
	}


		break;
	}

	case BSP_EVENT_KEY_2: {

		nrf_pwr_mgmt_feed();

		NRF_LOG_DEBUG("Key 2 press");
		ntag_t ntag;
		ret_code_t err_code = NRF_SUCCESS;

		uint8_t index = ntag_indicator_switch_next();
		err_code = ntag_store_read_default(index, &ntag);
		APP_ERROR_CHECK(err_code);
		ntag_emu_set_tag(&ntag);
		APP_ERROR_CHECK(err_code);

		ntag_indicator_update();

		break;
	}

	case BTN_ACTION_KEY1_LONGPUSH:{

		nrf_pwr_mgmt_feed();

		NRF_LOG_DEBUG("Key 1 long press");
		ret_code_t err_code = NRF_SUCCESS;

		ntag_t *ntag = ntag_emu_get_current_tag();
		uint8_t index = ntag_indicator_current();
		err_code = ntag_store_reset(index, ntag);
		APP_ERROR_CHECK(err_code);
		ntag_emu_set_tag(ntag);
		APP_ERROR_CHECK(err_code);

		ntag_indicator_update();

		NRF_LOG_DEBUG("reset tag");

		break;

	}

	case BSP_EVENT_SLEEP: {
		// // Set up NFCT peripheral as the only wake up source.
		// ret_code_t err_code = NRF_SUCCESS;
		// err_code = bsp_nfc_sleep_mode_prepare();
		// //APP_ERROR_CHECK(err_code);
		// //err_code = bsp_buttons_disable();
		// APP_ERROR_CHECK(err_code);
		// err_code = bsp_wakeup_button_enable(BTN_ID_SLEEP);
		// APP_ERROR_CHECK(err_code);
		// // Turn off LED to indicate that the device entered System OFF mode.
		// bsp_board_leds_off();
		break;
	}
	default:
		break;
	}

}

void bsp_evt_handler(bsp_event_t evt) {
	app_sched_event_put(&evt, sizeof(evt), bsp_evt_execute);
}

/**
 * @brief Function for shutdown events.
 *
 * @param[in]   event       Shutdown type.
 */
static bool shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    ret_code_t err_code;

    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
            // Set up NFCT peripheral as the only wake up source.
			NRF_LOG_DEBUG("go sleep");
			u8g2_drv_deinit();
			err_code = bsp_wakeup_button_enable(BTN_ID_SLEEP);
			APP_ERROR_CHECK(err_code);

			// err_code = bsp_btn_ble_sleep_mode_prepare();
			// APP_ERROR_CHECK(err_code);
			err_code = bsp_nfc_sleep_mode_prepare();
			//APP_ERROR_CHECK(err_code);
			//err_code = bsp_buttons_disable();
			APP_ERROR_CHECK(err_code);

			// Turn off LED to indicate that the device entered System OFF mode.
			bsp_board_leds_off();
			
            break;

        default:
            break;
    }

    return true;
}

NRF_PWR_MGMT_HANDLER_REGISTER(shutdown_handler, APP_SHUTDOWN_HANDLER_PRIORITY);


/**
 * @brief   Function for application main entry.
 */
int main(void) {
	ret_code_t err_code;


	log_init();


	APP_SCHED_INIT(APP_SCHED_MAX_EVENT_SIZE, APP_SCHED_QUEUE_SIZE);

	err_code = nrf_drv_clock_init();
	APP_ERROR_CHECK(err_code);
	nrf_drv_clock_lfclk_request(NULL);

	// Initialize timer module, making it use the scheduler
	err_code = app_timer_init();
	APP_ERROR_CHECK(err_code);

	/* Configure LED-pins as outputs */
	err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_evt_handler);
	APP_ERROR_CHECK(err_code);

	// err_code = bsp_nfc_btn_init(BTN_ID_SLEEP);
	// APP_ERROR_CHECK(err_code);


	err_code = bsp_event_to_button_action_assign(1, BSP_BUTTON_ACTION_LONG_PUSH, BTN_ACTION_KEY1_LONGPUSH);
	APP_ERROR_CHECK(err_code);

    err_code = nrf_crypto_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_mem_init();
    APP_ERROR_CHECK(err_code);

	err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);


	u8g2_drv_init();

	err_code = ble_init();
	APP_ERROR_CHECK(err_code);
	NRF_LOG_DEBUG("ble init done");

	
	extern const uint8_t amiibo_key_retail[];
	amiibo_helper_load_keys(amiibo_key_retail); 

	NRF_LOG_DEBUG("init done");


	err_code = ntag_store_init();
	APP_ERROR_CHECK(err_code);

	ntag_t ntag;
	uint8_t index = ntag_indicator_current();
	err_code = ntag_store_read_default(index, &ntag);
	APP_ERROR_CHECK(err_code);
	err_code = ntag_emu_init(&ntag);
	APP_ERROR_CHECK(err_code);

	ntag_indicator_update();

	bat_level_t bat_level = bat_get_level();

	NRF_LOG_DEBUG("display done");



	NRF_LOG_FLUSH();

	while (1) {


		app_sched_execute();

		NRF_LOG_FLUSH();
		nrf_pwr_mgmt_run();

		/*if (poweroff_mode) {
		 NRF_LOG_DEBUG("go power off");
		 NRF_LOG_FLUSH();
		 nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
		 }*/
	}
}

/** @} */
