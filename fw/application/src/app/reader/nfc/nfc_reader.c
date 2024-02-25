#include "nfc_reader.h"
#include "boards_defines.h"
#include "tag_helper.h"

#include "app_status.h"
#include "bsp_time.h"
#include "nrf_gpio.h"
#include "rc522.h"

void nfc_reader_enter() {

    nrf_gpio_pin_set(HF_ANT_SEL);
    nrf_gpio_pin_set(RD_PWR);
    bsp_timer_init();
    bsp_timer_start();
    pcd_14a_reader_init();
    pcd_14a_reader_reset();

    pcd_14a_reader_antenna_on();
}

void nfc_reader_exit() {

    pcd_14a_reader_antenna_off();

    bsp_timer_stop();
    bsp_timer_uninit();
    pcd_14a_reader_uninit();

    nrf_gpio_pin_clear(HF_ANT_SEL);
    nrf_gpio_pin_clear(RD_PWR);
}

void nfc_reader_init(){
    nrf_gpio_cfg_output(HF_ANT_SEL);
    nrf_gpio_cfg_output(RD_PWR);
    nrf_gpio_pin_clear(HF_ANT_SEL);
    nrf_gpio_pin_clear(RD_PWR);
}