/**
 * Copyright (c) 2017 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include "sdk_common.h"

#if(1)


#include "nrf_lcd.h"
#include "nrf_drv_spi.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_log.h"
#include "st7789.h"

#include "hal_spi_bus.h"

#define ST7789_DC_PIN LCD_DC_PIN
#define ST7789_RST_PIN LCD_RESET_PIN
#define ST7789_LEDA_PIN LCD_BL_PIN
#define ST7789_CS_PIN LCD_CS_PIN

// Set of commands described in ST7789 datasheet.
#define ST7789_NOP         0x00
#define ST7789_SWRESET     0x01
#define ST7789_RDDID       0x04
#define ST7789_RDDST       0x09

#define ST7789_SLPIN       0x10
#define ST7789_SLPOUT      0x11
#define ST7789_PTLON       0x12
#define ST7789_NORON       0x13

#define ST7789_RDMODE      0x0A
#define ST7789_RDMADCTL    0x0B
#define ST7789_RDPIXFMT    0x0C
#define ST7789_RDIMGFMT    0x0D
#define ST7789_RDSELFDIAG  0x0F

#define ST7789_INVOFF      0x20
#define ST7789_INVON       0x21
#define ST7789_GAMMASET    0x26
#define ST7789_DISPOFF     0x28
#define ST7789_DISPON      0x29

#define ST7789_CASET       0x2A
#define ST7789_RASET       0x2B
#define ST7789_RAMWR       0x2C
#define ST7789_RAMRD       0x2E

#define ST7789_PTLAR       0x30
#define ST7789_MADCTL      0x36
#define ST7789_COLMOD      0x3A

//#define ST7789_FRMCTR1     0xB1
#define ST7789_RGBCTRL     0xB1
#define ST7789_PORCTR      0xB2
//#define ST7789_FRMCTR2     0xB2
#define ST7789_FRMCTR3     0xB3
#define ST7789_INVCTR      0xB4
#define ST7789_DFUNCTR     0xB6
#define ST7789_GCTRL       0xB7

#define ST7789_VCOM        0xBB

#define ST7789_LCMCTR      0xC0
//#define ST7789_PWCTR1      0xC0
#define ST7789_PWCTR2      0xC1
#define ST7789_VDVVRHEN    0xC2
//#define ST7789_PWCTR3      0xC2
#define ST7789_VRHS        0xC3
//#define ST7789_PWCTR4      0xC3
#define ST7789_VDVS        0xC4
//#define ST7789_PWCTR5      0xC4
#define ST7789_VMCTR1      0xC5
#define ST7789_FRMCTR2     0xC6
#define ST7789_VMCTR2      0xC7
#define ST7789_PWCTRSEQ    0xCB
#define ST7789_PWCTRA      0xCD
#define ST7789_PWCTRB      0xCF

#define ST7789_PWCTRL1     0xD0

#define ST7789_RDID1       0xDA
#define ST7789_RDID2       0xDB
#define ST7789_RDID3       0xDC
#define ST7789_RDID4       0xDD

#define ST7789_PVGAMCTRL     0xE0
#define ST7789_NVGAMCTRL     0xE1
#define ST7789_DGMCTR1     0xE2
#define ST7789_DGMCTR2     0xE3
#define ST7789_SPI2EN      0xE7
#define ST7789_TIMCTRA     0xE8
#define ST7789_TIMCTRB     0xEA

#define ST7789_ENGMCTR     0xF2
#define ST7789_INCTR       0xF6
#define ST7789_PUMP        0xF7

#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_ML  0x10
#define ST7789_MADCTL_RGB 0x00 //replace BGR with this to get normal coloring
#define ST7789_MADCTL_BGR 0x08 //used by default
#define ST7789_MADCTL_MH  0x04

//Position setting for 240x240 LCD
#define ST7789_240x240_XSTART 0
#define ST7789_240x240_YSTART 80

//Datasheet: https://www.numworks.com/shared/binary/datasheets/st7789v-lcd-controller-73f8bc3e.pdf

static spi_device_t m_dev;


static void spi_write(const void *data, size_t size) {
    spi_transaction_t tx = {
        .p_tx_buffer = data,
        .tx_length = size,
        .p_rx_buffer = NULL,
        .rx_length = 0
    };

    hal_spi_bus_aquire(&m_dev);
    hal_spi_bus_xfer(&tx);
    hal_spi_bus_release(&m_dev);
}

static void spi_write16_cb(const void *data, size_t size, st7789_event_handler_t cb_handler, void* p_context) {
    spi_transaction_t tx = {
        .p_tx_buffer = data,
        .tx_length = size,
        .p_rx_buffer = NULL,
        .rx_length = 0
    };
    hal_spi_bus_aquire(&m_dev);
    hal_spi_bus_xfer(&tx);
    hal_spi_bus_release(&m_dev);
    cb_handler(ST7789_EVENT_FLUSH_DONE, p_context);

}

static void write_command(uint8_t c) {
    nrf_gpio_pin_clear(ST7789_DC_PIN);
    spi_write(&c, sizeof(c));
}

static void write_data(uint8_t c) {
    nrf_gpio_pin_set(ST7789_DC_PIN);
    spi_write(&c, sizeof(c));
}

static void write_data_buffered(uint8_t *c, uint16_t len, st7789_event_handler_t cb_handler, void* p_context) {
    nrf_gpio_pin_set(ST7789_DC_PIN);
    spi_write16_cb(c, len, cb_handler, p_context);
}

static void set_addr_window(uint16_t x_0, uint16_t y_0, uint16_t x_1, uint16_t y_1) {
    ASSERT(x_0 <= x_1);
    ASSERT(y_0 <= y_1);

    y_0 += 34;
    y_1 += 34;

    // x_0 += 34;
    // x_1 += 34;

    write_command(ST7789_CASET);

    write_data((x_0 >> 8) & 0xFF);
    write_data((x_0) & 0xFF);
    write_data((x_1 >> 8) & 0xFF);
    write_data((x_1) & 0xFF);

    write_command(ST7789_RASET);

    write_data((y_0 >> 8) & 0xFF);
    write_data((y_0) & 0xFF);
    write_data((y_1 >> 8) & 0xFF);
    write_data((y_1) & 0xFF);

    write_command(ST7789_RAMWR);
}


static void command_list(void) {
//************* Start Initial Sequence **********//
    write_command(0x11); //Sleep out
    nrf_delay_ms(120);              //Delay 120ms
    //************* Start Initial Sequence **********//
    write_command(0x36);

    //rotate
    write_data(70);

    write_command(0x3A);
    write_data(0x05);

    write_command(0xB2);
    write_data(0x0C);
    write_data(0x0C);
    write_data(0x00);
    write_data(0x33);
    write_data(0x33);

    write_command(0xB7);
    write_data(0x35);

    write_command(0xBB);
    write_data(0x32); //Vcom=1.35V

    write_command(0xC2);
    write_data(0x01);

    write_command(0xC3);
    write_data(0x15); //GVDD=4.8V  颜色深度

    write_command(0xC4);
    write_data(0x20); //VDV, 0x20:0v

    write_command(0xC6);
    write_data(0x0F); //0x0F:60Hz

    write_command(0xD0);
    write_data(0xA4);
    write_data(0xA1);

    write_command(0xE0);
    write_data(0xD0);
    write_data(0x08);
    write_data(0x0E);
    write_data(0x09);
    write_data(0x09);
    write_data(0x05);
    write_data(0x31);
    write_data(0x33);
    write_data(0x48);
    write_data(0x17);
    write_data(0x14);
    write_data(0x15);
    write_data(0x31);
    write_data(0x34);

    write_command(0xE1);
    write_data(0xD0);
    write_data(0x08);
    write_data(0x0E);
    write_data(0x09);
    write_data(0x09);
    write_data(0x15);
    write_data(0x31);
    write_data(0x33);
    write_data(0x48);
    write_data(0x17);
    write_data(0x14);
    write_data(0x15);
    write_data(0x31);
    write_data(0x34);
    write_command(0x21);

    write_command(0x29);
}

