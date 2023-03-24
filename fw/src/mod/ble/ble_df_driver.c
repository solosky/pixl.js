#include "ble_df_driver.h"
#include "ble_main.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "stddef.h"

static df_event_handler_t m_df_event_handler;
static df_frame_t m_rx_df_frame;

int32_t df_ble_driver_init(df_event_handler_t cb) { m_df_event_handler = cb; }
int32_t df_ble_driver_send(df_frame_t *df) { ble_nus_tx_data(df, DF_HEADER_LEN + df->length); }

df_driver_t ble_df_driver = {.init = df_ble_driver_init, .send = df_ble_driver_send};

df_driver_t *ble_get_df_driver() { return &ble_df_driver; }

void ble_on_received_data(const uint8_t *data, size_t length) {
    NRF_LOG_INFO("df_frame_size: %d", sizeof(m_rx_df_frame)); 
    NRF_LOG_INFO("ble data received %d bytes", length);
    //NRF_LOG_HEXDUMP_INFO(data, length);
    NRF_LOG_FLUSH();

    if (length >= DF_HEADER_LEN) {
        memcpy(&m_rx_df_frame, data, length);
        m_rx_df_frame.length = length - DF_HEADER_LEN;

        if (m_df_event_handler) {
            df_event_t evt = {.df = &m_rx_df_frame, .type = DF_EVENT_DATA_RECEVIED};
            m_df_event_handler(&evt);
        }
    }
}

void ble_on_transmit_ready() {
    NRF_LOG_INFO("ble  transmit ready!");
    if (m_df_event_handler) {
        df_event_t evt = {.type = DF_EVENT_DATA_TRANSMIT_READY};
        m_df_event_handler(&evt);
    }
}