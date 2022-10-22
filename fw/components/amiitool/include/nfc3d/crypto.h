/*
 * (c) 2020-2021 nitz — chris marc dailey https://cmd.wtf
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef HAVE_NFC3D_CRYPTO_H
#define HAVE_NFC3D_CRYPTO_H

#include "app_error.h"
#include "nrf_crypto.h"
#include "nrf_crypto_error.h"
#include "nrf_log.h"

#define AES_ERROR_CHECK(error)                                    \
	do                                                            \
	{                                                             \
		if (error)                                                \
		{                                                         \
			NRF_LOG_RAW_INFO("\r\nError = 0x%x\r\n%s\r\n",        \
							 (error),                             \
							 nrf_crypto_error_string_get(error)); \
			return;                                               \
		}                                                         \
	} while (0);

#endif // HAVE_NFC3D_CRYPTO_H
