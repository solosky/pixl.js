/*
 * (c) 2020-2021 nitz — chris marc dailey https://cmd.wtf
 * (c) 2015-2017 Marcos Del Sol Vives
 * (c) 2016      javiMaD
 *
 * SPDX-License-Identifier: MIT
 */

#include <errno.h>

#include "nfc3d/amiibo.h"
#include "nfc3d/crypto.h"
#include "portable_endian.h"

#define HMAC_POS_DATA 0x008
#define HMAC_POS_TAG 0x1B4
#define HMAC_BUFFER_LENGTH 0x20

#define AMIIBO_CIPHER_LENGTH 0x188

void nfc3d_amiibo_calc_seed(const uint8_t* dump, uint8_t* key)
{
	memcpy(key + 0x00, dump + 0x029, 0x02);
	memset(key + 0x02, 0x00, 0x0E);
	memcpy(key + 0x10, dump + 0x1D4, 0x08);
	memcpy(key + 0x18, dump + 0x1D4, 0x08);
	memcpy(key + 0x20, dump + 0x1E8, 0x20);
}

void nfc3d_amiibo_keygen(const nfc3d_keygen_masterkeys* masterKeys, const uint8_t* dump, nfc3d_keygen_derivedkeys* derivedKeys)
{
	uint8_t seed[NFC3D_KEYGEN_SEED_SIZE];

	nfc3d_amiibo_calc_seed(dump, seed);
	nfc3d_keygen(masterKeys, seed, derivedKeys);
}

void nfc3d_amiibo_cipher(const nfc3d_keygen_derivedkeys* keys, const uint8_t* in, uint8_t* out, nrf_crypto_operation_t operation)
{
	ret_code_t ret_val = NRF_SUCCESS;
	nrf_crypto_aes_context_t aes;
	size_t data_size = AMIIBO_CIPHER_LENGTH;

	ret_val = nrf_crypto_aes_init(&aes, &g_nrf_crypto_aes_ctr_128_info, operation);
	AES_ERROR_CHECK(ret_val);
	ret_val = nrf_crypto_aes_key_set(&aes, (uint8_t*)keys->aesKey);
	AES_ERROR_CHECK(ret_val);
	ret_val = nrf_crypto_aes_iv_set(&aes, (uint8_t*)keys->aesIV);
	AES_ERROR_CHECK(ret_val);
	ret_val = nrf_crypto_aes_finalize(&aes, (uint8_t*)(in + 0x02C), AMIIBO_CIPHER_LENGTH,
									  out + 0x02C, &data_size);
	AES_ERROR_CHECK(ret_val);

	memcpy(out + 0x000, in + 0x000, 0x008);
	// Data signature NOT copied
	memcpy(out + 0x028, in + 0x028, 0x004);
	// Tag signature NOT copied
	memcpy(out + 0x1D4, in + 0x1D4, 0x034);
}

void nfc3d_amiibo_tag_to_internal(const uint8_t* tag, uint8_t* intl)
{
	memcpy(intl + 0x000, tag + 0x008, 0x008);
	memcpy(intl + 0x008, tag + 0x080, 0x020);
	memcpy(intl + 0x028, tag + 0x010, 0x024);
	memcpy(intl + 0x04C, tag + 0x0A0, 0x168);
	memcpy(intl + 0x1B4, tag + 0x034, 0x020);
	memcpy(intl + 0x1D4, tag + 0x000, 0x008);
	memcpy(intl + 0x1DC, tag + 0x054, 0x02C);
}

void nfc3d_amiibo_internal_to_tag(const uint8_t* intl, uint8_t* tag)
{
	memcpy(tag + 0x008, intl + 0x000, 0x008);
	memcpy(tag + 0x080, intl + 0x008, 0x020);
	memcpy(tag + 0x010, intl + 0x028, 0x024);
	memcpy(tag + 0x0A0, intl + 0x04C, 0x168);
	memcpy(tag + 0x034, intl + 0x1B4, 0x020);
	memcpy(tag + 0x000, intl + 0x1D4, 0x008);
	memcpy(tag + 0x054, intl + 0x1DC, 0x02C);
}

