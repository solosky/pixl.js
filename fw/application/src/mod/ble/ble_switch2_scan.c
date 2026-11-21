#include "ble_main.h"

#include "app_scheduler.h"
#include "nrf.h"
#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh_soc.h"
#include "nrf_soc.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define SCAN_SOC_OBSERVER_PRIO 1
#define SCAN_TIMESLOT_LEN_US   50000UL // 50 ms per slot
#define SCAN_RX_WINDOW_US      47000UL // 47 ms RX window (3 ms safety margin)
#define SCAN_REQ_TIMEOUT_US    100000UL

static const uint8_t k_adv_channels[3] = {37, 38, 39};
static const uint8_t k_adv_freqs[3] = {2, 26, 80}; // 2402, 2426, 2480 MHz

static volatile bool m_scan_active = false;
static volatile bool m_scan_stop_req = false;
static volatile bool m_scan_captured = false;
static volatile uint16_t m_scan_slots_left = 0;
static volatile uint8_t m_scan_ch_idx = 0;

static ble_switch2_scan_cb_t m_scan_cb = NULL;
static ble_switch2_scan_result_t m_scan_result;
static uint8_t m_rx_buf[68];

static nrf_radio_signal_callback_return_param_t m_ret_param;
static nrf_radio_request_t m_timeslot_req;

static void scan_notify_sched_handler(void *p_event_data, uint16_t event_size) {
    (void)p_event_data;
    (void)event_size;
    ble_switch2_scan_cb_t cb = m_scan_cb;
    bool found = m_scan_captured;
    m_scan_cb = NULL;
    m_scan_active = false;
    if (cb) {
        cb(found, found ? &m_scan_result : NULL);
    }
}

static bool parse_rx_packet(const uint8_t *buf) {
    uint8_t pdu_type = buf[0] & 0x0F;
    // Accept ADV_IND (0), ADV_NONCONN_IND (2), ADV_SCAN_IND (6)
    if (pdu_type != 0x00 && pdu_type != 0x02 && pdu_type != 0x06) {
        return false;
    }

    uint8_t pdu_len = buf[1];
    if (pdu_len < 18 || pdu_len > 37) {
        return false;
    }

    const uint8_t *adva = &buf[2];
    const uint8_t *adv = &buf[8];
    uint8_t adv_len = (uint8_t)(pdu_len - 6);

    uint8_t pos = 0;
    while (pos < adv_len) {
        uint8_t field_len = adv[pos];
        if (field_len == 0 || (uint16_t)pos + 1 + field_len > adv_len) {
            break;
        }
        uint8_t ad_type = adv[pos + 1];
        if (ad_type == 0xFF && field_len >= 19) {
            const uint8_t *mfg = &adv[pos + 2];
            // Nintendo Company ID 0x0553 (little-endian: 53 05)
            // Wake flag at payload offset 9 (mfg[2 + 9] == 0x81)
            if (mfg[0] == 0x53 && mfg[1] == 0x05 && mfg[2 + 9] == 0x81) {
                for (int i = 0; i < 6; i++) {
                    m_scan_result.controller_mac[i] = adva[5 - i];
                    m_scan_result.host_mac[i] = mfg[2 + 10 + (5 - i)];
                }
                m_scan_result.pid = (uint16_t)(mfg[2 + 5] | ((uint16_t)mfg[2 + 6] << 8));
                memcpy(m_scan_result.adv_data, adv, adv_len);
                m_scan_result.adv_len = adv_len;
                return true;
            }
        }
        pos = (uint8_t)(pos + 1 + field_len);
    }

    return false;
}

static void radio_stop_rx(void) {
    NRF_RADIO->SHORTS = 0;
    NRF_RADIO->INTENCLR = 0xFFFFFFFFUL;
    if (NRF_RADIO->STATE != RADIO_STATE_STATE_Disabled) {
        NRF_RADIO->EVENTS_DISABLED = 0;
        NRF_RADIO->TASKS_DISABLE = 1;
        while (NRF_RADIO->EVENTS_DISABLED == 0 && NRF_RADIO->STATE != RADIO_STATE_STATE_Disabled) {
        }
    }
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->EVENTS_READY = 0;
    NVIC_DisableIRQ(RADIO_IRQn);
    NVIC_ClearPendingIRQ(RADIO_IRQn);
}

static void timer0_stop(void) {
    NRF_TIMER0->TASKS_STOP = 1;
    NRF_TIMER0->INTENCLR = 0xFFFFFFFFUL;
    NRF_TIMER0->EVENTS_COMPARE[0] = 0;
    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
}

