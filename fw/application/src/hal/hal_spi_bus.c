#include "hal_spi_bus.h"

#include "app_error.h"
#include "app_util_platform.h"
#include "boards.h"
#include <stdio.h>

#include <nrfx_spim.h>

#include "nrf_delay.h"

#include "nrf_gpio.h"
#include "nrf_log.h"

#define SPI_INSTANCE 0

static const nrfx_spim_t m_spi = NRFX_SPIM_INSTANCE(SPI_INSTANCE);

void hal_spi_bus_init() {
    ret_code_t err_code;

    nrfx_spim_config_t spi_oled_config = NRFX_SPIM_DEFAULT_CONFIG;

    spi_oled_config.sck_pin = NRFX_SPIM_SCK_PIN;
    spi_oled_config.mosi_pin = NRFX_SPIM_MOSI_PIN;
    spi_oled_config.miso_pin = NRFX_SPIM_MISO_PIN;
    spi_oled_config.frequency = NRF_SPIM_FREQ_8M; // The SSD1326 can go up to 10 MHz clock
    spi_oled_config.mode = NRF_SPIM_MODE_0;
    spi_oled_config.ss_active_high = false;

    err_code = nrfx_spim_init(&m_spi, &spi_oled_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
}

void hal_spi_bus_attach(spi_device_t *p_dev) {
    nrf_gpio_cfg_output(p_dev->cs_pin);
    nrf_gpio_pin_set(p_dev->cs_pin);
}

void hal_spi_bus_aquire(spi_device_t *p_dev) { nrf_gpio_pin_clear(p_dev->cs_pin); }

void hal_spi_bus_release(spi_device_t *p_dev) { nrf_gpio_pin_set(p_dev->cs_pin); }

uint32_t hal_spi_bus_xfer(spi_transaction_t *p_trans) {
    uint32_t err_code = NRF_SUCCESS;
    nrfx_spim_xfer_desc_t spim_xfer_desc;

    uint32_t max_xfer_bytes = p_trans->tx_length > p_trans->rx_length ? p_trans->tx_length : p_trans->rx_length;
    for (uint32_t i = 0; i < max_xfer_bytes; i += 128) {

        if (i < p_trans->tx_length) {
            spim_xfer_desc.p_tx_buffer = p_trans->p_tx_buffer + i;
            spim_xfer_desc.tx_length = p_trans->tx_length - i < 128 ? p_trans->tx_length - i : 128;
        } else {
            spim_xfer_desc.p_tx_buffer = NULL;
            spim_xfer_desc.tx_length = 0;
        }
        if (i < p_trans->rx_length) {
            spim_xfer_desc.p_rx_buffer = p_trans->p_rx_buffer + i;
            spim_xfer_desc.rx_length = p_trans->rx_length - i < 128 ? p_trans->rx_length - i : 128;
        } else {
            spim_xfer_desc.p_rx_buffer = NULL;
            spim_xfer_desc.rx_length = 0;
        }

        err_code = nrfx_spim_xfer(&m_spi, &spim_xfer_desc, 0);
        if (err_code != NRF_SUCCESS) {
            return err_code;
        }
    }

    return err_code;
}