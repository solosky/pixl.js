/**
 * Copyright (c) 2017 - 2020, Nordic Semiconductor ASA
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

#if 1//NRF_MODULE_ENABLED(APP_USBD_MTP)

#include <string.h>
#include <ctype.h>
#include "app_usbd.h"
#include "app_usbd_mtp.h"
#include "app_usbd_string_desc.h"
#include "nrf_gpio.h"
#include "nrf_log_ctrl.h"

#include "mtp_codec.h"

/**
 * @defgroup app_usbd_mtp_internal USBD Dummy internals
 * @{
 * @ingroup app_usbd_mtp
 * @internal
 */

#define NRF_LOG_MODULE_NAME usbd_mtp

#if APP_USBD_MTP_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL       APP_USBD_MTP_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR  APP_USBD_MTP_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR APP_USBD_MTP_CONFIG_DEBUG_COLOR
#else //APP_USBD_MTP_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL       3
#endif //APP_USBD_MTP_CONFIG_LOG_ENABLED

#include "nrf_log.h"

NRF_LOG_MODULE_REGISTER();

typedef enum {
    MTP_SETUP_CANCEL = 0x64,
    MTP_SETUP_RESET = 0x66,
    MTP_SETUP_GET_DEVICE_STATUS = 0x67
} mtp_setup_req_type_t;

const MicrosoftCompatibleDescriptor msft_compatible = {
        .dwLength = sizeof(MicrosoftCompatibleDescriptor) + sizeof(MicrosoftCompatibleDescriptor_Interface),
        .bcdVersion = 0x0100,
        .wIndex = 0x0004,
        .bCount = 1,
        .reserved = {0, 0, 0, 0, 0, 0, 0},
        .interfaces = {
                {
                        .bFirstInterfaceNumber = 0,
                        .reserved1 = 0,
                        .compatibleID = "MTP",
                        .subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
                        .reserved2 = {0, 0, 0, 0, 0, 0},
                }
        }
};

/**
 * @brief Class specific OUT request data callback.
 *
 * @param status    Endpoint status.
 * @param p_context Context of transfer (set by @ref app_usbd_core_setup_data_handler_set).
 *
 * @return Standard error code.
 */
static ret_code_t app_usbd_mtp_req_out_data_cb(nrf_drv_usbd_ep_status_t status, void *p_context) {
    if (status != NRF_USBD_EP_OK) {
        return NRF_ERROR_INTERNAL;
    }

    app_usbd_mtp_t const *p_app_usb_mtp = (app_usbd_mtp_t const *) p_context;
    app_usbd_mtp_ctx_t *ctx = &p_app_usb_mtp->specific.p_data->ctx;
    mtp_codec_t *codec = &ctx->mtp.codec;
    mtp_buffer_t *buffer = &ctx->buffer;

    switch (ctx->request_type) {
        case MTP_SETUP_CANCEL: {
            uint16_t cancellation_code = mtp_buff_get_u16(buffer);
            uint32_t transaction_id = mtp_buff_get_u32(buffer);

            NRF_LOG_INFO("mtp cancel tx: code=%d, tx_id=%d", cancellation_code, transaction_id);
            mtp_codec_cancel(codec, transaction_id);
        }
            break;
    }

    return NRF_SUCCESS;
}


/**
 * @brief Internal SETUP standard IN request handler.
 *
 * @param[in] p_inst        Generic class instance.
 * @param[in] p_setup_ev    Setup event.
 *
 * @return Standard error code.
 */