static void radio_start_ble_rx(uint8_t ch_idx) {
    uint8_t ch = k_adv_channels[ch_idx % 3];
    uint8_t freq = k_adv_freqs[ch_idx % 3];

    NRF_RADIO->POWER = 1;
    NRF_RADIO->SHORTS = 0;
    NRF_RADIO->INTENCLR = 0xFFFFFFFFUL;

    NRF_RADIO->MODE = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);
    NRF_RADIO->FREQUENCY = freq;
    NRF_RADIO->DATAWHITEIV = ch;

    // BLE Advertising Access Address: 0x8E89BED6
    NRF_RADIO->BASE0 = 0x89BED600UL;
    NRF_RADIO->PREFIX0 = 0x0000008EUL;
    NRF_RADIO->RXADDRESSES = 0x01UL;

    NRF_RADIO->PCNF0 = (8UL << RADIO_PCNF0_LFLEN_Pos) |
                       (1UL << RADIO_PCNF0_S0LEN_Pos) |
                       (0UL << RADIO_PCNF0_S1LEN_Pos) |
                       (RADIO_PCNF0_PLEN_8bit << RADIO_PCNF0_PLEN_Pos);

    NRF_RADIO->PCNF1 = (64UL << RADIO_PCNF1_MAXLEN_Pos) |
                       (0UL << RADIO_PCNF1_STATLEN_Pos) |
                       (3UL << RADIO_PCNF1_BALEN_Pos) |
                       (RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos) |
                       (RADIO_PCNF1_WHITEEN_Enabled << RADIO_PCNF1_WHITEEN_Pos);

    NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) |
                        (RADIO_CRCCNF_SKIPADDR_Skip << RADIO_CRCCNF_SKIPADDR_Pos);
    NRF_RADIO->CRCPOLY = 0x00065BUL;
    NRF_RADIO->CRCINIT = 0x555555UL;

    NRF_RADIO->PACKETPTR = (uint32_t)m_rx_buf;

    NRF_RADIO->EVENTS_READY = 0;
    NRF_RADIO->EVENTS_ADDRESS = 0;
    NRF_RADIO->EVENTS_PAYLOAD = 0;
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->EVENTS_DISABLED = 0;

    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_DISABLE_Msk;
    NRF_RADIO->INTENSET = RADIO_INTENSET_DISABLED_Msk;

    NVIC_ClearPendingIRQ(RADIO_IRQn);
    NVIC_EnableIRQ(RADIO_IRQn);

    NRF_RADIO->TASKS_RXEN = 1;
}

static nrf_radio_signal_callback_return_param_t *radio_signal_cb(uint8_t signal_type) {
    m_ret_param.callback_action = NRF_RADIO_SIGNAL_CALLBACK_ACTION_NONE;
    m_ret_param.params.request.p_next = NULL;

    switch (signal_type) {
    case NRF_RADIO_CALLBACK_SIGNAL_TYPE_START:
        if (m_scan_stop_req || m_scan_captured) {
            m_ret_param.callback_action = NRF_RADIO_SIGNAL_CALLBACK_ACTION_END;
            break;
        }
        // Setup TIMER0 to end timeslot safely before deadline
        NRF_TIMER0->TASKS_STOP = 1;
        NRF_TIMER0->TASKS_CLEAR = 1;
        NRF_TIMER0->MODE = TIMER_MODE_MODE_Timer;
        NRF_TIMER0->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
        NRF_TIMER0->PRESCALER = 4; // 1 MHz (1 us)
        NRF_TIMER0->CC[0] = SCAN_RX_WINDOW_US;
        NRF_TIMER0->EVENTS_COMPARE[0] = 0;
        NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Msk;
        NVIC_ClearPendingIRQ(TIMER0_IRQn);
        NVIC_EnableIRQ(TIMER0_IRQn);
        NRF_TIMER0->TASKS_START = 1;

        radio_start_ble_rx(m_scan_ch_idx);
        break;

    case NRF_RADIO_CALLBACK_SIGNAL_TYPE_RADIO:
        if (NRF_RADIO->EVENTS_DISABLED) {
            NRF_RADIO->EVENTS_DISABLED = 0;
            NRF_RADIO->EVENTS_END = 0;
            if (NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCOk) {
                if (parse_rx_packet(m_rx_buf)) {
                    m_scan_captured = true;
                    radio_stop_rx();
                    timer0_stop();
                    m_ret_param.callback_action = NRF_RADIO_SIGNAL_CALLBACK_ACTION_END;
                    break;
                }
            }
            if (!m_scan_stop_req && !m_scan_captured) {
                NRF_RADIO->TASKS_RXEN = 1;
            }
        }
        break;

    case NRF_RADIO_CALLBACK_SIGNAL_TYPE_TIMER0:
        radio_stop_rx();
        timer0_stop();
        m_scan_ch_idx = (uint8_t)((m_scan_ch_idx + 1) % 3);
        m_ret_param.callback_action = NRF_RADIO_SIGNAL_CALLBACK_ACTION_END;
        break;

    case NRF_RADIO_CALLBACK_SIGNAL_TYPE_EXTEND_FAILED:
    case NRF_RADIO_CALLBACK_SIGNAL_TYPE_EXTEND_SUCCEEDED:
    default:
        break;
    }

    return &m_ret_param;
}

