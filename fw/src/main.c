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

#include "app_error.h"
#include "app_scheduler.h"
#include "boards.h"
#include "nfc_ndef_msg.h"
#include "nfc_t4t_lib.h"
#include "nordic_common.h"
#include "nrf_error.h"
#include "nrf_log_ctrl.h"
#include <stdbool.h>
#include <stdint.h>

#include "app_button.h"
#include "app_timer.h"
#include "bsp.h"
#include "bsp_nfc.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ntag_emu.h"
#include "ntag_indicator.h"
#include "ntag_store.h"

#include "amiibo_data.h"

#include "bat.h"
#include "ble_main.h"

#include "nrf_sdh.h"

#include "mem_manager.h"
#include "nrf_crypto.h"

#include "nrf_pwr_mgmt.h"

#include "u8g2_drv.h"

#include "amiibo_helper.h"

#include "mini_app_launcher.h"
#include "mui_include.h"

#include "hal_spi_bus.h"
#include "hal_spi_flash.h"

#include "lfs_port.h"

#define APP_SCHED_MAX_EVENT_SIZE 4 /**< Maximum size of scheduler events. */
#define APP_SCHED_QUEUE_SIZE 16 /**< Maximum number of events in the scheduler queue. */

#define BTN_ID_SLEEP 1 /**< ID of button used to put the application into sleep mode. */
#define BTN_ACTION_KEY1_LONGPUSH BSP_EVENT_KEY_LAST + 9

#define APP_SHUTDOWN_HANDLER_PRIORITY 1

//#define SPI_FLASH

/**
 *@brief Function for initializing logging.
 */
static void log_init(void) {
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void bsp_evt_execute(void *p_event_data, uint16_t event_size) {

    bsp_event_t *evt_p = (bsp_event_t *)p_event_data;
    bsp_event_t evt = *evt_p;
    NRF_LOG_DEBUG("bsp event: %d\n", evt);
    mui_input_on_bsp_event(evt);

    switch (evt) {
    case BSP_EVENT_KEY_0:
    case BSP_EVENT_KEY_1:
    case BSP_EVENT_KEY_2:
    case BTN_ACTION_KEY1_LONGPUSH:
        nrf_pwr_mgmt_feed();
        break;
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
static bool shutdown_handler(nrf_pwr_mgmt_evt_t event) {
    ret_code_t err_code;

    switch (event) {
    case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
        // Set up NFCT peripheral as the only wake up source.
        NRF_LOG_DEBUG("go sleep");
        mui_deinit(mui());
        hal_spi_flash_sleep();
        err_code = bsp_wakeup_button_enable(BTN_ID_SLEEP);
        APP_ERROR_CHECK(err_code);

        // err_code = bsp_btn_ble_sleep_mode_prepare();
        // APP_ERROR_CHECK(err_code);
        err_code = bsp_nfc_sleep_mode_prepare();
        // APP_ERROR_CHECK(err_code);
        // err_code = bsp_buttons_disable();
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

    err_code = bsp_event_to_button_action_assign(1, BSP_BUTTON_ACTION_LONG_PUSH,
                                                 BTN_ACTION_KEY1_LONGPUSH);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_crypto_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_mem_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

    hal_spi_bus_init();

    //u8g2_drv_init();

#ifdef SPI_FLASH
    hal_spi_flash_init();

    // #endif
    // #ifdef LFS_PORT_INTERNAL_FLASH

    err_code = lfs_port_init();
    APP_ERROR_CHECK(err_code);

#endif

    // enable sd to enable pwr mgmt
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // enable dcdc
    //  err_code = sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
    //  APP_ERROR_CHECK(err_code);

    err_code = sd_power_pof_threshold_set(NRF_POWER_THRESHOLD_V18);
    APP_ERROR_CHECK(err_code);

    err_code = sd_power_pof_enable(true);
    APP_ERROR_CHECK(err_code);

    // err_code = ble_init();
    // APP_ERROR_CHECK(err_code);
    // NRF_LOG_DEBUG("ble init done");

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

    // ntag_indicator_update();

    // NRF_LOG_DEBUG("display done");


    mui_t *p_mui = mui();
    mui_init(p_mui);

    mini_app_launcher_t *p_launcher = mini_app_launcher();
    mini_app_launcher_init(p_launcher);

    NRF_LOG_FLUSH();

    while (1) {

        app_sched_execute();
        mui_tick(p_mui);
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
