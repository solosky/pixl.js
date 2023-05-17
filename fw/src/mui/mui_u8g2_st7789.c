#include "mui_u8g2.h"

/*
 * u8g2 Demo Application running on Nordic Semiconductors nRF52 DK with nRF SDK 17.02 and:
 *
 * - TWIM and a Waveshield SH1106 OLED 128x64 display
 * or
 * - SPIM and a Waveshield SSD1327 OLED 128x128 16 grey level display
 * that can be found on Ebay/Amazon for few EUR.
 *
 * Created 20200102 by @daubsi, based on the great u8g2 library
 * https://github.com/olikraus/u8g2 Probably not the cleanest code but it works :-D
 *
 * TWI: Set up with i2c/TWIM in 400 kHz (100 kHz works equally well) in non-blocking mode
 * SPI: Setup up in SPIM with 4 MHz in non-blocking mode
 *
 * Wiring TWI:
 * Connect VCC and GND of the display with VDD and GND on the nRF52 DK
 * Connect SCL to pin 27 ("P0.27") and SDA to pin 26 ("P0.26") on the nRF52 DK
 *
 * Wiring SPI:
 * Connect VCC and GND of the diplay with VDD and GND on the NRF52 DK
 * Connect CLK (yellow wire) to pin 27 ("P0.27") and DIN (blue wire) to pin 26 ("P0.26")
 * on the nRF52 DK Connect CS (orange wire) to pin 12 ("P0.12") and DC (green wire) to pin
 * 11 ("P0.11") on the nRF52 DK Connect Reset (white wire) to pin 31 ("P0.31") on the
 * nRF52 DK
 *
 * Settings in sdk_config.h
 *
 * TWI:
 * #define NRFX_TWIM_ENABLED 1
 * #define NRFX_TWIM0_ENABLED 1
 * #define NRFX_TWIM1_ENABLED 0
 * #define NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY 104857600
 * #define NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT 0
 * #define TWI_ENABLED 1
 * #define TWI_DEFAULT_CONFIG_FREQUENCY 104857600
 * #define TWI_DEFAULT_CONFIG_CLR_BUS_INIT 0
 * #define TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT 0
 * #define TWI0_ENABLED 1
 * #define TWI0_USE_EASY_DMA 1
 * #define TWI1_ENABLED 0
 * #define TWI1_USE_EASY_DMA 1
 *
 * SWI:
 * #define NRFX_SPIM_ENABLED 1
 * #define NRFX_SPIM0_ENABLED 0
 * #define NRFX_SPIM1_ENABLED 1
 * #define NRFX_SPIM_EXTENDED_ENABLED 0 // Only available on NRF52840 on SPIM3
 * #define NRFX_SPIM_MISO_PULL_CFG 1
 * #define NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY 6
 *
 * Probably most of those configs are not needed apart from TWI_ENABLED and TWI0_ENABLED
 * but I kept them there
 *
 * When compiling for SPI (== #define USE_SPI 1) and TWI (== #define USE_TWI 1) at the
 * same time, make sure, that you use different HW resources, e.g. TWI_INSTANCE_ID = 0 and
 * SPI_INSTANCE_ID = 1 (and the corresponding TWI0_ENABLED 1, SPIM1_ENABLED 1 in
 * sdk_config.h Otherwise there vill be linker conflicts when using the same id.
 * https://devzone.nordicsemi.com/f/nordic-q-a/35182/irq-handler-compile-error
 *
 * The u8g2 sources have to be unpacked and the csrc subfolder of that archive should be
 * placed in the main directory of this project in a folder u8g2.
 */

#if (1)
#include <stdio.h>

#include "hal_spi_bus.h"

#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "settings.h"

#include "st7789.h"
#include "u8g2.h"
#include "u8x8.h"

#define LCD_CS_PIN 27
#define LCD_RESET_PIN 29
#define LCD_BL_PIN 20
#define LCD_DC_PIN 28 // green wire (DC)

#include "app_pwm.h"

u8g2_t u8g2;
static spi_device_t m_dev;
uint8_t m_u8g2_initialized = 0;

APP_PWM_INSTANCE(pwm1, 1); // Create the instance "PWM1" using TIMER1.