static uint32_t request_next_timeslot(void) {
    memset(&m_timeslot_req, 0, sizeof(m_timeslot_req));
    m_timeslot_req.request_type = NRF_RADIO_REQ_TYPE_EARLIEST;
    m_timeslot_req.params.earliest.hfclk = NRF_RADIO_HFCLK_CFG_XTAL_GUARANTEED;
    m_timeslot_req.params.earliest.priority = NRF_RADIO_PRIORITY_NORMAL;
    m_timeslot_req.params.earliest.length_us = SCAN_TIMESLOT_LEN_US;
    m_timeslot_req.params.earliest.timeout_us = SCAN_REQ_TIMEOUT_US;
    return sd_radio_request(&m_timeslot_req);
}

static void scan_soc_evt_handler(uint32_t evt_id, void *p_context) {
    (void)p_context;
    if (!m_scan_active) {
        return;
    }

    switch (evt_id) {
    case NRF_EVT_RADIO_SESSION_IDLE:
        if (m_scan_captured || m_scan_stop_req || m_scan_slots_left == 0) {
            (void)sd_radio_session_close();
        } else {
            m_scan_slots_left--;
            nrf_pwr_mgmt_feed();
            if (request_next_timeslot() != NRF_SUCCESS) {
                (void)sd_radio_session_close();
            }
        }
        break;

    case NRF_EVT_RADIO_SESSION_CLOSED:
        app_sched_event_put(NULL, 0, scan_notify_sched_handler);
        break;

    case NRF_EVT_RADIO_BLOCKED:
    case NRF_EVT_RADIO_CANCELED:
        if (m_scan_captured || m_scan_stop_req || m_scan_slots_left == 0) {
            (void)sd_radio_session_close();
        } else {
            m_scan_slots_left--;
            if (request_next_timeslot() != NRF_SUCCESS) {
                (void)sd_radio_session_close();
            }
        }
        break;

    default:
        break;
    }
}

NRF_SDH_SOC_OBSERVER(m_switch2_scan_soc_observer, SCAN_SOC_OBSERVER_PRIO, scan_soc_evt_handler, NULL);

uint32_t ble_switch2_scan_start(uint16_t timeout_sec, ble_switch2_scan_cb_t cb) {
    if (m_scan_active) {
        return NRF_ERROR_BUSY;
    }

    ble_init();
    ble_raw_adv_stop();
    ble_disable();

    memset(&m_scan_result, 0, sizeof(m_scan_result));
    m_scan_stop_req = false;
    m_scan_captured = false;
    m_scan_ch_idx = 0;
    m_scan_slots_left = (uint16_t)((timeout_sec > 0 ? timeout_sec : 15) * 20); // 20 * 50ms = 1s
    m_scan_cb = cb;
    m_scan_active = true;

    uint32_t err = sd_radio_session_open(radio_signal_cb);
    if (err != NRF_SUCCESS) {
        m_scan_active = false;
        m_scan_cb = NULL;
        return err;
    }

    err = request_next_timeslot();
    if (err != NRF_SUCCESS) {
        (void)sd_radio_session_close();
        m_scan_active = false;
        m_scan_cb = NULL;
        return err;
    }

    nrf_pwr_mgmt_feed();
    return NRF_SUCCESS;
}

void ble_switch2_scan_stop(void) {
    if (!m_scan_active) {
        return;
    }
    m_scan_stop_req = true;
    m_scan_cb = NULL;
}

bool ble_switch2_scan_is_active(void) {
    return m_scan_active;
}
