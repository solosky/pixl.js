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
#ifndef APP_USBD_MTP_H__
#define APP_USBD_MTP_H__

#include <stdint.h>
#include <stdbool.h>

#include "nrf_drv_usbd.h"
#include "app_usbd_class_base.h"
#include "app_usbd.h"
#include "app_usbd_core.h"
#include "app_usbd_descriptor.h"

#include "app_usbd_mtp_types.h"
#include "app_usbd_mtp_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup app_usbd_mtp USB Dummy class
 * @ingroup app_usbd
 *
 * @brief @tagAPI52840 Module with types, definitions,
 *                     and API used by the USB Dummy class.
 *
 * @note Class is used as a filler to provide correct alignement of interfaces.
 *
 * @{
 */

#ifdef DOXYGEN
/**
 * @brief Dummy class instance type.
 *
 * @ref APP_USBD_CLASS_TYPEDEF
 */
typedef struct { } app_usbd_mtp_t;
#else
/*lint -save -e10 -e26 -e123 -e505 */
APP_USBD_CLASS_TYPEDEF(app_usbd_mtp,                                             \
                             APP_USBD_MTP_CONFIG(0, 0, 0, 0),                                   \
                             APP_USBD_MTP_INSTANCE_SPECIFIC_DEC,                       \
                             APP_USBD_MTP_DATA_SPECIFIC_DEC                            \
);
#endif
/*lint -restore*/

/*!
 *
 * @brief       : Microsoft WCID descriptor
 */
typedef struct
{
    uint8_t bFirstInterfaceNumber; /*< first interface number */
    uint8_t reserved1; /*< reversed 1 */
    uint8_t compatibleID[8]; /*< compatible ID */
    uint8_t subCompatibleID[8]; /*< subcompatible ID */
    uint8_t reserved2[6]; /*< reversed 2*/
}__attribute__((packed)) MicrosoftCompatibleDescriptor_Interface;
/*!
 * @brief		:structure for Microsoft compatible descriptor
 */
typedef struct
{
    uint32_t dwLength; /*< length */
    uint16_t bcdVersion; /*< bcd version */
    uint16_t wIndex; /*< index */
    uint8_t bCount; /*< count */
    uint8_t reserved[7];/*< reserved */
    MicrosoftCompatibleDescriptor_Interface interfaces[];
}__attribute__((packed)) MicrosoftCompatibleDescriptor;



/**
 * @brief Global definition of app_usbd_mtp_t class.
 *
 * @param instance_name         Name of global instance.
 * @param interface_number      Unique interface number.
 *
 * @note This macro is just simplified version of @ref APP_USBD_MTP_GLOBAL_DEF_INTERNAL
 *
 */
/*lint -save -emacro(26 64 123 505 651, APP_USBD_MTP_GLOBAL_DEF)*/

#define APP_USBD_MTP_GLOBAL_DEF(instance_name, interface_number, comm_ein, data_ein, data_eout) \
        APP_USBD_MTP_GLOBAL_DEF_INTERNAL(instance_name, interface_number, comm_ein, data_ein, data_eout)

/*lint -restore*/


void app_usbd_mtp_init(app_usbd_mtp_t const * p_mtp);

/**
 * @@brief Helper function to get class instance from mtp.
 *
 * @param[in] p_mtp Dummy instance
 *                    (declared by @ref APP_USBD_MTP_GLOBAL_DEF).
 *
 * @return Base class instance.
 */
static inline app_usbd_class_inst_t const *
app_usbd_mtp_class_inst_get(app_usbd_mtp_t const * p_mtp)
{
    return &p_mtp->base;
}

/**
 * @brief Helper function to get mtp from base class instance.
 *
 * @param[in] p_inst Base class instance.
 *
 * @return Dummy class handle.
 */
static inline app_usbd_mtp_t const *
app_usbd_mtp_class_get( app_usbd_class_inst_t const * p_inst)
{
    return (app_usbd_mtp_t const *)p_inst;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* APP_USBD_MTP_H__ */