static ret_code_t pwm_init(void) {
    ret_code_t err_code;
    /* 2-channel PWM, 200Hz, output on DK LED pins. */
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(200L, LCD_BL_PIN);

    /* Switch the polarity of the second channel. */
    pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;

    /* Initialize and enable PWM. */
    err_code = app_pwm_init(&pwm1, &pwm1_cfg, NULL);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    settings_data_t *p_settings = settings_get_data();
    app_pwm_duty_t init_duty = p_settings->lcd_backlight <= 100 ? p_settings->lcd_backlight : 0;
    NRF_LOG_INFO("init bl = %d", init_duty);
    if (init_duty > 0) {
        app_pwm_enable(&pwm1);
        while (app_pwm_channel_duty_set(&pwm1, 0, init_duty) == NRF_ERROR_BUSY)
            ;

    }

    return NRF_SUCCESS;
}

uint8_t u8x8_HW_com_spi_nrf52832(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8g2_nrf_gpio_and_delay_spi_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

uint8_t u8g2_nrf_gpio_and_delay_spi_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_GPIO_DC:
        // nrf_gpio_pin_write(LCD_DC_PIN, arg_int);
        break;

    case U8X8_MSG_GPIO_RESET:
        // nrf_gpio_pin_write(LCD_RESET_PIN, arg_int);
        break;

    case U8X8_MSG_DELAY_MILLI:
        nrf_delay_ms(arg_int);
        break;

    case U8X8_MSG_DELAY_10MICRO:
        nrf_delay_us(10 * arg_int);
        break;

    default:
        u8x8_SetGPIOResult(u8x8, 1); // default return value
        break;
    }
    return 1;
}

