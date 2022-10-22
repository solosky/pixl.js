/*
 * (c) 2015-2017 Marcos Del Sol Vives
 * (c) 2016      javiMaD
 *
 * SPDX-License-Identifier: MIT
 */

#include "nfc3d/drbg.h"
#include <assert.h>
#include <string.h>

void nfc3d_drbg_init(nfc3d_drbg_ctx* ctx, const uint8_t* hmacKey, size_t hmacKeySize, const uint8_t* seed, size_t seedSize)
{
	ret_code_t ret_val = NRF_SUCCESS;

	assert(ctx != NULL);
	assert(hmacKey != NULL);
	assert(seed != NULL);
	assert(seedSize <= NFC3D_DRBG_MAX_SEED_SIZE);

	// Initialize primitives
	ctx->used = false;
	ctx->iteration = 0;
	ctx->bufferSize = sizeof(ctx->iteration) + seedSize;

	// store key
	ctx->hmacKey = hmacKey;
	ctx->hmacKeySize = hmacKeySize;

	// The 16-bit counter is prepended to the seed when hashing, so we'll leave 2 bytes at the start
	memcpy(ctx->buffer + sizeof(uint16_t), seed, seedSize);

	// Initialize underlying HMAC context
	ret_val = nrf_crypto_hmac_init(&ctx->hmacCtx,
								   &g_nrf_crypto_hmac_sha256_info,
								   ctx->hmacKey,
								   ctx->hmacKeySize);
	APP_ERROR_CHECK(ret_val);
}

void nfc3d_drbg_step(nfc3d_drbg_ctx* ctx, uint8_t* output)
{
	ret_code_t ret_val = NRF_SUCCESS;
	size_t digest_size = NFC3D_DRBG_OUTPUT_SIZE;

	assert(ctx != NULL);
	assert(output != NULL);

	if (ctx->used)
	{
		// If used at least once, reinitialize the HMAC
		ret_val = nrf_crypto_hmac_init(&ctx->hmacCtx,
									   &g_nrf_crypto_hmac_sha256_info,
									   ctx->hmacKey,
									   ctx->hmacKeySize);
		APP_ERROR_CHECK(ret_val);
	}
	else
	{
		ctx->used = true;
	}

	// Store counter in big endian, and increment it
	ctx->buffer[0] = ctx->iteration >> 8;
	ctx->buffer[1] = ctx->iteration >> 0;
	ctx->iteration++;

	// Do HMAC magic
	nrf_crypto_hmac_update(&ctx->hmacCtx, ctx->buffer, ctx->bufferSize);
	nrf_crypto_hmac_finalize(&ctx->hmacCtx, output, &digest_size);
}

void nfc3d_drbg_cleanup(nfc3d_drbg_ctx* ctx)
{
	assert(ctx != NULL);
}

void nfc3d_drbg_generate_bytes(const uint8_t* hmacKey, size_t hmacKeySize, const uint8_t* seed, size_t seedSize, uint8_t* output, size_t outputSize)
{
	uint8_t temp[NFC3D_DRBG_OUTPUT_SIZE];

	nfc3d_drbg_ctx rngCtx;
	nfc3d_drbg_init(&rngCtx, hmacKey, hmacKeySize, seed, seedSize);

	while (outputSize > 0)
	{
		if (outputSize < NFC3D_DRBG_OUTPUT_SIZE)
		{
			nfc3d_drbg_step(&rngCtx, temp);
			memcpy(output, temp, outputSize);
			break;
		}

		nfc3d_drbg_step(&rngCtx, output);
		output += NFC3D_DRBG_OUTPUT_SIZE;
		outputSize -= NFC3D_DRBG_OUTPUT_SIZE;
	}

	nfc3d_drbg_cleanup(&rngCtx);
}
