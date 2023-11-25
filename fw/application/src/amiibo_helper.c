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
    uid1[0] = ntag->data[0];
    uid1[1] = ntag->data[1];
    uid1[2] = ntag->data[2];
    uid1[3] = ntag->data[4];
    uid1[4] = ntag->data[5];
    uid1[5] = ntag->data[6];
    uid1[6] = ntag->data[7];
}

void amiibo_helper_replace_uuid(uint8_t *buffer, const uint8_t uuid[]) {
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
}

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
void amiibo_helper_set_defaults(uint8_t *buffer, const uint8_t uuid[]) {
    // set keygen salt
    ret_code_t err_code = utils_rand_bytes(buffer + 0x1E8, 32);
    APP_ERROR_CHECK(err_code);

    memcpy(buffer, Internal_StaticLock, 8);
    // set BCC
    buffer[0] = uuid[3] ^ uuid[4] ^ uuid[5] ^ uuid[6];

    amiibo_helper_replace_uuid(buffer, uuid);
    amiibo_helper_replace_password(buffer, uuid);

    memcpy(buffer + 0x28, A5Static, 4);
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
    // decrypt old amiibo
    uint8_t modified[NTAG215_SIZE];
    uint8_t new_uuid[UUID_SIZE];
    if (!nfc3d_amiibo_unpack(&amiibo_keys, old_ntag->data, modified)) {
        return NRF_ERROR_INVALID_DATA;
    }

    // encrypt
    amiibo_helper_get_uuid(new_ntag, new_uuid);
    amiibo_helper_replace_uuid(modified, new_uuid);
    amiibo_helper_set_defaults(modified, new_uuid);
    nfc3d_amiibo_pack(&amiibo_keys, modified, new_ntag->data);

    return NRF_SUCCESS;
}

ret_code_t amiibo_helper_rand_amiibo_uuid(ntag_t *ntag) {
    ret_code_t err_code;
    ntag_t ntag_new;
    ntag_t *ntag_current = ntag;

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

ret_code_t amiibo_helper_generate_amiibo(uint32_t head, uint32_t tail, ntag_t *ntag) {
    if (!amiibo_helper_is_key_loaded()) {
        return NRF_ERROR_INVALID_DATA;
    }
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
    amiibo_helper_set_defaults(modified, uuid);

    // encrypt
    nfc3d_amiibo_pack(&amiibo_keys, modified, ntag->data);

    // 最后20位字节，非常关键
    memcpy(ntag->data + 520, LOCK, sizeof(LOCK));

    return NRF_SUCCESS;
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

uint32_t to_little_endian_int32(const uint8_t* data){
    uint32_t val = 0;
    val += data[0];
    val <<= 8;
    val += data[1];
    val <<= 8;
    val += data[2];
    val <<=8;
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