uint8_t u8x8_HW_com_spi_nrf52832(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {

    uint8_t *data;
    bool res = false;
    ret_code_t err_code;
    static uint8_t buffer[128];
    static uint8_t buf_idx = 1;

    switch (msg) {
    case U8X8_MSG_BYTE_SEND: {

        //        spi_transaction_t trans = {
        //            .p_tx_buffer = arg_ptr,
        //            .tx_length = arg_int,
        //            .p_rx_buffer = NULL,
        //            .rx_length = 0,
        //        };
        //        hal_spi_bus_aquire(&m_dev);
        //        err_code = hal_spi_bus_xfer(&trans);
        //        hal_spi_bus_release(&m_dev);
        //        APP_ERROR_CHECK(err_code);
        break;
    }
    case U8X8_MSG_BYTE_SET_DC: {
        // u8x8_gpio_SetDC(u8x8, arg_int);
        break;
    }
    case U8X8_MSG_BYTE_START_TRANSFER: {
        break;
    }
    case U8X8_MSG_BYTE_END_TRANSFER: {
        break;
    }
    default:
        return 0;
    }
    return 1;
}

static const u8x8_display_info_t u8x8_st7789_enh_dg128064_display_info = {
    /* chip_enable_level = */ 0,
    /* chip_disable_level = */ 1,

    /* post_chip_enable_wait_ns = */ 150, /* */
    /* pre_chip_disable_wait_ns = */ 50,  /* */
    /* reset_pulse_width_ms = */ 1,
    /* post_reset_wait_ms = */ 1,
    /* sda_setup_time_ns = */ 50,   /* */
    /* sck_pulse_width_ns = */ 120, /* */
    /* sck_clock_hz = */ 4000000UL, /* */
    /* spi_mode = */ 0,             /* active high, rising edge */
    /* i2c_bus_clock_100kHz = */ 4,
    /* data_setup_time_ns = */ 40,   /* */
    /* write_pulse_width_ns = */ 80, /* */
    /* tile_width = */ 20,           /* width of 16*8=128 pixel */
    /* tile_hight = */ 11,
    /* default_x_offset = */ 0,
    /* flipmode_x_offset = */ 4,
    /* pixel_width = */ 160,
    /* pixel_height = */ 86};

static volatile bool m_xfer_done = false;

void u8x8_d_st7789_xfer_done_cb(st7789_evt_type_t e, void *p) { m_xfer_done = true; }

static inline void u8x8_d_st7789_set_pix(uint8_t *data, uint8_t x, uint8_t y, uint16_t px) {
    data[y * 16 * 2 + x * 2] = px >> 8;
    data[y * 16 * 2 + x * 2 + 1] = px;
}

void u8x8_d_st7789_transfer_tile(uint16_t x, uint16_t y, uint8_t *data) {
    uint8_t px[8 * 8 * 2 * 4]; // 1 tile: 8x8 with 2x zoom out
    memset(px, 0, sizeof(px));
    for (uint8_t yi = 0; yi < 8; yi++) {
        for (uint8_t xi = 0; xi < 8; xi++) {
            if ((data[xi] >> yi) & 1) {
                u8x8_d_st7789_set_pix(px, xi * 2, yi * 2, 0xFFFF);
                u8x8_d_st7789_set_pix(px, xi * 2 + 1, yi * 2, 0xFFFF);
                u8x8_d_st7789_set_pix(px, xi * 2, yi * 2 + 1, 0xFFFF);
                u8x8_d_st7789_set_pix(px, xi * 2 + 1, yi * 2 + 1, 0xFFFF);
            } else {
                //                u8x8_d_st7789_set_pix(px, xi * 2, yi * 2, 0);
                //                u8x8_d_st7789_set_pix(px, xi * 2 + 1, yi * 2, 0);
                //                u8x8_d_st7789_set_pix(px, xi * 2, yi * 2 + 1, 0);
                //                u8x8_d_st7789_set_pix(px, xi * 2 + 1, yi * 2 + 1, 0);
            }
        }
    }

    m_xfer_done = false;
    st7789_display_area(px, sizeof(px),
                        x * 8 * 2,           // x1
                        y * 8 * 2,           // y1
                        (x + 1) * 8 * 2 - 1, // x2
                        (y + 1) * 8 * 2 - 1, // y2
                        u8x8_d_st7789_xfer_done_cb, NULL);

    while (!m_xfer_done) {
        ;
    }
}

uint8_t u8x8_d_st7789_enh_dg128064(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    uint8_t x, c, y;
    uint8_t *ptr;

    switch (msg) {
    case U8X8_MSG_DISPLAY_SETUP_MEMORY:
        u8x8_d_helper_display_setup_memory(u8x8, &u8x8_st7789_enh_dg128064_display_info);
        break;
    case U8X8_MSG_DISPLAY_SET_FLIP_MODE:
        //        if ( arg_int == 0 )
        //        {
        //            u8x8_cad_SendSequence(u8x8, u8x8_d_st7567_n_flip0_seq);
        //            u8x8->x_offset = u8x8->display_info->default_x_offset;
        //        }
        //        else
        //        {
        //            u8x8_cad_SendSequence(u8x8, u8x8_d_st7567_n_flip1_seq);
        //            u8x8->x_offset = u8x8->display_info->flipmode_x_offset;
        //        }
        break;
    case U8X8_MSG_DISPLAY_INIT:
        // u8x8_d_helper_display_init(u8x8);
        // u8x8_cad_SendSequence(u8x8, u8x8_st7567_enh_dg128064_init_seq);
        break;
    case U8X8_MSG_DISPLAY_SET_POWER_SAVE:
        //        if ( arg_int == 0 )
        //            u8x8_cad_SendSequence(u8x8, u8x8_d_st7567_132x64_powersave0_seq);
        //        else
        //            u8x8_cad_SendSequence(u8x8, u8x8_d_st7567_132x64_powersave1_seq);
        break;
#ifdef U8X8_WITH_SET_CONTRAST
    case U8X8_MSG_DISPLAY_SET_CONTRAST:
        //        u8x8_cad_StartTransfer(u8x8);
        //        u8x8_cad_SendCmd(u8x8, 0x081 );
        //        u8x8_cad_SendArg(u8x8, arg_int >> 2 );	/* st7567 has range from 0 to 63 */
        //        u8x8_cad_EndTransfer(u8x8);
        break;
#endif
    case U8X8_MSG_DISPLAY_DRAW_TILE:
        //        u8x8_cad_StartTransfer(u8x8);
        //
        //        x = ((u8x8_tile_t *)arg_ptr)->x_pos;
        //        x *= 8;
        //        x += u8x8->x_offset;
        //        u8x8_cad_SendCmd(u8x8, 0x010 | (x>>4) );
        //        u8x8_cad_SendCmd(u8x8, 0x000 | ((x&15)));
        //        u8x8_cad_SendCmd(u8x8, 0x0b0 | (((u8x8_tile_t *)arg_ptr)->y_pos));
        //
        //        c = ((u8x8_tile_t *)arg_ptr)->cnt;
        //        c *= 8;
        //        ptr = ((u8x8_tile_t *)arg_ptr)->tile_ptr;
        //        /*
        //      The following if condition checks the hardware limits of the st7567
        //      controller: It is not allowed to write beyond the display limits.
        //      This is in fact an issue within flip mode.
        //        */
        //        if ( c + x > 132u )
        //        {
        //            c = 132u;
        //            c -= x;
        //        }
        //        do
        //        {
        //            u8x8_cad_SendData(u8x8, c, ptr);	/* note: SendData can not handle more than 255 bytes */
        //            arg_int--;
        //        } while( arg_int > 0 );
        //
        //        u8x8_cad_EndTransfer(u8x8);
        {
            u8x8_tile_t *p_tile;

            p_tile = (u8x8_tile_t *)arg_ptr;
            ptr = ((u8x8_tile_t *)arg_ptr)->tile_ptr;
            //NRF_LOG_INFO("tile:  x:%d, y:%d, cnt:%d", p_tile->x_pos, p_tile->y_pos, p_tile->cnt);

            for (uint16_t i = 0; i < p_tile->cnt; i++) {
                u8x8_d_st7789_transfer_tile(p_tile->x_pos + i, p_tile->y_pos, ptr + i * 8);
            }
        }
        break;
    }
    return 1;
}

uint8_t *u8g2_m_20_11_f(uint8_t *page_cnt)
{
    static uint8_t buf[20*11*8];
    *page_cnt = 11;
    return buf;
}

void u8g2_Setup_st7789_enh_dg128064_f(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb,
                                      u8x8_msg_cb gpio_and_delay_cb) {
    uint8_t tile_buf_height;
    uint8_t *buf;
    u8g2_SetupDisplay(u8g2, u8x8_d_st7789_enh_dg128064, u8x8_cad_001, byte_cb, gpio_and_delay_cb);
    buf = u8g2_m_20_11_f(&tile_buf_height);
    u8g2_SetupBuffer(u8g2, buf, tile_buf_height, u8g2_ll_hvline_vertical_top_lsb, rotation);
}

void mui_u8g2_init(u8g2_t *p_u8g2) {
    //    m_dev.cs_pin = LCD_CS_PIN;
    //    hal_spi_bus_attach(&m_dev);

    //    nrf_gpio_cfg_output(LCD_RESET_PIN);
    //    nrf_gpio_cfg_output(LCD_DC_PIN);
    //    nrf_gpio_cfg_output(LCD_BL_PIN);
    //    nrf_gpio_pin_clear(LCD_BL_PIN);

    u8g2_Setup_st7789_enh_dg128064_f(p_u8g2, U8G2_R0, u8x8_HW_com_spi_nrf52832, u8g2_nrf_gpio_and_delay_spi_cb);

    st7789_init();

    //    settings_data_t *p_settings = settings_get_data();
    //    if (p_settings->backlight) {
    //        mui_u8g2_set_backlight(p_settings->backlight);
    //    }

    u8g2_InitDisplay(p_u8g2);
    u8g2_SetPowerSave(p_u8g2, 0);

    pwm_init();
}

void mui_u8g2_deinit(u8g2_t *p_u8g2) {
    u8g2_SetPowerSave(p_u8g2, 1);

    st7789_sleep();

    app_pwm_disable(&pwm1);

    nrf_gpio_pin_clear(LCD_BL_PIN);
    nrf_gpio_cfg_default(LCD_BL_PIN);
}

void mui_u8g2_set_backlight(uint8_t bl) { nrf_gpio_pin_write(LCD_BL_PIN, bl); }

uint8_t mui_u8g2_get_backlight() { return nrf_gpio_pin_out_read(LCD_BL_PIN); }

void mui_u8g2_set_backlight_level(uint8_t value) {
    if (value == 0) {
        app_pwm_disable(&pwm1);
    } else {
        if (pwm1.p_cb->state != NRFX_DRV_STATE_POWERED_ON) {
            app_pwm_enable(&pwm1);
        }
        while (app_pwm_channel_duty_set(&pwm1, 0, value) == NRF_ERROR_BUSY)
            ;
    }
}
int8_t mui_u8g2_get_backlight_level(void) { return (int8_t) app_pwm_channel_duty_get(&pwm1, 0); }

#endif