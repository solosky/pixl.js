/*
 * bat.c
 *
 *  Created on: 2021年9月25日
 *      Author: solos
 */

#include "bat.h"

#include "app_error.h"
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

#ifdef BAT_LIPO

#define ADC_CHANNEL 0
#define ADC_SAADC_INPUT NRF_SAADC_INPUT_AIN2
#define CHRG_IN_PIN 5

// 3.0 2.8  2.6  2.4  2.2 2.1 2.0 1.9 1.8 1.7
// const uint32_t adc_map[] = { 853, 796, 739, 682, 625, 597, 569, 541, 512, 484}
const float adc_map[] = {3.5f, 3.6f, 3.7f, 3.8f, 3.9f, 4.0f, 4.05f, 4.1f, 4.15f, 4.2f};

void saadc_callback(nrf_drv_saadc_evt_t const *p_event) { NRF_LOG_INFO("ADC event: %d", p_event->type); }

void saadc_init(void) {
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(ADC_SAADC_INPUT);

    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(ADC_CHANNEL, &channel_config);
    APP_ERROR_CHECK(err_code);
}

void saadc_uninit(void) {
    nrfx_saadc_channel_uninit(ADC_CHANNEL);
    nrf_drv_saadc_uninit();
}

float bat_get_voltage(void) {

    nrf_saadc_value_t adc_value;
    ret_code_t err_code;

    saadc_init();
    err_code = nrfx_saadc_sample_convert(ADC_CHANNEL, &adc_value);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("adc value: %d", adc_value);

    nrf_drv_saadc_uninit();

    return 3.6f / 1024 * adc_value * 1.451;
}

bool bat_is_charging() {
    nrf_gpio_cfg_input(CHRG_IN_PIN, NRF_GPIO_PIN_PULLUP);
    uint32_t charge_status = nrf_gpio_pin_read(CHRG_IN_PIN);
    return !charge_status;
}

void bat_get_state(bat_state_t *p_state) {
    nrf_saadc_value_t adc_value;
    ret_code_t err_code;

    saadc_init();
    err_code = nrfx_saadc_sample_convert(ADC_CHANNEL, &adc_value);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("adc value: %d", adc_value);

    nrf_drv_saadc_uninit();

    nrf_gpio_cfg_input(CHRG_IN_PIN, NRF_GPIO_PIN_PULLUP);

    p_state->voltage = 3.6f / 1024 * adc_value * 1.451f;
    p_state->charging = !nrf_gpio_pin_read(CHRG_IN_PIN);
    for (uint32_t i = 0; i < sizeof(adc_map) / sizeof(adc_map[0]); i++) {
        if (adc_map[i] > p_state->voltage) {
            p_state->level = i;
            break;
        }
    }
}

#endif