static ret_code_t setup_req_std_in(app_usbd_class_inst_t const *p_inst,
                                   app_usbd_setup_evt_t const *p_setup_ev) {
    /* Only Get Descriptor standard IN request is supported by CDC class */
    NRF_LOG_INFO("setup_req_std_in:%d",app_usbd_setup_req_rec(p_setup_ev->setup.bmRequestType) );
    if ((app_usbd_setup_req_rec(p_setup_ev->setup.bmRequestType) == APP_USBD_SETUP_REQREC_INTERFACE)
        &&
        (p_setup_ev->setup.bRequest == APP_USBD_SETUP_STDREQ_GET_DESCRIPTOR)) {
        size_t dsc_len = 0;
        size_t max_size;

        uint8_t *p_trans_buff = app_usbd_core_setup_transfer_buff_get(&max_size);

        /* Try to find descriptor in class internals*/
        ret_code_t ret = app_usbd_class_descriptor_find(
                p_inst,
                p_setup_ev->setup.wValue.hb,
                p_setup_ev->setup.wValue.lb,
                p_trans_buff,
                &dsc_len);

        if (ret != NRF_ERROR_NOT_FOUND) {
            ASSERT(dsc_len < NRF_DRV_USBD_EPSIZE);
            return app_usbd_core_setup_rsp(&(p_setup_ev->setup), p_trans_buff, dsc_len);
        }
    }

    NRF_LOG_INFO("setup_req_std_in not supported");
    return NRF_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Internal SETUP standard OUT request handler.
 *
 * @param[in] p_inst        Generic class instance.
 * @param[in] p_setup_ev    Setup event.
 *
 * @return Standard error code.
 */
static ret_code_t setup_req_std_out(app_usbd_class_inst_t const *p_inst,
                                    app_usbd_setup_evt_t const *p_setup_ev) {

    switch (p_setup_ev->setup.bRequest) {
        default:
            break;
    }

    return NRF_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Internal SETUP class IN request handler.
 *
 * @param[in] p_inst        Generic class instance.
 * @param[in] p_setup_ev    Setup event.
 *
 * @return Standard error code.
 */
static ret_code_t setup_req_class_in(app_usbd_class_inst_t const *p_inst,
                                     app_usbd_setup_evt_t const *p_setup_ev) {
    app_usbd_mtp_t const *p_app_usb_mtp = app_usbd_mtp_class_get(p_inst);
    app_usbd_mtp_ctx_t *ctx = &p_app_usb_mtp->specific.p_data->ctx;
    mtp_codec_t *codec = &ctx->mtp.codec;
    MTP_NEW_BUFFER(buffer, ctx->buffer_data, sizeof(ctx->buffer_data));

    NRF_LOG_INFO("setup_req_class_in:%X", p_setup_ev->setup.bRequest);
    switch (p_setup_ev->setup.bRequest) {

        case MTP_SETUP_RESET:
            NRF_LOG_INFO("MTP_SETUP_RESET");
            mtp_codec_reset(codec);
            app_usbd_core_setup_rsp(&p_setup_ev->setup, NULL, 0);
            break;

        case MTP_SETUP_GET_DEVICE_STATUS:
            NRF_LOG_INFO("MTP_SETUP_GET_DEVICE_STATUS");
            mtp_buff_put_u16(&buffer, 4); // total length
            mtp_buff_put_u16(&buffer, 0x2001); //status OK
            app_usbd_core_setup_rsp(&p_setup_ev->setup,
                                    mtp_buff_get_data(&buffer),
                                    mtp_buff_get_size(&buffer));
            break;

        default:
            break;
    }

    return NRF_ERROR_NOT_SUPPORTED;
}

/**
 * @brief vendor SETUP class IN request handler.
 *
 * @param[in] p_inst        Generic class instance.
 * @param[in] p_setup_ev    Setup event.
 *
 * @return Standard error code.
 */
static ret_code_t setup_req_vendor_in(app_usbd_class_inst_t const *p_inst,
                                      app_usbd_setup_evt_t const *p_setup_ev) {
    app_usbd_mtp_t const *p_app_usb_mtp = app_usbd_mtp_class_get(p_inst);
    NRF_LOG_INFO("setup_req_vendor_in");

    switch (p_setup_ev->setup.bRequest) {
        case GET_MS_DESCRIPTOR: {
            if (p_setup_ev->setup.wIndex.w == 0x0004) {

                return app_usbd_core_setup_rsp(&p_setup_ev->setup,
                                               &msft_compatible,
                                               p_setup_ev->setup.wLength.w);
            }
        }
        default:
            break;
    }

    return NRF_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Internal SETUP class OUT request handler.
 *
 * @param[in] p_inst        Generic class instance.
 * @param[in] p_setup_ev    Setup event.
 *
 * @return Standard error code.
 */
static ret_code_t setup_req_class_out(app_usbd_class_inst_t const *p_inst,
                                      app_usbd_setup_evt_t const *p_setup_ev) {

    NRF_LOG_INFO("setup_req_class_out:%X", p_setup_ev->setup.bRequest);

    app_usbd_mtp_t const *p_app_usb_mtp = app_usbd_mtp_class_get(p_inst);
    app_usbd_mtp_ctx_t *ctx = &p_app_usb_mtp->specific.p_data->ctx;

    switch (p_setup_ev->setup.bRequest) {
        case MTP_SETUP_CANCEL:
            NRF_LOG_INFO("MTP_SETUP_CANCEL");

            ctx->request_type = p_setup_ev->setup.bRequest;
            mtp_buff_reset(&ctx->buffer);
            mtp_buff_set_limit(&ctx->buffer, p_setup_ev->setup.wLength.w);

            /*Request setup data*/
            NRF_DRV_USBD_TRANSFER_OUT(transfer,
                                      mtp_buff_get_data(&ctx->buffer),
                                      p_setup_ev->setup.wLength.w);
            ret_code_t ret;
            CRITICAL_REGION_ENTER();
                ret = app_usbd_ep_transfer(NRF_DRV_USBD_EPOUT0, &transfer);
                if (ret == NRF_SUCCESS) {
                    const app_usbd_core_setup_data_handler_desc_t desc = {
                            .handler = app_usbd_mtp_req_out_data_cb,
                            .p_context = (void *) p_app_usb_mtp
                    };

                    ret = app_usbd_core_setup_data_handler_set(NRF_DRV_USBD_EPOUT0, &desc);
                }
            CRITICAL_REGION_EXIT();
            return ret;
    }


    return NRF_ERROR_NOT_SUPPORTED;
}


/**
 * @brief Control endpoint handler.
 *
 * @param[in] p_inst        Generic class instance.
 * @param[in] p_setup_ev    Setup event.
 *
 * @return Standard error code.
 */
static ret_code_t setup_event_handler(app_usbd_class_inst_t const *p_inst,
                                      app_usbd_setup_evt_t const *p_setup_ev) {
    ASSERT(p_inst != NULL);
    ASSERT(p_setup_ev != NULL);

    NRF_LOG_INFO("setup: bRequest=%x, wValue=%x,%x, wLength=%d", p_setup_ev->setup.bRequest, p_setup_ev->setup.wValue.lb,
                 p_setup_ev->setup.wValue.hb, p_setup_ev->setup.wLength.w);

    if (app_usbd_setup_req_dir(p_setup_ev->setup.bmRequestType) == APP_USBD_SETUP_REQDIR_IN) {
        switch (app_usbd_setup_req_typ(p_setup_ev->setup.bmRequestType)) {
            case APP_USBD_SETUP_REQTYPE_STD:
                return setup_req_std_in(p_inst, p_setup_ev);
            case APP_USBD_SETUP_REQTYPE_CLASS:
                return setup_req_class_in(p_inst, p_setup_ev);
            case APP_USBD_SETUP_REQTYPE_VENDOR:
                return setup_req_vendor_in(p_inst, p_setup_ev);
            default:
                break;
        }
    } else /*APP_USBD_SETUP_REQDIR_OUT*/
    {
        switch (app_usbd_setup_req_typ(p_setup_ev->setup.bmRequestType)) {
            case APP_USBD_SETUP_REQTYPE_STD:
                return setup_req_std_out(p_inst, p_setup_ev);
            case APP_USBD_SETUP_REQTYPE_CLASS:
                return setup_req_class_out(p_inst, p_setup_ev);
            default:
                break;
        }
    }

    return NRF_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Class specific endpoint transfer handler.
 *
 * @param[in] p_inst        Generic class instance.
 * @param[in] p_setup_ev    Setup event.
 *
 * @return Standard error code.
 */
static ret_code_t mtp_endpoint_ev(app_usbd_class_inst_t const *p_inst,
                                  app_usbd_complex_evt_t const *p_event) {
    ret_code_t ret;
    app_usbd_mtp_t const *p_app_usb_mtp = app_usbd_mtp_class_get(p_inst);
    //NRF_LOG_INFO("endpoint_ev:%X, status:%X",p_event->drv_evt.data.eptransfer.ep, p_event->drv_evt.data.eptransfer.status);
    if (NRF_USBD_EPIN_CHECK(p_event->drv_evt.data.eptransfer.ep)) {
        switch (p_event->drv_evt.data.eptransfer.status) {
            case NRF_USBD_EP_OK:
                //NRF_LOG_INFO("EPIN_DATA: %x done", p_event->drv_evt.data.eptransfer.ep);
                mtp_codec_usbd_ep_in_data_sent(&p_app_usb_mtp->specific.p_data->ctx.mtp.codec, p_event);
                return NRF_SUCCESS;
            case NRF_USBD_EP_ABORTED:
                return NRF_SUCCESS;
            default:
                return NRF_ERROR_INTERNAL;
        }
    }

    if (NRF_USBD_EPOUT_CHECK(p_event->drv_evt.data.eptransfer.ep)) {
        switch (p_event->drv_evt.data.eptransfer.status) {
            case NRF_USBD_EP_OK:
                //NRF_LOG_INFO("EPOUT_DATA: %x done, size: %d", p_event->drv_evt.data.eptransfer.ep,
                //             nrfx_usbd_epout_size_get(p_event->drv_evt.data.eptransfer.ep));
                mtp_codec_usbd_ep_out_data_received(&p_app_usb_mtp->specific.p_data->ctx.mtp.codec, p_event);

                return NRF_SUCCESS;
            case NRF_USBD_EP_WAITING:
                //NRF_LOG_INFO("EPOUT_WATING: %x done, size:%d", p_event->drv_evt.data.eptransfer.ep,
                //             nrfx_usbd_epout_size_get(p_event->drv_evt.data.eptransfer.ep));
                mtp_codec_usbd_ep_out_data_ready(&p_app_usb_mtp->specific.p_data->ctx.mtp.codec, p_event);
                return NRF_SUCCESS;
            case NRF_USBD_EP_ABORTED:
                return NRF_SUCCESS;
            default:
                return NRF_ERROR_INTERNAL;
        }
    }

    return NRF_ERROR_NOT_SUPPORTED;
}


/** @brief @ref app_usbd_class_methods_t::event_handler */
static ret_code_t mtp_class_event_handler(app_usbd_class_inst_t const *p_inst,
                                          app_usbd_complex_evt_t const *p_event) {
    //NRF_LOG_INFO("mpt event: %d", p_event->app_evt.type);

    ASSERT(p_inst != NULL);
    ASSERT(p_event != NULL);

    ret_code_t ret = NRF_SUCCESS;
    switch (p_event->app_evt.type) {
        case APP_USBD_EVT_DRV_SOF:
            break;
        case APP_USBD_EVT_DRV_RESET:
            //cdc_acm_reset_port(p_inst);
            break;
        case APP_USBD_EVT_DRV_SETUP:
            ret = setup_event_handler(p_inst, (app_usbd_setup_evt_t const *) p_event);
            break;
        case APP_USBD_EVT_DRV_EPTRANSFER:
            ret = mtp_endpoint_ev(p_inst, p_event);
            break;
        case APP_USBD_EVT_DRV_SUSPEND:
            break;
        case APP_USBD_EVT_DRV_RESUME:
            break;
        case APP_USBD_EVT_INST_APPEND:
            break;
        case APP_USBD_EVT_INST_REMOVE:
            break;
        case APP_USBD_EVT_STARTED:
            break;
        case APP_USBD_EVT_STOPPED:
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            break;
        default:
            ret = NRF_ERROR_NOT_SUPPORTED;
            break;
    }

    return ret;

    if ((p_event->app_evt.type == APP_USBD_EVT_INST_APPEND) ||
        (p_event->app_evt.type == APP_USBD_EVT_INST_REMOVE)) {
        return NRF_SUCCESS;
    } else {
        return NRF_ERROR_NOT_SUPPORTED;
    }
}

/** @brief @ref app_usbd_class_methods_t::feed_descriptors */
static bool mtp_class_feed_descriptors(app_usbd_class_descriptor_ctx_t *p_ctx,
                                       app_usbd_class_inst_t const *p_inst,
                                       uint8_t *p_buff,
                                       size_t max_size) {
    static app_usbd_class_iface_conf_t const *p_cur_iface = 0;
    p_cur_iface = app_usbd_class_iface_get(p_inst, 0);
    static uint8_t ifaces = 0;
    ifaces = app_usbd_class_iface_count_get(p_inst);

    app_usbd_mtp_t const *p_mtp = app_usbd_mtp_class_get(p_inst);


    APP_USBD_CLASS_DESCRIPTOR_BEGIN(p_ctx, p_buff, max_size)

        /* INTERFACE DESCRIPTOR */
        APP_USBD_CLASS_DESCRIPTOR_WRITE(sizeof(app_usbd_descriptor_iface_t)); // bLength
        APP_USBD_CLASS_DESCRIPTOR_WRITE(APP_USBD_DESCRIPTOR_INTERFACE); // bDescriptorType
        APP_USBD_CLASS_DESCRIPTOR_WRITE(app_usbd_class_iface_number_get(p_cur_iface)); // bInterfaceNumber
        APP_USBD_CLASS_DESCRIPTOR_WRITE(0x00); // bAlternateSetting
        APP_USBD_CLASS_DESCRIPTOR_WRITE(3); // bNumEndpoints
        APP_USBD_CLASS_DESCRIPTOR_WRITE(APP_USBD_MTP_CLASS); // bInterfaceClass
        APP_USBD_CLASS_DESCRIPTOR_WRITE(APP_USBD_MTP_SUBCLASS); // bInterfaceSubClass
        APP_USBD_CLASS_DESCRIPTOR_WRITE(APP_USBD_MTP_PROTOCOL); // bInterfaceProtocol
        APP_USBD_CLASS_DESCRIPTOR_WRITE(0x92); // iInterface //TODO "MTP" string descriptor


        /* ENDPOINT DESCRIPTORS */
        static uint8_t endpoints = 0;
        endpoints = app_usbd_class_iface_ep_count_get(p_cur_iface);
        static uint8_t j = 0;

        for (j = 0; j < endpoints; j++) {
            APP_USBD_CLASS_DESCRIPTOR_WRITE(0x07);                         // bLength
            APP_USBD_CLASS_DESCRIPTOR_WRITE(APP_USBD_DESCRIPTOR_ENDPOINT); // bDescriptorType = Endpoint

            static app_usbd_class_ep_conf_t const *p_cur_ep = NULL;
            p_cur_ep = app_usbd_class_iface_ep_get(p_cur_iface, j);
            APP_USBD_CLASS_DESCRIPTOR_WRITE(app_usbd_class_ep_address_get(p_cur_ep)); // bEndpointAddress

            nrf_drv_usbd_ep_t ep_address = app_usbd_class_ep_address_get(p_cur_ep);

            if (p_mtp->specific.inst.comm_epin == ep_address) {
                APP_USBD_CLASS_DESCRIPTOR_WRITE(APP_USBD_DESCRIPTOR_EP_ATTR_TYPE_INTERRUPT); // bmAttributes
            } else {
                APP_USBD_CLASS_DESCRIPTOR_WRITE(APP_USBD_DESCRIPTOR_EP_ATTR_TYPE_BULK); // bmAttributes
            }
            APP_USBD_CLASS_DESCRIPTOR_WRITE(LSB_16(NRF_DRV_USBD_EPSIZE)); // wMaxPacketSize LSB
            APP_USBD_CLASS_DESCRIPTOR_WRITE(MSB_16(NRF_DRV_USBD_EPSIZE)); // wMaxPacketSize MSB

            if (p_mtp->specific.inst.comm_epin == ep_address) {
                APP_USBD_CLASS_DESCRIPTOR_WRITE(APP_USER_MTP_INTERRUPT_INTERVAL); // bInterval
            } else {
                APP_USBD_CLASS_DESCRIPTOR_WRITE(0x00); // bInterval
            }
        }

    APP_USBD_CLASS_DESCRIPTOR_END();
}

void app_usbd_mtp_init(app_usbd_mtp_t const *p_app_usb_mtp) {
    mtp_init_t init = {
            .comm_ep_in = p_app_usb_mtp->specific.inst.comm_epin,
            .data_ep_in= p_app_usb_mtp->specific.inst.data_epin,
            .data_ep_out = p_app_usb_mtp->specific.inst.data_epout
    };
    mtp_core_init(&p_app_usb_mtp->specific.p_data->ctx.mtp, &init);

    app_usbd_mtp_ctx_t *p_ctx = &p_app_usb_mtp->specific.p_data->ctx;
    p_ctx->buffer.buff = p_ctx->buffer_data;
    p_ctx->buffer.cap = sizeof(p_ctx->buffer_data);
    p_ctx->buffer.pos = 0;
    p_ctx->buffer.limit = 0;

}


const app_usbd_class_methods_t app_usbd_mtp_class_methods = {
        .event_handler    = mtp_class_event_handler,
        .feed_descriptors = mtp_class_feed_descriptors
};


#endif //NRF_MODULE_ENABLED(APP_USBD_MTP)
