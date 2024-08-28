/**
 * Copyright (c) 2014 - 2020, Nordic Semiconductor ASA
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
 * @defgroup ble_sdk_uart_over_ble_main main.c
 * @{
 * @ingroup  ble_sdk_app_nus_eval
 * @brief    UART over BLE application main file.
 *
 * This file contains the source code for a sample application that uses the Nordic UART service.
 * This application uses the @ref srvlib_conn_params module.
 */

#include "app_timer.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_hci.h"
#include "ble_nus.h"
#include "bsp_btn_ble.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include <stdint.h>
#include <string.h>

#if defined(UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined(UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ble_amiibolink.h"
#include "ble_df_driver.h"
#include "ble_main2.h"
#include "df_core.h"
#include "nrf_pwr_mgmt.h"

#include "app_scheduler.h"

#define APP_BLE_CONN_CFG_TAG 1 /**< A tag identifying the SoftDevice BLE configuration. */

#define NUS_SERVICE_UUID_TYPE                                                                                          \
    BLE_UUID_TYPE_VENDOR_BEGIN /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_BLE_OBSERVER_PRIO 3 /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define APP_ADV_INTERVAL 64 /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */

#define APP_ADV_DURATION 18000 /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL                                                                                              \
    MSEC_TO_UNITS(                                                                                                     \
        20,                                                                                                            \
        UNIT_1_25_MS) /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL                                                                                              \
    MSEC_TO_UNITS(                                                                                                     \
        75,                                                                                                            \
        UNIT_1_25_MS)   /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY 0 /**< Slave latency. */
#define CONN_SUP_TIMEOUT                                                                                               \
    MSEC_TO_UNITS(                                                                                                     \
        4000, UNIT_10_MS) /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY                                                                                 \
    APP_TIMER_TICKS(5000) /**< Time from initiating event (connect or start of notification) to first time             \
                             sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY                                                                                  \
    APP_TIMER_TICKS(                                                                                                   \
        30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT                                                                                   \
    3 /**< Number of attempts before giving up the connection parameter negotiation.                                   \
       */

#define DEAD_BEEF                                                                                                      \
    0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */



BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT); /**< BLE NUS service instance. */
NRF_BLE_GATT_DEF(m_gatt);                         /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                           /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);               /**< Advertising module instance. */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */
static uint16_t m_ble_nus_max_data_len =
    BLE_GATT_ATT_MTU_DEFAULT -
    3; /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
static ble_uuid_t m_adv_uuids[] = /**< Universally unique service identifier. */
    {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};

static nus_rx_data_handler_t m_nus_rx_data_handler =
    NULL; /**< Event handler to be called for handling received packets. */
static nus_tx_ready_handler_t m_nus_tx_ready_handler =
    NULL; /**< Event handler to be called for handling transmitted packets. */

#define DEVICE_NAME_PIXLJS "Pixl.js"
#define DEVICE_NAME_AMIIBOLINK "amiibolink"
#define DEVICE_NAME_AMILOOP "AmiLoop"

#define DEVICE_NAME DEVICE_NAME_PIXLJS

static ble_gap_addr_t m_default_gap_addr = {0};

static uint8_t m_ble_initialized = false;

#define BLE_CONNECT_SLEEP_TIMEOUT_SECONDS 180 /**< Sleep timeout when ble connection established. */
uint16_t m_previous_pwr_mgmt_sleep_timeout = BLE_CONNECT_SLEEP_TIMEOUT_SECONDS;

/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for initializing the timer module.
 */
static void timers_init(void) {
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void) {
    uint32_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);

    sd_ble_gap_addr_get(&m_default_gap_addr);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error) { APP_ERROR_HANDLER(nrf_error); }

static void nus_event_async_call_rx_data(void *p_event_data, uint16_t event_size) {
    // NRF_LOG_INFO("nus_event_async_call_rx_data: bytes=%d", event_size);
    if (m_nus_rx_data_handler) {
        m_nus_rx_data_handler(p_event_data, event_size);
    }
}

