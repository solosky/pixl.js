#include "bsp_btn.h"
#include "app_button.h"
#include "app_timer.h"
#include "bsp.h"
#include "nrf_log.h"

#include "nrf_pwr_mgmt.h"

typedef enum { BTN_STATE_IDLE, BTN_STATE_PRESSED, BTN_STATE_LONG_PRESSED, BTN_STATE_REPEAT } btn_state_t;

#define BSP_BUTTON_ACTION_REPEAT_PUSH (3)
#define BSP_BUTTON_ACTION_STICK_PUSH (4)
#define BSP_BUTTON_LONG_PUSH_TIMEOUT_MS (1000)
#define BSP_BUTTON_REPEAT_PUSH_TIMEOUT_MS (200)
#define BSP_BUTTON_STICK_PUSH_TIMEOUT_MS (10000)

typedef struct {
    uint8_t state;
} bsp_btn_t;

APP_TIMER_DEF(m_bsp_button_long_push_tmr);
APP_TIMER_DEF(m_bsp_button_repeat_push_tmr);
APP_TIMER_DEF(m_bsp_button_stick_push_tmr);
bsp_btn_event_cb_t m_bsp_btn_event_cb = NULL;
uint8_t m_current_long_push_pin_no;

static void bsp_button_event_handler(uint8_t pin_no, uint8_t button_action);

static const app_button_cfg_t app_buttons[BUTTONS_NUMBER] = {
    {BSP_BUTTON_0, false, BUTTON_PULL, bsp_button_event_handler},
    {BSP_BUTTON_1, false, BUTTON_PULL, bsp_button_event_handler},
    {BSP_BUTTON_2, false, BUTTON_PULL, bsp_button_event_handler},
};

static bsp_btn_t m_bsp_btns[BUTTONS_NUMBER] = {0};

static uint8_t bsp_button_pin_to_index(uint8_t pin_no) {
    for (uint32_t i = 0; i < BUTTONS_NUMBER; i++) {
        if (app_buttons[i].pin_no == pin_no) {
            return i;
        }
    }
    return 0xFF;
}

static void bsp_button_callback(uint8_t idx, bsp_btn_event_t event) {
    if (m_bsp_btn_event_cb) {
        m_bsp_btn_event_cb(idx, event);
    }
}

static void bsp_button_event_handler(uint8_t pin_no, uint8_t button_action) {
    uint32_t err_code = NRF_SUCCESS;

    // feed
    nrf_pwr_mgmt_feed();

    uint8_t idx = bsp_button_pin_to_index(pin_no);
    if (button_action == APP_BUTTON_PUSH) {
        // trigger pressed first
        // bsp_button_callback(idx, BSP_BTN_EVENT_PRESSED);

        switch (m_bsp_btns[idx].state) {
        case BTN_STATE_IDLE:
            err_code = app_timer_start(m_bsp_button_long_push_tmr, APP_TIMER_TICKS(BSP_BUTTON_LONG_PUSH_TIMEOUT_MS),
                                       (void *)&m_current_long_push_pin_no);
            if (err_code == NRF_SUCCESS) {
                m_bsp_btns[idx].state = BTN_STATE_PRESSED;
                m_current_long_push_pin_no = pin_no;
            }

        case BTN_STATE_LONG_PRESSED:
        case BTN_STATE_PRESSED:
        case BTN_STATE_REPEAT:
            // TODO

            break;
        }

    } else if (button_action == APP_BUTTON_RELEASE) {
        // trigger release first
        // bsp_button_callback(idx, BSP_BTN_EVENT_RELEASED);

        switch (m_bsp_btns[idx].state) {
        case BTN_STATE_PRESSED:
            // short push release
            app_timer_stop(m_bsp_button_long_push_tmr);
            bsp_button_callback(idx, BSP_BTN_EVENT_SHORT);
            break;

        case BTN_STATE_LONG_PRESSED:
        case BTN_STATE_REPEAT:
            // long and repeat push release
            app_timer_stop(m_bsp_button_repeat_push_tmr);
            app_timer_stop(m_bsp_button_stick_push_tmr);
            break;
        }

        m_bsp_btns[idx].state = BTN_STATE_IDLE;

    } else if (button_action == BSP_BUTTON_ACTION_LONG_PUSH) {
        bsp_button_callback(idx, BSP_BTN_EVENT_LONG);

        // start repeat timer
        err_code = app_timer_start(m_bsp_button_repeat_push_tmr, APP_TIMER_TICKS(BSP_BUTTON_REPEAT_PUSH_TIMEOUT_MS),
                                   (void *)&m_current_long_push_pin_no);
        if (err_code == NRF_SUCCESS) {
            m_bsp_btns[idx].state = BTN_STATE_LONG_PRESSED;
            m_current_long_push_pin_no = pin_no;
        }

        app_timer_start(m_bsp_button_stick_push_tmr, APP_TIMER_TICKS(BSP_BUTTON_STICK_PUSH_TIMEOUT_MS),
                                   (void *)&m_current_long_push_pin_no);

    } else if (button_action == BSP_BUTTON_ACTION_REPEAT_PUSH) {
        m_bsp_btns[idx].state = BTN_STATE_REPEAT;
        bsp_button_callback(idx, BSP_BTN_EVENT_REPEAT);
    } else if(button_action == BSP_BUTTON_ACTION_STICK_PUSH){
        bsp_button_callback(idx, BSP_BTN_EVENT_STICK);
    }
}

static void button_long_push_timer_handler(void *p_context) {
    bsp_button_event_handler(*(uint8_t *)p_context, BSP_BUTTON_ACTION_LONG_PUSH);
}

static void button_repeat_push_timer_handler(void *p_context) {
    bsp_button_event_handler(*(uint8_t *)p_context, BSP_BUTTON_ACTION_REPEAT_PUSH);
}

static void button_stick_push_timer_handler(void *p_context) {
    bsp_button_event_handler(*(uint8_t *)p_context, BSP_BUTTON_ACTION_STICK_PUSH);
}

void bsp_btn_init(bsp_btn_event_cb_t p_event_cb) {
    uint32_t err_code = NRF_SUCCESS;
    err_code = app_button_init((app_button_cfg_t *)app_buttons, BUTTONS_NUMBER, APP_TIMER_TICKS(50));
    APP_ERROR_CHECK(err_code);
    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
    err_code =
        app_timer_create(&m_bsp_button_long_push_tmr, APP_TIMER_MODE_SINGLE_SHOT, button_long_push_timer_handler);
    APP_ERROR_CHECK(err_code);
    err_code =
        app_timer_create(&m_bsp_button_repeat_push_tmr, APP_TIMER_MODE_REPEATED, button_repeat_push_timer_handler);
    APP_ERROR_CHECK(err_code);
    err_code =
        app_timer_create(&m_bsp_button_stick_push_tmr, APP_TIMER_MODE_SINGLE_SHOT, button_stick_push_timer_handler);
    APP_ERROR_CHECK(err_code);

    m_bsp_btn_event_cb = p_event_cb;
}
