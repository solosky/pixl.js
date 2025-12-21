#include "amiibo_helper.h"
#include "db_header.h"
#include "nfc3d/amiibo.h"
#include "nrf_log.h"
#include "ntag_store.h"
#include "vfs.h"

#include "utils2.h"

#include <string.h>

#define UUID_OFFSET 468
#define AMII_ID_OFFSET 476
#define AMII_ID_OFFSET2 0x54
#define PASSWORD_OFFSET 532
#define PASSWORD_SIZE 4
#define UUID_SIZE 7
#define AMIIID_SIZE 8

// to avoid stack-allocating the new larger ntag_t struct
// static ntag_t ntag_new;

static nfc3d_amiibo_keys amiibo_keys;
static bool amiibo_keys_loaded;

// plain amiibo data static code
static const uint8_t Internal_StaticLock[8] = {0x65, 0x48, 0x0F, 0xE0, 0xF1, 0x10, 0xFF, 0xEE}; // 0x0
static const uint8_t A5Static[4] = {0xA5, 0x00, 0x00, 0x00};                                    // 0x28
static const uint8_t DynLock[4] = {0x01, 0x00, 0x0F, 0xBD};                                     // 0x208
static const uint8_t Cfg0[4] = {0x00, 0x00, 0x00, 0x04};                                        // 0x20C
static const uint8_t Cfg1[4] = {0x5F, 0x00, 0x00, 0x00};                                        // 0x210