static void command_list2(){
//************* Start Initial Sequence **********//
    write_command(0x11); //Sleep out
    nrf_delay_ms(120);              //Delay 120ms
    //************* Start Initial Sequence **********//
    	write_command(0x11); 
//	delay_ms(120); 
	write_command(0x36); 

    write_data(0x70);

    
	// if(USE_HORIZONTAL==0)write_data(0x00);
	// else if(USE_HORIZONTAL==1)write_data(0xC0);
	// else if(USE_HORIZONTAL==2)write_data(0x70);
	// else write_data(0xA0);

	write_command(0x3A);
	write_data(0x05);

	write_command(0xB2);
	write_data(0x0C);
	write_data(0x0C);
	write_data(0x00);
	write_data(0x33);
	write_data(0x33); 

	write_command(0xB7); 
	write_data(0x35);  

	write_command(0xBB);
	write_data(0x35);

	write_command(0xC0);
	write_data(0x2C);

	write_command(0xC2);
	write_data(0x01);

	write_command(0xC3);
	write_data(0x13);   

	write_command(0xC4);
	write_data(0x20);  

	write_command(0xC6); 
	write_data(0x0F);    

	write_command(0xD0); 
	write_data(0xA4);
	write_data(0xA1);

	write_command(0xD6); 
	write_data(0xA1);

	write_command(0xE0);
	write_data(0xF0);
	write_data(0x00);
	write_data(0x04);
	write_data(0x04);
	write_data(0x04);
	write_data(0x05);
	write_data(0x29);
	write_data(0x33);
	write_data(0x3E);
	write_data(0x38);
	write_data(0x12);
	write_data(0x12);
	write_data(0x28);
	write_data(0x30);

	write_command(0xE1);
	write_data(0xF0);
	write_data(0x07);
	write_data(0x0A);
	write_data(0x0D);
	write_data(0x0B);
	write_data(0x07);
	write_data(0x28);
	write_data(0x33);
	write_data(0x3E);
	write_data(0x36);
	write_data(0x14);
	write_data(0x14);
	write_data(0x29);
	write_data(0x32);
	
// 	write_command(0x2A);
//	write_data(0x00);
//	write_data(0x22);
//	write_data(0x00);
//	write_data(0xCD);
//	write_data(0x2B);
//	write_data(0x00);
//	write_data(0x00);
//	write_data(0x01);
//	write_data(0x3F);
//	write_command(0x2C);
	write_command(0x21); 
  
  write_command(0x11);
  nrf_delay_ms(120);	
	write_command(0x29); 

    NRF_LOG_INFO("lcd command list");

}