static void nus_event_async_call_tx_ready(void *p_event_data, uint16_t event_size) {
    if (m_nus_tx_ready_handler) {
        m_nus_tx_ready_handler();
    }
}

/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_evt       Nordic UART Service event.
 */
/**@snippet [Handling the data received over BLE] */
static void nus_data_handler(ble_nus_evt_t *p_evt) {
    nrf_pwr_mgmt_feed();
    if (p_evt->type == BLE_NUS_EVT_RX_DATA) {
        if (m_nus_rx_data_handler) {
            ret_code_t err = app_sched_event_put(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length,
                                nus_event_async_call_rx_data);
            APP_ERROR_CHECK(err);
            //nus_event_async_call_rx_data(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);
        }

    } else if (p_evt->type == BLE_NUS_EVT_TX_RDY) {
        if (m_nus_tx_ready_handler) {
            ret_code_t err = app_sched_event_put(NULL, 0, nus_event_async_call_tx_ready);
            APP_ERROR_CHECK(err);
            //nus_event_async_call_tx_ready(NULL, 0);
        }
    }
}

/**@snippet [Handling the data received over BLE] */

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void) {
    uint32_t err_code;
    ble_nus_init_t nus_init;
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize NUS.
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt) {
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error) { APP_ERROR_HANDLER(nrf_error); }

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void) {
    uint32_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = on_conn_params_evt;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt) {
    uint32_t err_code;

    switch (ble_adv_evt) {
    case BLE_ADV_EVT_FAST:
        err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
        APP_ERROR_CHECK(err_code);
        break;
    case BLE_ADV_EVT_IDLE:
        bsp_indication_set(BSP_INDICATE_IDLE);
        break;
    default:
        break;
    }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
    uint32_t err_code;
    char device_name[35];
    uint32_t device_name_len = 35;

    switch (p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
        NRF_LOG_INFO("Connected");
        err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
        APP_ERROR_CHECK(err_code);
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
        APP_ERROR_CHECK(err_code);

        err_code = sd_ble_gap_device_name_get(device_name, &device_name_len);
        APP_ERROR_CHECK(err_code);

        if (strcmp(device_name, DEVICE_NAME_AMILOOP) == 0) {
            NRF_LOG_INFO("Set mtu to %d", NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
            sd_ble_gattc_exchange_mtu_request(p_ble_evt->evt.gap_evt.conn_handle, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
        }

        m_previous_pwr_mgmt_sleep_timeout = nrf_pwr_mgmt_get_timeout();
        nrf_pwr_mgmt_set_timeout(BLE_CONNECT_SLEEP_TIMEOUT_SECONDS);

        break;

    case BLE_GAP_EVT_DISCONNECTED:
        NRF_LOG_INFO("Disconnected");
        // LED indication will be changed when advertising starts.
        m_conn_handle = BLE_CONN_HANDLE_INVALID;
        nrf_pwr_mgmt_set_timeout(m_previous_pwr_mgmt_sleep_timeout);

        break;

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
        NRF_LOG_DEBUG("PHY update request.");
        ble_gap_phys_t const phys = {
            .rx_phys = BLE_GAP_PHY_AUTO,
            .tx_phys = BLE_GAP_PHY_AUTO,
        };
        err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        APP_ERROR_CHECK(err_code);
    } break;

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        // Pairing not supported
        err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        // No system attributes have been stored.
        err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTC_EVT_TIMEOUT:
        // Disconnect on GATT Client timeout event.
        err_code =
            sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        // Disconnect on GATT Server timeout event.
        err_code =
            sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    default:
        // No implementation needed.
        break;
    }
}

/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void) {
    ret_code_t err_code;

    if (!nrf_sdh_is_enabled()) {
        err_code = nrf_sdh_enable_request();
        APP_ERROR_CHECK(err_code);
    }

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t *p_gatt, nrf_ble_gatt_evt_t const *p_evt) {
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)) {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
    NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x", p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
}

