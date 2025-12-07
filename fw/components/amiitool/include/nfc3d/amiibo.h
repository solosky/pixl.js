/*
 * (c) 2020-2021 nitz — chris marc dailey https://cmd.wtf
 * (c) 2015-2017 Marcos Del Sol Vives
 * (c) 2016      javiMaD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef HAVE_NFC3D_AMIIBO_H
#define HAVE_NFC3D_AMIIBO_H

#include "keygen.h"
#include <stdbool.h>
#include <stdint.h>

#define NFC3D_AMIIBO_SIZE 520
#define NTAG215_SIZE 540

#pragma pack(1)
typedef struct
{
	nfc3d_keygen_masterkeys data;
	nfc3d_keygen_masterkeys tag;
} nfc3d_amiibo_keys;
#pragma pack()

bool nfc3d_amiibo_unpack(const nfc3d_amiibo_keys* amiiboKeys, const uint8_t* tag, uint8_t* plain, bool tag_v3);
void nfc3d_amiibo_pack(const nfc3d_amiibo_keys* amiiboKeys, const uint8_t* plain, uint8_t* tag, bool tag_v3);
bool nfc3d_amiibo_load_keys(nfc3d_amiibo_keys* amiiboKeys, const uint8_t* data);
void nfc3d_amiibo_copy_app_data(const uint8_t* src, uint8_t* dst);

#endif