static const uint8_t LOCK[20] = {0x01, 0x00, 0x0f, 0xbd, 0x00, 0x00, 0x00, 0x04, 0x5f, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void amiibo_helper_get_uuid(ntag_t *ntag, uint8_t *uid1) {
    ntag_store_get_uuid(ntag, uid1);
}

// operates on internal data layout, but check bytes differ
void amiibo_helper_replace_uuid(uint8_t *buffer, const uint8_t uuid[], bool tag_v3) {
    if (tag_v3) {
        // writes to ntag215-data-1 section
        for (int i = 0; i < 7; i++) {
            buffer[UUID_OFFSET + i] = uuid[i];
        }
        buffer[UUID_OFFSET+7] = 0x00; // rfu
    } else {
        uint8_t bcc[2];
        bcc[0] = 0x88 ^ uuid[0] ^ uuid[1] ^ uuid[2];
        bcc[1] = uuid[3] ^ uuid[4] ^ uuid[5] ^ uuid[6];

        int i;
        for (i = 0; i < 3; i++) {
            buffer[UUID_OFFSET + i] = uuid[i];
        }

        buffer[UUID_OFFSET + i++] = bcc[0];

        for (; i < 8; i++) {
            buffer[UUID_OFFSET + i] = uuid[i - 1];
        }

        // first byte of ntag215-data-2 section = byte 9 of uid (bcc1)
        buffer[0] = bcc[1];
    }
}

// operates on internal data layout
void amiibo_helper_replace_password(uint8_t *buffer, const uint8_t uuid[]) {
    uint8_t password[PASSWORD_SIZE] = {0, 0, 0, 0};

    printf("Updating password\n");
    password[0] = 0xAA ^ uuid[1] ^ uuid[3];
    password[1] = 0x55 ^ uuid[2] ^ uuid[4];
    password[2] = 0xAA ^ uuid[3] ^ uuid[5];
    password[3] = 0x55 ^ uuid[4] ^ uuid[6];

    for (int i = 0; i < PASSWORD_SIZE; i++) {
        buffer[PASSWORD_OFFSET + i] = password[i];
    }
}

// 使用uuid生成amiibo数据
// operates on internal data layout
void amiibo_helper_set_defaults(uint8_t *buffer, const uint8_t uuid[], bool tag_v3) {
    // set keygen salt
    ret_code_t err_code = utils_rand_bytes(buffer + 0x1E8, 32);
    APP_ERROR_CHECK(err_code);

    // ntag data 2
    memcpy(buffer, Internal_StaticLock, 8);
    // set BCC
    buffer[0] = uuid[3] ^ uuid[4] ^ uuid[5] ^ uuid[6];

    amiibo_helper_replace_uuid(buffer, uuid, tag_v3);
    amiibo_helper_replace_password(buffer, uuid);

    memcpy(buffer + 0x28, A5Static, 4);

    // note: these sections are never written back when re-packing
    memcpy(buffer + 0x208, DynLock, 4);
    memcpy(buffer + 0x20C, Cfg0, 4);
    memcpy(buffer + 0x210, Cfg1, 4);
    // auth magic value
    buffer[0x218] = 0;
    buffer[0x219] = 0;
}

ret_code_t amiibo_helper_load_keys(const uint8_t *data) {
    if (!nfc3d_amiibo_load_keys(&amiibo_keys, data)) {
        return NRF_ERROR_INVALID_DATA;
    }
    amiibo_keys_loaded = true;
    return NRF_SUCCESS;
}

bool amiibo_helper_is_key_loaded() { return amiibo_keys_loaded; }

ret_code_t amiibo_helper_sign_new_ntag(ntag_t *old_ntag, ntag_t *new_ntag) {
    if (new_ntag->type != old_ntag->type) {
        return NRF_ERROR_INVALID_DATA;
    }

    // decrypt old amiibo
    uint8_t modified[NTAG215_SIZE];
    uint8_t new_uuid[UUID_SIZE];
    bool tag_v3 = old_ntag->type == NTAG_I2C_PLUS_2K;
    if (!nfc3d_amiibo_unpack(&amiibo_keys, old_ntag->data, modified, tag_v3)) {
        return NRF_ERROR_INVALID_DATA;
    }

    // encrypt
    amiibo_helper_get_uuid(new_ntag, new_uuid);
    amiibo_helper_replace_uuid(modified, new_uuid, tag_v3);
    amiibo_helper_set_defaults(modified, new_uuid, tag_v3);
    nfc3d_amiibo_pack(&amiibo_keys, modified, new_ntag->data, tag_v3);

    return NRF_SUCCESS;
}

ret_code_t amiibo_helper_rand_amiibo_uuid(ntag_t *ntag) {
    ret_code_t err_code;
    ntag_t *ntag_current = ntag;

    ntag_t ntag_new;

    memcpy(&ntag_new, ntag_current, sizeof(ntag_t));

    if (!is_valid_amiibo_ntag(ntag_current)) {
        return NRF_ERROR_INVALID_DATA;
    }

    if (!amiibo_helper_is_key_loaded()) {
        return NRF_ERROR_INVALID_DATA;
    }

    err_code = ntag_store_uuid_rand(&ntag_new);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    
    // sign new
    err_code = amiibo_helper_sign_new_ntag(ntag_current, &ntag_new);
    if (err_code == NRF_SUCCESS) {
        memcpy(ntag, &ntag_new, sizeof(ntag_t));
    }
    return err_code;
}

ret_code_t amiibo_helper_set_amiibo_uuid(ntag_t *ntag, uint8_t *uuid) {
    ret_code_t err_code;
    ntag_t *ntag_current = ntag;
    ntag_t ntag_new;

    memcpy(&ntag_new, ntag_current, sizeof(ntag_t));

    if (!is_valid_amiibo_ntag(ntag_current)) {
        return NRF_ERROR_INVALID_DATA;
    }

    if (!amiibo_helper_is_key_loaded()) {
        return NRF_ERROR_INVALID_DATA;
    }

    ntag_store_set_uuid(&ntag_new, uuid);

    // sign new
    err_code = amiibo_helper_sign_new_ntag(ntag_current, &ntag_new);
    if (err_code == NRF_SUCCESS) {
        memcpy(ntag, &ntag_new, sizeof(ntag_t));
    }
    return err_code;
}

ret_code_t amiibo_helper_generate_amiibo_ntag215(uint32_t head, uint32_t tail, ntag_t *ntag) {
    if (!amiibo_helper_is_key_loaded()) {
        return NRF_ERROR_INVALID_DATA;
    }

    ntag->type = NTAG_215;
    
    // 随机uuid
    uint8_t uuid[UUID_SIZE];
    ntag_store_new_rand(ntag);
    amiibo_helper_get_uuid(ntag, uuid);

    uint8_t modified[NTAG215_SIZE];
    memset(modified, 0, NTAG215_SIZE);
    // 填充amiibo id
    int32_to_bytes_le(head, modified + AMII_ID_OFFSET);
    int32_to_bytes_le(tail, modified + AMII_ID_OFFSET + 4);
    int32_to_bytes_le(head, modified + AMII_ID_OFFSET2);
    int32_to_bytes_le(tail, modified + AMII_ID_OFFSET2 + 4);
    // 填充特定数据
    amiibo_helper_set_defaults(modified, uuid, false);

    // encrypt
    nfc3d_amiibo_pack(&amiibo_keys, modified, ntag->data, false);

    // 最后20位字节，非常关键
    memcpy(ntag->data + 520, LOCK, sizeof(LOCK));

    return NRF_SUCCESS;
}

ret_code_t amiibo_helper_generate_amiibo_v3(uint32_t head, uint32_t tail, ntag_t *ntag) {
    if (!amiibo_helper_is_key_loaded()) {
        return NRF_ERROR_INVALID_DATA;
    }

    // Step 1: Generate regular amiibo data first
    ret_code_t err_code = amiibo_helper_generate_amiibo_ntag215(head, tail, ntag);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    // Step 2: Convert to v3 format (NTAG_I2C_PLUS_2K)
    ntag->type = NTAG_I2C_PLUS_2K;
    
    // Calculate the size of data that needs to be moved back (from 0x80 onwards)
    size_t data_to_move_size = NTAG_DATA_SIZE - 0x80;
    
    // Move data from 0x80 onwards 0x40 bytes back to make room for new_nonce
    // Using memmove for safe overlapping memory operations
    memmove(ntag->data + 0x80 + 0x40, ntag->data + 0x80, data_to_move_size);
    
    // Clear the new_nonce area (0x40 bytes at 0x80)
    memset(ntag->data + 0x80, 0, 0x40);
    
    // Calculate total size after adding new_nonce
    size_t total_size_after_nonce = NTAG_DATA_SIZE + 0x40;
    
    // Padding up to page 0xE2 (each page is 4 bytes)
    // Page 0xE2 starts at 0xE2 * 4 = 0x388
    if (total_size_after_nonce < 0x388) {
        memset(ntag->data + total_size_after_nonce, 0, 0x388 - total_size_after_nonce);
    }
    
    // Page E2: dynamic lock bytes
    memcpy(ntag->data + 0x388, (uint8_t[]){0x01, 0x00, 0xFF, 0x00}, 4);
    
    // Page E3: auth0
    memcpy(ntag->data + 0x38C, (uint8_t[]){0x00, 0x00, 0x00, 0x04}, 4);
    
    // Page E4: access
    memcpy(ntag->data + 0x390, (uint8_t[]){0x07, 0x00, 0x00, 0x00}, 4);
    
    // Pages E5-EB: zeros
    memset(ntag->data + 0x394, 0, (0xEC - 0xE5) * 4);
    
    // Page EC: default config + sram enabled
    memcpy(ntag->data + 0x3B0, (uint8_t[]){0x41, 0x00, 0xF8, 0x48}, 4);
    
    // Page ED: default config + sram_rf_ready
    memcpy(ntag->data + 0x3B4, (uint8_t[]){0x08, 0x01, 0x29, 0x00}, 4);
    
    // Add 8 bytes of zeros
    memset(ntag->data + 0x3B8, 0, 8);
    
    // Generate sram_data directly in ntag->data memory
    size_t sram_data_offset = 0x3C0;
    size_t sram_data_size = 0x40;
    
    // Clear sram_data area
    memset(ntag->data + sram_data_offset, 0, sram_data_size);
    
    // Calculate CRC16-MCRF4XX for sram_data (first 0x3e bytes)
    uint16_t crc = crc16_mcrf4xx(ntag->data + sram_data_offset, 0x3e);
    
    // Add CRC to sram_data (big endian)
    ntag->data[sram_data_offset + 0x3e] = (crc >> 8) & 0xFF;
    ntag->data[sram_data_offset + 0x3f] = crc & 0xFF;
    
    // Add padding to reach 256*4 bytes (1024 bytes)
    size_t padding_start = sram_data_offset + sram_data_size;
    if (padding_start < 1024) {
        memset(ntag->data + padding_start, 0, 1024 - padding_start);
    }
    
    return NRF_SUCCESS;
}

ret_code_t amiibo_helper_generate_amiibo(uint32_t head, uint32_t tail, ntag_t *ntag) {
    if (is_valid_amiibo_v3(head, tail)) {
        return amiibo_helper_generate_amiibo_v3(head, tail, ntag);
    }else{
        return amiibo_helper_generate_amiibo_ntag215(head, tail, ntag);
    }
}

void amiibo_helper_try_load_amiibo_keys_from_vfs() {
    if (!amiibo_helper_is_key_loaded() && vfs_drive_enabled(VFS_DRIVE_EXT)) {
        uint8_t key_data[160];
        vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
        int32_t err = p_driver->read_file_data("/key_retail.bin", key_data, sizeof(key_data));
        NRF_LOG_INFO("amiibo key read: %d", err);
        if (err == sizeof(key_data)) {
            ret_code_t ret = amiibo_helper_load_keys(key_data);
            if (ret == NRF_SUCCESS) {
                NRF_LOG_INFO("amiibo key loaded!");
            }
        }
    }
}

uint32_t to_little_endian_int32(const uint8_t *data) {
    uint32_t val = 0;
    val += data[0];
    val <<= 8;
    val += data[1];
    val <<= 8;
    val += data[2];
    val <<= 8;
    val += data[3];
    return val;
}

bool is_valid_amiibo_ntag(const ntag_t *ntag) {
    uint32_t head = to_little_endian_int32(&ntag->data[84]);
    uint32_t tail = to_little_endian_int32(&ntag->data[88]);

    if (ntag->data[0] != 0x4) {
        return false;
    }

    const db_amiibo_t *amd = get_amiibo_by_id(head, tail);
    if (amd != NULL) {
        return true;
    }
    if (head != 0 && tail != 0) {
        return true;
    }

    return false;
}

uint16_t crc16_mcrf4xx(const uint8_t* data, size_t length) {
    uint16_t crc = 0xffff;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0x8408;
            } else {
                crc = (crc >> 1);
            }
        }
    }
    return crc & 0xffff;
}