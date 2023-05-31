#include "amiibo_helper.h"
#include "amiibo_data.h"
#include "nfc3d/amiibo.h"
#include "nrf_log.h"
#include "ntag_store.h"
#include "vfs.h"

#include <string.h>

#define UUID_OFFSET 468
#define PASSWORD_OFFSET 532
#define PASSWORD_SIZE 4
#define UUID_SIZE 7

static nfc3d_amiibo_keys amiibo_keys;
static bool amiibo_keys_loaded;

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

void amiibo_helper_set_defaults(uint8_t *buffer, const uint8_t uuid[]) {

    // Same as bcc[1]
    buffer[0] = uuid[3] ^ uuid[4] ^ uuid[5] ^ uuid[6];

    // All of these are magic values
    buffer[536] = 0x80;
    buffer[537] = 0x80;

    buffer[520] = 0;
    buffer[521] = 0;
    buffer[522] = 0;

    buffer[2] = 0;
    buffer[3] = 0;
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
    // amiibo_helper_replace_password(modified, new_uuid);
    amiibo_helper_set_defaults(modified, new_uuid);

    nfc3d_amiibo_pack(&amiibo_keys, modified, new_ntag->data);

    // static lock
    new_ntag->data[10] = 0x0f;
    new_ntag->data[11] = 0xe0;

    return NRF_SUCCESS;
}

ret_code_t amiibo_helper_ntag_generate(ntag_t *ntag) {

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
    if(err_code != NRF_SUCCESS){
        return err_code;
    }

    // sign new
    err_code = amiibo_helper_sign_new_ntag(ntag_current, &ntag_new);
    if (err_code == NRF_SUCCESS) {
        memcpy(ntag, &ntag_new, sizeof(ntag_t));
    }
    return err_code;
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

bool is_valid_amiibo_ntag(const ntag_t *ntag) {
    uint32_t head = to_little_endian_int32(&ntag->data[84]);
    uint32_t tail = to_little_endian_int32(&ntag->data[88]);

    const amiibo_data_t *amd = find_amiibo_data(head, tail);
    if (amd != NULL) {
        return true;
    }
    if (head != 0 && tail != 0) {
        return true;
    }

    return false;
}
