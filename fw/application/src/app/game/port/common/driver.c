#include "driver.h"
#include "app_error.h"
#include "game_view.h"
#include "mui_u8g2.h"

#include "boards_defines.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

void JOY_OLED_end() { hal_spi_bus_release(mui_u8g2_get_spi_device()); }

void JOY_OLED_write_data(uint8_t is_data, uint8_t data) {
    spi_transaction_t trans = {
        .p_tx_buffer = &data,
        .tx_length = 1,
        .p_rx_buffer = NULL,
        .rx_length = 0,
    };
    nrf_gpio_pin_write(LCD_DC_PIN, is_data);
    uint32_t err_code = hal_spi_bus_xfer(&trans);
    APP_ERROR_CHECK(err_code);
}

void JOY_OLED_write_cmd(uint8_t cmd) { JOY_OLED_write_data(0, cmd); }

void JOY_OLED_set_pos(uint8_t column, uint8_t page) {
    JOY_OLED_write_cmd(0xb0 + (0x07 & page));
    JOY_OLED_write_cmd(0x10 | column >> 4);
    JOY_OLED_write_cmd(column & 0x0f);
}

void JOY_OLED_send(uint8_t b) { JOY_OLED_write_data(1, b); }

void JOY_OLED_data_start(uint8_t y) {
    hal_spi_bus_aquire(mui_u8g2_get_spi_device());
    JOY_OLED_set_pos(0, y);
}

void JOY_idle() {
    game_view_handle_event();
    NRF_LOG_FLUSH();
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Pseudo random number generator
uint16_t rnval = 0xACE1;
uint16_t JOY_random(void) {
    rnval = (rnval >> 0x01) ^ (-(rnval & 0x01) & 0xB400);
    return rnval;
}

// Buzzer
void JOY_sound(uint8_t freq, uint8_t dur) {
    //   while(dur--) {
    //     #if JOY_SOUND == 1
    //     if(freq) PIN_low(PIN_BEEP);
    //     #endif
    //     DLY_us(255 - freq);
    //     PIN_high(PIN_BEEP);
    //     DLY_us(255 - freq);
    //   }
}
