/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "bsp/board_api.h"
// #include "board.h"

// Suppress warning caused by mcu driver

#include "nrf_log.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "nrfx.h"
#include "nrfx_power.h"

#include "tusb.h"

#include "app_cmd.h"
#include "dataframe.h"

//--------------------------------------------------------------------+
// Forward USB interrupt events to TinyUSB IRQ Handler
//--------------------------------------------------------------------+
void USBD_IRQHandler(void) { tud_int_handler(0); }

/*------------------------------------------------------------------*/
/* MACRO TYPEDEF CONSTANT ENUM
 *------------------------------------------------------------------*/
// tinyusb function that handles power event (detected, ready, removed)
// We must call it within SD's SOC event handler, or set it as power event handler if SD is not enabled.
extern void tusb_hal_nrf_power_event(uint32_t event);
// Value is chosen to be as same as NRFX_POWER_USB_EVT_* in nrfx_power.h
enum { USB_EVT_DETECTED = 0, USB_EVT_REMOVED = 1, USB_EVT_READY = 2 };
#define VBUSDETECT_Msk POWER_USBREGSTATUS_VBUSDETECT_Msk
#define OUTPUTRDY_Msk POWER_USBREGSTATUS_OUTPUTRDY_Msk

//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+

void board_init(void) {

    // init dataframe
    //  cmd callback register
    on_data_frame_complete(on_data_frame_received);

    // Priorities 0, 1, 4 (nRF52) are reserved for SoftDevice
    // 2 is highest for application
    NVIC_SetPriority(USBD_IRQn, 2);

    // USB power may already be ready at this time -> no event generated
    // We need to invoke the handler based on the status initially
    uint32_t usb_reg = 0;

    sd_power_usbdetected_enable(true);
    sd_power_usbpwrrdy_enable(true);
    sd_power_usbremoved_enable(true);

    sd_power_usbregstatus_get(&usb_reg);
    if (usb_reg & VBUSDETECT_Msk) tusb_hal_nrf_power_event(USB_EVT_DETECTED);
    if (usb_reg & OUTPUTRDY_Msk) tusb_hal_nrf_power_event(USB_EVT_READY);
}

size_t board_get_unique_id(uint8_t id[], size_t max_len) {
    (void)max_len;
    uintptr_t did_addr = (uintptr_t)NRF_FICR->DEVICEID;
    const uint8_t *device_id = (const uint8_t *)did_addr;
    for (uint8_t i = 0; i < 8; i++) {
        id[i] = device_id[i];
    }
    return 8;
}

int board_uart_read(uint8_t *buf, int len) {
    (void)buf;
    (void)len;
    return 0;
    //  return NRFX_SUCCESS == nrfx_uart_rx(&_uart_id, buf, (size_t) len) ? len : 0;
}

int board_uart_write(void const *buf, int len) {
    // return (NRFX_SUCCESS == nrfx_uarte_tx(&_uart_id, (uint8_t const *)buf, (size_t)len)) ? len : 0;
}

static void tusb_power_sdh_soc_evt_handler(uint32_t evt_id, void *p_context);

NRF_SDH_SOC_OBSERVER(m_soc_tusb, POWER_CONFIG_SOC_OBSERVER_PRIO, tusb_power_sdh_soc_evt_handler, NULL);
static void tusb_power_sdh_soc_evt_handler(uint32_t evt_id, void *p_context) {
    int32_t usbevt = (evt_id == NRF_EVT_POWER_USB_DETECTED)      ? NRFX_POWER_USB_EVT_DETECTED
                     : (evt_id == NRF_EVT_POWER_USB_POWER_READY) ? NRFX_POWER_USB_EVT_READY
                     : (evt_id == NRF_EVT_POWER_USB_REMOVED)     ? NRFX_POWER_USB_EVT_REMOVED
                                                                 : -1;

    if (usbevt >= 0) {
        tusb_hal_nrf_power_event(usbevt);
    }
}

void nrf_error_cb(uint32_t id, uint32_t pc, uint32_t info) {
    (void)id;
    (void)pc;
    (void)info;
}

void tud_cdc_rx_cb(uint8_t itf) {
    // Take out the first byte first, WHY ???
    NRF_LOG_INFO("cdc rx: %d", tud_cdc_available());
    static uint8_t cdc_data_buffer[256];
    uint32_t ret = 0;
    do {
        ret = tud_cdc_read(cdc_data_buffer, sizeof(cdc_data_buffer));
        if (ret > 0) {
            // The byte after success
            NRF_LOG_HEXDUMP_DEBUG(cdc_data_buffer, ret);
            data_frame_receive(cdc_data_buffer, ret);
        }
    } while (ret > 0);
}

const uint8_t *buffer;
size_t size;
size_t avaliable;
void tud_cdc_write_begin(const uint8_t *buff, size_t data_size) {
    buffer = buff;
    size = data_size;
    avaliable = data_size;
    NRF_LOG_INFO("cdc write begin: size: %d", data_size);
    NRF_LOG_HEXDUMP_DEBUG(buff, data_size);
    tud_cdc_tx_complete_cb(0);
}

void tud_cdc_tx_complete_cb(uint8_t itf) {
    if (buffer && avaliable > 0) {
        size_t written_bytes = tud_cdc_write(buffer, avaliable);
        tud_cdc_write_flush();
        avaliable -= written_bytes;
        buffer += written_bytes;
        NRF_LOG_INFO("cdc write: written: %d, avaliable: %d", written_bytes, avaliable);
    } else {
        buffer = NULL;
        size = 0;
        avaliable = 0;
        NRF_LOG_INFO("cdc write complete");
    }
}