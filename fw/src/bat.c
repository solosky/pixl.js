/*
 * bat.c
 *
 *  Created on: 2021年9月25日
 *      Author: solos
 */

#include "bat.h"

#include "app_error.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "boards.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_timer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "settings.h"

#define ADC_CHANNEL 0

APP_TIMER_DEF(m_chrg_timer);

const float cr2032_voltage_map[] = {2.9, 2.8, 2.7, 2.6, 2.5, 2.4, 2.3, 2.1, 2.0f };
const float lipo_voltage_map[] = {4.15, 4.1, 4.05, 4.0f, 3.9, 3.8, 3.7, 3.6, 3.5};

#define BAT_LEVEL_NUM 9

void saadc_callback(nrf_drv_saadc_evt_t const *p_event) { NRF_LOG_INFO("ADC event: %d", p_event->type); }

chrg_data_t chrg = {0};

void chrg_read(void) {
    uint32_t new_stats = nrf_gpio_pin_read(CHRG_PIN);
    if (new_stats == chrg.stats) return;
    chrg.stats = new_stats;
    if (chrg.callback != NULL) chrg.callback();
}

void chrg_set_callback(void *cb) { chrg.callback = cb; }

bool get_stats(void) {
    if (!settings_get_data()->bat_mode) {
        return false;
    }
    return chrg.stats == 0;
}

void chrg_init(void) {
    ret_code_t err_code;

    if (settings_get_data()->bat_mode) {
        nrf_gpio_cfg_input(CHRG_PIN, NRF_GPIO_PIN_PULLUP);

        err_code = app_timer_create(&m_chrg_timer, APP_TIMER_MODE_REPEATED, chrg_read);
        APP_ERROR_CHECK(err_code);

        err_code = app_timer_start(m_chrg_timer, APP_TIMER_TICKS(200), NULL);
        APP_ERROR_CHECK(err_code);
    }
}

void saadc_init(void) {
    ret_code_t err_code;

    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(
        settings_get_data()->bat_mode ? NRF_SAADC_INPUT_AIN0 : NRF_SAADC_INPUT_VDD);

    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(ADC_CHANNEL, &channel_config);
    APP_ERROR_CHECK(err_code);
}

void saadc_uninit(void) {
    nrfx_saadc_channel_uninit(ADC_CHANNEL);
    nrf_drv_saadc_uninit();
}

uint8_t bat_get_level(void) {

    nrf_saadc_value_t adc_value;
    ret_code_t err_code;

    saadc_init();
    err_code = nrfx_saadc_sample_convert(ADC_CHANNEL, &adc_value);
    APP_ERROR_CHECK(err_code);
    saadc_uninit();

    const float* p_voltage_map = NULL;
    float voltage = 0.0f;
    uint32_t level = 0;

    // lipo mode
    if (settings_get_data()->bat_mode) {
        p_voltage_map = lipo_voltage_map;
        voltage = 3.6f / 1024 * adc_value * 1.451f;
    } else { // cr2032 mode
        p_voltage_map = cr2032_voltage_map;
        voltage = 3.6f / 1024 * adc_value;
    }



    for (uint32_t i = 0; i < BAT_LEVEL_NUM; i++) {
        if (voltage >= p_voltage_map[i]) {
            level = BAT_LEVEL_NUM - i - 1;
            break;
        }
    }
    char txt[32];
    sprintf(txt, "BAT: %d, %.02f, %d", adc_value, voltage, level);
    NRF_LOG_INFO("%s", nrf_log_push(txt));


    return level;
}