/**@brief Function for initializing the GATT library. */
void gatt_init(void) {
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
}

/**@snippet [Handling the data received over UART] */

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void) {
    uint32_t err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.srdata.uuids_complete.p_uuids = m_adv_uuids;

    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout = APP_ADV_DURATION;
    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Function for starting advertising.
 */
static void advertising_start(void) {
    m_advertising.adv_modes_config.ble_adv_on_disconnect_disabled = false;
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}

/**@brief Application main function.
 */
void ble_init(void) {

    if (!m_ble_initialized) {
        ble_stack_init();
        gap_params_init();
        gatt_init();
        services_init();
        conn_params_init();
        df_core_init();
        m_ble_initialized = true;
    }

    NRF_LOG_INFO("BLE started.");
}

void ble_set_device_name(const char *device_name) {
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    uint32_t err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)device_name, strlen(device_name));
    APP_ERROR_CHECK(err_code);
}

void ble_addr_set(uint8_t offset) {
    ble_gap_addr_t addr = {0};
    memcpy(&addr, &m_default_gap_addr, sizeof(ble_gap_addr_t));
    addr.addr[0] += offset;
    uint32_t err_code = sd_ble_gap_addr_set(&addr);
    APP_ERROR_CHECK(err_code);
}

void ble_service_id_set(uint32_t suffix) {
    uint32_t *ptr = m_advertising.p_adv_data->scan_rsp_data.p_data
                    + m_advertising.p_adv_data->scan_rsp_data.len - 4;
    *ptr = suffix;
}

void ble_device_mode_prepare(ble_device_mode_t mode) {
    if (mode == BLE_DEVICE_MODE_AMIIBOLINK || mode == BLE_DEVICE_MODE_AMIIBOLINK_V2) {
        ble_set_device_name(DEVICE_NAME_AMIIBOLINK);
        ble_addr_set(0x10);
    } else if (mode == BLE_DEVICE_MODE_AMILOOP) {
        ble_set_device_name(DEVICE_NAME_AMILOOP);
        ble_addr_set(0x20);
    } else {
        ble_set_device_name(DEVICE_NAME_PIXLJS);
        ble_addr_set(0);
    }
    advertising_init();

    ble_service_id_set(mode == BLE_DEVICE_MODE_AMIIBOLINK_V2 ? 0x0000180F : 0x6e400001);
}

void ble_adv_start(void) {
    if (m_ble_initialized && m_advertising.adv_mode_current == BLE_ADV_MODE_IDLE) {
        advertising_start();
    }
}

void ble_disable() {

    ble_advertising_start(&m_advertising, BLE_ADV_MODE_IDLE);
    sd_ble_gap_adv_stop(m_advertising.adv_handle);
    m_advertising.adv_modes_config.ble_adv_on_disconnect_disabled = true;
    if (m_conn_handle != BLE_CONN_HANDLE_INVALID) {
        sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    }
}

void ble_nus_set_handler(nus_rx_data_handler_t rx_data_handler, nus_tx_ready_handler_t tx_ready_handler) {
    m_nus_rx_data_handler = rx_data_handler;
    m_nus_tx_ready_handler = tx_ready_handler;
}

ret_code_t ble_nus_tx_data(void *data, size_t length) {
    size_t tx_length = length;
    NRF_LOG_INFO("ble_nus_tx_data: %d bytes", tx_length);
    ret_code_t err_code = ble_nus_data_send(&m_nus, data, &tx_length, m_conn_handle);
    if (err_code) {
        NRF_LOG_INFO("send back error:%d", err_code);
    }
    return err_code;
}

void ble_get_addr_str(char addr_fmt[]) {
    ble_gap_addr_t addr = {0};
    sd_ble_gap_addr_get(&addr);
    sprintf(addr_fmt, "%02X:%02X:%02X:%02X:%02X:%02X", addr.addr[5], addr.addr[4], addr.addr[3], addr.addr[2],
            addr.addr[1], addr.addr[0]);
}