bool nfc3d_amiibo_unpack(const nfc3d_amiibo_keys* amiiboKeys, const uint8_t* tag, uint8_t* plain)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t internal[NFC3D_AMIIBO_SIZE];
	nfc3d_keygen_derivedkeys dataKeys;
	nfc3d_keygen_derivedkeys tagKeys;
	nrf_crypto_hmac_context_t ctx;
	size_t digest_length = HMAC_BUFFER_LENGTH;

	// Convert format
	nfc3d_amiibo_tag_to_internal(tag, internal);

	// Generate keys
	nfc3d_amiibo_keygen(&amiiboKeys->data, internal, &dataKeys);
	nfc3d_amiibo_keygen(&amiiboKeys->tag, internal, &tagKeys);

	// Decrypt
	nfc3d_amiibo_cipher(&dataKeys, internal, plain, NRF_CRYPTO_DECRYPT);

	// Regenerate tag HMAC. Note: order matters, data HMAC depends on tag HMAC!
	digest_length = HMAC_BUFFER_LENGTH;
	ret_val = nrf_crypto_hmac_calculate(&ctx,
										&g_nrf_crypto_hmac_sha256_info,
										plain + HMAC_POS_TAG,
										&digest_length,
										tagKeys.hmacKey,
										sizeof(tagKeys.hmacKey),
										plain + 0x1D4, 0x34);
	APP_ERROR_CHECK(ret_val);

	// Regenerate data HMAC
	digest_length = HMAC_BUFFER_LENGTH;
	ret_val = nrf_crypto_hmac_calculate(&ctx,
										&g_nrf_crypto_hmac_sha256_info,
										plain + HMAC_POS_DATA,
										&digest_length,
										dataKeys.hmacKey,
										sizeof(dataKeys.hmacKey),
										plain + 0x029, 0x1DF);
	APP_ERROR_CHECK(ret_val);

	return memcmp(plain + HMAC_POS_DATA, internal + HMAC_POS_DATA, 32) == 0 &&
		   memcmp(plain + HMAC_POS_TAG, internal + HMAC_POS_TAG, 32) == 0;
}

void nfc3d_amiibo_pack(const nfc3d_amiibo_keys* amiiboKeys, const uint8_t* plain, uint8_t* tag)
{
	ret_code_t ret_val = NRF_SUCCESS;
	uint8_t cipher[NFC3D_AMIIBO_SIZE];
	nfc3d_keygen_derivedkeys tagKeys;
	nfc3d_keygen_derivedkeys dataKeys;
	nrf_crypto_hmac_context_t ctx;
	size_t digest_length = HMAC_BUFFER_LENGTH;

	// Generate keys
	nfc3d_amiibo_keygen(&amiiboKeys->tag, plain, &tagKeys);
	nfc3d_amiibo_keygen(&amiiboKeys->data, plain, &dataKeys);

	// Generate tag HMAC
	digest_length = HMAC_BUFFER_LENGTH;
	ret_val = nrf_crypto_hmac_calculate(&ctx,
										&g_nrf_crypto_hmac_sha256_info,
										cipher + HMAC_POS_TAG,
										&digest_length,
										tagKeys.hmacKey,
										sizeof(tagKeys.hmacKey),
										plain + 0x1D4, 0x34);
	APP_ERROR_CHECK(ret_val);

	// Init a new nrf5 HMAC context
	ret_val = nrf_crypto_hmac_init(&ctx, &g_nrf_crypto_hmac_sha256_info, dataKeys.hmacKey, sizeof(dataKeys.hmacKey));
	APP_ERROR_CHECK(ret_val);

	// Generate data HMAC
	ret_val = nrf_crypto_hmac_update(&ctx, plain + 0x029, 0x18B); // Data
	APP_ERROR_CHECK(ret_val);
	ret_val = nrf_crypto_hmac_update(&ctx, cipher + HMAC_POS_TAG, 0x20); // Tag HMAC
	APP_ERROR_CHECK(ret_val);
	ret_val = nrf_crypto_hmac_update(&ctx, plain + 0x1D4, 0x34); // Here be dragons
	APP_ERROR_CHECK(ret_val);

	digest_length = HMAC_BUFFER_LENGTH;
	ret_val = nrf_crypto_hmac_finalize(&ctx, cipher + HMAC_POS_DATA, &digest_length);
	APP_ERROR_CHECK(ret_val);

	// Encrypt
	nfc3d_amiibo_cipher(&dataKeys, plain, cipher, NRF_CRYPTO_ENCRYPT);

	// Convert back to hardware
	nfc3d_amiibo_internal_to_tag(cipher, tag);
}

bool nfc3d_amiibo_load_keys(nfc3d_amiibo_keys* amiiboKeys, const uint8_t* data)
{
	if (!data)
	{
		return false;
	}

	memcpy(amiiboKeys, data, sizeof(*amiiboKeys));

	if (
		(amiiboKeys->data.magicBytesSize > 16) ||
		(amiiboKeys->tag.magicBytesSize > 16))
	{
		errno = EILSEQ;
		return false;
	}

	return true;
}

void nfc3d_amiibo_copy_app_data(const uint8_t* src, uint8_t* dst)
{

	uint16_t* ami_nb_wr = (uint16_t*)(dst + 0x29);
	uint16_t* cfg_nb_wr = (uint16_t*)(dst + 0xB4);

	/* increment write counters */
	*ami_nb_wr = htobe16(be16toh(*ami_nb_wr) + 1);
	*cfg_nb_wr = htobe16(be16toh(*cfg_nb_wr) + 1);

	/* copy flags */
	dst[0x2C] = src[0x2C];
	/* copy programID */
	memcpy(dst + 0xAC, src + 0xAC, 8);
	/* copy AppID */
	memcpy(dst + 0xB6, src + 0xB6, 4);
	/* copy AppData */
	memcpy(dst + 0xDC, src + 0xDC, 216);
}
