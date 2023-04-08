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
#ifndef APP_USBD_MTP_INTERNAL_H__
#define APP_USBD_MTP_INTERNAL_H__

#include "mtp_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup app_usbd_mtp_internals USB Dummy internals
 * @ingroup app_usbd_mtp
 *
 * @brief @tagAPI52840 Internals of the USB Dummy class.
 * @{
 */

/** @brief Forward declaration of Dummy Class type */
APP_USBD_CLASS_FORWARD(app_usbd_mtp);


/** @brief Dummy part of class instance data */
typedef struct {
    uint8_t iface;
    uint8_t comm_epin;          //!< COMM subclass IN endpoint.
    uint8_t data_epout;         //!< DATA subclass OUT endpoint.
    uint8_t data_epin;          //!< DATA subclass IN endpoint.
} app_usbd_mtp_inst_t;

/** @brief Dummy context */
typedef struct {
    struct mtp_core mtp;
    uint8_t buffer_data[16];
    mtp_buffer_t buffer;
    uint16_t request_type;
} app_usbd_mtp_ctx_t;


/**
 * @brief Dummy configuration macro.
 *
 * Used by @ref APP_USBD_MTP_GLOBAL_DEF
 *
 * @param iface Interface number.
 * */
#define APP_USBD_MTP_CONFIG(iface, comm_ein, data_ein, data_eout) ((iface, comm_ein, data_ein, data_eout))


/**
 * @brief Specific class constant data for Dummy.
 *
 * @ref app_usbd_mtp_inst_t
 */
#define APP_USBD_MTP_INSTANCE_SPECIFIC_DEC app_usbd_mtp_inst_t inst;


#define APP_USBD_MTP_INST_CONFIG( iface1, \
                                    comm_ein1,                                                   \
                                    data_ein1,                                                   \
                                    data_eout1)                                                  \
        .inst = {                                                                                \
                .iface  = iface1,                                                     \
                .comm_epin       = comm_ein1,                                                     \
                .data_epin       = data_ein1,                                                     \
                .data_epout      = data_eout1                                                    \
        }


/**
 * @brief Specific class data for Dummy.
 *
 * @ref app_usbd_mtp_ctx_t
 * */
#define APP_USBD_MTP_DATA_SPECIFIC_DEC app_usbd_mtp_ctx_t ctx;


/** @brief Public Dummy class interface */
extern const app_usbd_class_methods_t app_usbd_mtp_class_methods;

/** @brief Global definition of Dummy instance */
#define APP_USBD_MTP_GLOBAL_DEF_INTERNAL(instance_name,                 \
                                        interface_number,               \
                                        comm_ein,                       \
                                        data_ein,                       \
                                        data_eout)                      \
    APP_USBD_CLASS_INST_GLOBAL_DEF(                                                          \
        instance_name,                                                                             \
        app_usbd_mtp,                                                                            \
        &app_usbd_mtp_class_methods,                                                             \
        APP_USBD_MTP_CONFIG(interface_number, comm_ein, data_ein, data_eout),                                                 \
        (APP_USBD_MTP_INST_CONFIG(interface_number, comm_ein, data_ein, data_eout))            \
    )


/** @} */



#ifdef __cplusplus
}
#endif

#endif /* APP_USBD_MTP_INTERNAL_H__ */
