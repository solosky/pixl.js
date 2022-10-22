/*
 * bat.c
 *
 *  Created on: 2021年9月25日
 *      Author: solos
 */

#include "bat.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "app_util_platform.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define ADC_CHANNEL 0

void saadc_callback(nrf_drv_saadc_evt_t const *p_event) {
	NRF_LOG_INFO("ADC event: %d", p_event->type);
}

void saadc_init(void) {
	ret_code_t err_code;
	nrf_saadc_channel_config_t channel_config =
			NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN5);

	err_code = nrf_drv_saadc_init(NULL, saadc_callback);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_drv_saadc_channel_init(ADC_CHANNEL, &channel_config);
	APP_ERROR_CHECK(err_code);
}

void saadc_uninit(void) {
	nrfx_saadc_channel_uninit(ADC_CHANNEL);
	nrf_drv_saadc_uninit();
}

bat_level_t bat_get_level(void) {

	nrf_saadc_value_t adc_value;
	ret_code_t err_code;

	bsp_board_led_on(0);
	nrf_delay_ms(1);

	saadc_init();
	err_code = nrfx_saadc_sample_convert(ADC_CHANNEL, &adc_value);
	APP_ERROR_CHECK(err_code);

	NRF_LOG_INFO("adc value: %d", adc_value);

	nrf_drv_saadc_uninit();
	bsp_board_led_off(0);

	if (adc_value > 455) { //3.2V
		return FULL;
	} else if (adc_value > 426) { //3.0V
		return LEVEL_3;
	} else if (adc_value > 398) { //2.8V
		return LEVEL_2;
	} else if (adc_value > 341) { //2.4V
		return LEVEL_1;
	} else {
		return EMPTY;
	}
}