static void gpio_init(void) {
    nrf_gpio_cfg_output(ST7789_DC_PIN);
    nrf_gpio_cfg_output(ST7789_RST_PIN);
    nrf_gpio_cfg_output(ST7789_LEDA_PIN);

    nrf_gpio_pin_set(ST7789_LEDA_PIN);

    nrf_gpio_pin_set(ST7789_RST_PIN);
    nrf_delay_ms(100);
    nrf_gpio_pin_clear(ST7789_RST_PIN);
    nrf_delay_ms(100);
    nrf_gpio_pin_set(ST7789_RST_PIN);
}



static ret_code_t hardware_init(void) {
    m_dev.cs_pin = ST7789_CS_PIN;
    hal_spi_bus_attach(&m_dev);
    gpio_init();

    return NRF_SUCCESS;
}


ret_code_t st7789_init(void) {
    ret_code_t err_code;

    err_code = hardware_init();
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    command_list2();

    return err_code;
}

static void st7789_uninit(void) {
    //nrfx_spim_uninit(&spi);
}

void st7789_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    set_addr_window(x, y, x + width - 1, y + height - 1);

    uint8_t data[2] = {color >> 8, color};

    nrf_gpio_pin_set(ST7789_DC_PIN);

    // Duff's device algorithm for optimizing loop.
    uint32_t i = (height * width + 7) / 8;

/*lint -save -e525 -e616 -e646 */
    switch ((height * width) % 8) {
        case 0:
            do {
                spi_write(data, sizeof(data));
                case 7:
                    spi_write(data, sizeof(data));
                case 6:
                    spi_write(data, sizeof(data));
                case 5:
                    spi_write(data, sizeof(data));
                case 4:
                    spi_write(data, sizeof(data));
                case 3:
                    spi_write(data, sizeof(data));
                case 2:
                    spi_write(data, sizeof(data));
                case 1:
                    spi_write(data, sizeof(data));
            } while (--i > 0);
        default:
            break;
    }
/*lint -restore */

    nrf_gpio_pin_clear(ST7789_DC_PIN);
}

void st7789_display_area(uint8_t *data, uint16_t len, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                         st7789_event_handler_t handler, void* p_context) {
    //NRF_LOG_INFO("Display area: (%d,%d), (%d, %d): %d bytes", x0, y0, x1, y1, len);
    set_addr_window(x0, y0, x1, y1);
    write_data_buffered(data, len, handler, p_context);
}


static void st7789_rotation_set(nrf_lcd_rotation_t rotation) {
    write_command(ST7789_MADCTL);
    switch (rotation % 4) {
        case NRF_LCD_ROTATE_0:
            write_data(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);  //Not working correctly
            //Column address (MX): Right to left
            //Page address (MY): Bottom to top
            //Page/ Column order (MV): normal
            //RGB/BGR order: RGB
            break;
        case NRF_LCD_ROTATE_90:
            write_data(ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
            //Column address (MX): Left to right
            //Page address (MY): Top to bottom
            //Page/ Column order (MV): reverse
            //RGB/BGR order: RGB
            break;
        case NRF_LCD_ROTATE_180:
            write_data(ST7789_MADCTL_RGB);
            //Column address (MX): Left to right
            //Page address (MY): Top to bottom
            //Page/ Column order (MV): normal
            //RGB/BGR order: RGB
            break;
        case NRF_LCD_ROTATE_270:
            write_data(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
            //Column address (MX): Right to left
            //Page address (MY): Top to bottom
            //Page/ Column order (MV): reverse
            //RGB/BGR order: RGB
            break;
        default:
            break;
    }
}

static void st7789_display_invert(bool invert) {
    write_command(invert ? ST7789_INVON : ST7789_INVOFF);
}

void st7789_sleep(void) {
    write_command(0x10);
    nrf_gpio_pin_clear(ST7789_LEDA_PIN);
}

#endif // NRF_MODULE_ENABLED(ST7789)