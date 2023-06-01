/*
 * utils.c
 *
 *  Created on: 2021年10月10日
 *      Author: solos
 */

#include "utils.h"
#include "nrf_bootloader_info.h"
#include "nrf_sdh.h"
#include "nrf_soc.h"
#include "nrf_nvic.h"

ret_code_t utils_rand_bytes(uint8_t rand[], uint8_t bytes) {
    ret_code_t err;
    while ((err = sd_rand_application_vector_get(rand, bytes)) == NRF_ERROR_SOC_RAND_NOT_ENOUGH_VALUES) {
    };
    return err;
}

void enter_dfu() {
    sd_power_gpregret_clr(0, 0);
    sd_power_gpregret_set(0, BOOTLOADER_DFU_START);
    sd_nvic_SystemReset();
}