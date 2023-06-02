/*
 * ntag_store.c
 *
 *  Created on: 2021年9月1日
 *      Author: solos
 */

#include "ntag_store.h"
//#include "fds.h"
#include "nrf_log.h"
#include "utils.h"

#include "vfs.h"

const ntag_t default_ntag215 = {
    .data = {0x04, 0x68, 0x95, 0x71, 0xfa, 0x5c, 0x64, 0x80, 0x42, 0x48, 0x00, 0x00, 0xe1,
             0x10, 0x3e, 0x00, 0x03, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0xf1, 0x10, 0xff, 0xee, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0x10, 0xff, 0xee, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0xbd, 0x04, 0x00, 0x00, 0xff, 0x00, 0x05, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    };

ret_code_t ntag_store_generate(uint8_t idx, ntag_t *ntag) {
    memcpy(ntag, &default_ntag215, sizeof(ntag_t));
    ntag->data[7] = idx;
    //ntag->index = idx;
    // BCC 0 is always equal to UID0 ⊕ UID 1 ⊕ UID 2 ⊕ 0x88
    // ntag->data[3] = ntag->data[0] ^ ntag->data[1] ^ ntag->data[2] ^ 0x88;
    // BCC 1 is always equal to UID3 ⊕ UID 4 ⊕ UID 5 ⊕ UID6
    ntag->data[8] = ntag->data[4] ^ ntag->data[5] ^ ntag->data[6] ^ ntag->data[7];
    return NRF_SUCCESS;
}



ret_code_t ntag_store_uuid_rand(ntag_t *ntag) {

    int8_t uuid[6];

    ret_code_t err_code = utils_rand_bytes(uuid, sizeof(uuid));
    VERIFY_SUCCESS(err_code);

    ntag->data[0] = 04; // fixed
    ntag->data[1] = uuid[0];
    ntag->data[2] = uuid[1];
    // BCC 0 is always equal to UID0 ⊕ UID 1 ⊕ UID 2 ⊕ 0x88
    ntag->data[3] = ntag->data[0] ^ ntag->data[1] ^ ntag->data[2] ^ 0x88;
    ntag->data[4] = uuid[2];
    ntag->data[5] = uuid[3];
    ntag->data[6] = uuid[4];
    ntag->data[7] = uuid[5];

    // BCC 1 is always equal to UID3 ⊕ UID 4 ⊕ UID 5 ⊕ UID6
    ntag->data[8] = ntag->data[4] ^ ntag->data[5] ^ ntag->data[6] ^ ntag->data[7];
    return NRF_SUCCESS;
}

void ntag_store_new_rand(ntag_t* ntag){
    memcpy(ntag, &default_ntag215, sizeof(ntag_t));
    ntag_store_uuid_rand(ntag);
}

#ifdef STORAGE_FDS

static volatile bool m_fds_ready =
    false; /**< Flag used to indicate that FDS initialization is finished. */
static volatile ret_code_t m_fs_op_retcode = NRF_SUCCESS;

#define FILE_ID 0xAABB
#define RECORD_ID_BASE 0x1100
#define RECORD_ID(i) RECORD_ID_BASE + i

static void fds_evt_handler(fds_evt_t const *const p_fds_evt);
static ret_code_t fds_wait_ready();

ret_code_t ntag_store_init() {
    ret_code_t err_code;

    // Register FDS event handler to the FDS module.
    err_code = fds_register(fds_evt_handler);
    VERIFY_SUCCESS(err_code);

    // Initialize FDS.
    err_code = fds_init();
    VERIFY_SUCCESS(err_code);

    // Wait until FDS is initialized.
    while (!m_fds_ready)
        ;

    return err_code;
}

ret_code_t ntag_store_gc() {
    ret_code_t err_code;

    NRF_LOG_INFO("ntag store gc begin");
    m_fds_ready = false;
    err_code = fds_gc();
    VERIFY_SUCCESS(err_code);
    // Wait until FDS is initialized.
    while (!m_fds_ready)
        ;
    NRF_LOG_INFO("ntag store gc end");
    return NRF_SUCCESS;
}

ret_code_t ntag_store_read(uint8_t idx, ntag_t *tag) {

    ret_code_t err_code;
    fds_find_token_t ftok;
    fds_flash_record_t flash_record;
    fds_record_desc_t m_record_desc;

    // Always clear token before running new file/record search.
    memset(&ftok, 0x00, sizeof(fds_find_token_t));

    // Search for NDEF message in FLASH.
    err_code = fds_record_find(FILE_ID, RECORD_ID(idx), &m_record_desc, &ftok);

    // If there is no record with given key and file ID,
    // create default message and store in FLASH.
    if (err_code == NRF_SUCCESS) {
        NRF_LOG_INFO("Found NDEF file record.");

        // Open record for read.
        err_code = fds_record_open(&m_record_desc, &flash_record);
        VERIFY_SUCCESS(err_code);

        // Access the record through the flash_record structure.
        memcpy(tag->data, flash_record.p_data, NTAG_DATA_SIZE);

        // Print file length and raw message data.
        NRF_LOG_DEBUG("NDEF file data length: %u bytes.",
                      flash_record.p_header->length_words * sizeof(uint32_t));

        // Close the record when done.
        err_code = fds_record_close(&m_record_desc);
    }

    return err_code;
}
ret_code_t ntag_store_write(uint8_t idx, ntag_t *ntag) {
    ret_code_t err_code;
    fds_find_token_t ftok;
    fds_flash_record_t flash_record;
    fds_record_desc_t m_record_desc;
    fds_record_t m_record;

    // Always clear token before running new file/record search.
    memset(&ftok, 0x00, sizeof(fds_find_token_t));

    // Search for NDEF message in FLASH.
    err_code = fds_record_find(FILE_ID, RECORD_ID(idx), &m_record_desc, &ftok);

    // If there is no record with given key and file ID,
    // create default message and store in FLASH.
    if (err_code == NRF_SUCCESS) {
        // found, update
        m_record.file_id = FILE_ID;
        m_record.key = RECORD_ID(idx);
        m_record.data.p_data = ntag->data;
        m_record.data.length_words = BYTES_TO_WORDS(NTAG_DATA_SIZE); // Align data length to 4 bytes.
        NRF_LOG_INFO("update ntag: %d", idx);
        m_fds_ready = false;
        err_code = fds_record_update(&m_record_desc, &m_record);
        if (err_code == FDS_ERR_NO_SPACE_IN_FLASH) {
            NRF_LOG_INFO("no space in flash ntag");
            err_code = ntag_store_gc();
            VERIFY_SUCCESS(err_code);
            err_code = fds_record_update(&m_record_desc, &m_record);
        }
        VERIFY_SUCCESS(err_code);
        err_code = fds_wait_ready();
        NRF_LOG_INFO("update ntag %d result: %d", idx, m_fs_op_retcode);
        return err_code;
    } else if (err_code == FDS_ERR_NOT_FOUND) {
        // not found, create and write
        m_record.file_id = FILE_ID;
        m_record.key = RECORD_ID(idx);
        m_record.data.p_data = ntag->data;
        m_record.data.length_words = BYTES_TO_WORDS(NTAG_DATA_SIZE); // Align data length to 4 bytes.
        NRF_LOG_INFO("write ntag: %d", idx);
        m_fds_ready = false;
        err_code = fds_record_write(&m_record_desc, &m_record);
        VERIFY_SUCCESS(err_code);
        err_code = fds_wait_ready();
        NRF_LOG_INFO("write ntag %d result: %d", idx, m_fs_op_retcode);
        return err_code;
    } else {
        return err_code;
    }
}

ret_code_t ntag_store_write_with_gc(uint8_t idx, ntag_t *ntag) {
    ret_code_t err_code;
    err_code = ntag_store_write(idx, ntag);
    if (err_code == FDS_ERR_NO_SPACE_IN_FLASH) {
        err_code = ntag_store_gc();
        if (err_code != NRF_SUCCESS) {
            return err_code;
        }
        return ntag_store_write(idx, ntag);
    } else {
        return err_code;
    }
}

ret_code_t ntag_store_reset(uint8_t idx, ntag_t *ntag) {
    ntag_store_generate(idx, ntag);
    return ntag_store_write(idx, ntag);
}

ret_code_t ntag_store_read_default(uint8_t idx, ntag_t *ntag) {
    NRF_LOG_DEBUG("read default:%d", idx);
    ret_code_t err_code;
    err_code = ntag_store_read(idx, ntag);
    if (err_code == NRF_SUCCESS) {
        return err_code;
    } else if (err_code == FDS_ERR_NOT_FOUND) {
        NRF_LOG_INFO("ntag is not found, create one");
        ntag_store_generate(idx, ntag);
        err_code = ntag_store_write(idx, ntag);
        if (err_code == FDS_ERR_NO_SPACE_IN_FLASH) {
            err_code = ntag_store_gc();
            if (err_code != NRF_SUCCESS) {
                return err_code;
            }
            return ntag_store_write(idx, ntag);
        }
        return err_code;
    } else {
        return err_code;
    }
}

static void fds_evt_handler(fds_evt_t const *const p_fds_evt) {
    ret_code_t err_code;

    NRF_LOG_INFO("FDS event %u with result %u.", p_fds_evt->id, p_fds_evt->result);

    switch (p_fds_evt->id) {
    case FDS_EVT_INIT:
        m_fs_op_retcode = p_fds_evt->result;
        m_fds_ready = true;
        APP_ERROR_CHECK(p_fds_evt->result);
        break;

    case FDS_EVT_UPDATE:
        m_fs_op_retcode = p_fds_evt->result;
        m_fds_ready = true;
        APP_ERROR_CHECK(p_fds_evt->result);
        NRF_LOG_INFO("FDS update success.");
        break;

    case FDS_EVT_WRITE:
        m_fs_op_retcode = p_fds_evt->result;
        m_fds_ready = true;
        APP_ERROR_CHECK(p_fds_evt->result);
        NRF_LOG_INFO("FDS write success.");

        break;

    case FDS_EVT_GC:
        m_fs_op_retcode = p_fds_evt->result;
        m_fds_ready = true;
        APP_ERROR_CHECK(p_fds_evt->result);
        NRF_LOG_INFO("Garbage Collector activity finished.");

        break;

    default:
        break;
    }
}

static ret_code_t fds_wait_ready() {
    // Wait until FDS is initialized.
    while (!m_fds_ready)
        ;
    return m_fs_op_retcode;
}

#endif

#ifdef STORAGE_LFS

#include "lfs.h"
#include "lfs_port.h"

#define FILE_MAX_PATH 16

#define NTAG_STORAGE_NOT_FOUND 10000
#define NTAG_STORAGE_READ_ERROR 10001
#define NTAG_STORAGE_WRITE_ERROR 10002
#define NTAG_STORAGE_READ_CORRUPT 10003

ret_code_t ntag_store_init() { return NRF_SUCCESS; }

void ntag_store_format_path(uint8_t idx, char *path) {
    sprintf(path, "amiibo/%02d.bin", idx);
}

ret_code_t ntag_store_read(uint8_t idx, ntag_t *ntag) {
    char path[FILE_MAX_PATH];
    lfs_file_t file;

    ntag_store_format_path(idx, path);
    int err = lfs_file_open(&lfs, &file, path, LFS_O_RDONLY);
    NRF_LOG_INFO("lfs_file_open: %d", err);
    if (err == LFS_ERR_NOENT) {
        return NTAG_STORAGE_NOT_FOUND;
    } else if (err) {
        return NTAG_STORAGE_READ_ERROR;
    }

    err = lfs_file_read(&lfs, &file, ntag->data, 540);
    NRF_LOG_INFO("lfs_file_read: %d", err);

    if (err < 540) {
        lfs_file_close(&lfs, &file);
        return NTAG_STORAGE_READ_CORRUPT;
    }

    lfs_file_close(&lfs, &file);
    ntag->size = 540;
    ntag->type = NTAG_215;
    ntag->index = 0;
}

ret_code_t ntag_store_write(uint8_t idx, ntag_t *ntag) {
    char path[FILE_MAX_PATH];
    lfs_file_t file;

    ntag_store_format_path(idx, path);
    int err = lfs_file_open(&lfs, &file, path, LFS_O_WRONLY | LFS_O_CREAT);
    if (err) {
        return NTAG_STORAGE_WRITE_ERROR;
    }

    if (lfs_file_write(&lfs, &file, ntag->data, 540) < 540) {
        lfs_file_close(&lfs, &file);
        return NTAG_STORAGE_WRITE_ERROR;
    }

    lfs_file_close(&lfs, &file);

    return NRF_SUCCESS;
}

ret_code_t ntag_store_read_default(uint8_t idx, ntag_t *ntag) {
    char path[16];
    ret_code_t err = ntag_store_read(idx, ntag);
    NRF_LOG_INFO("read ntag: %d", idx);
    if (err == NTAG_STORAGE_NOT_FOUND || err == NTAG_STORAGE_READ_CORRUPT) {
        NRF_LOG_INFO("generate ntag: %d", idx);
        ntag_store_generate(idx, ntag);
        return ntag_store_write(idx, ntag);
    }

    return err;
}
ret_code_t ntag_store_write_with_gc(uint8_t idx, ntag_t *ntag) {
    return ntag_store_write(idx, ntag);
}

ret_code_t ntag_store_reset(uint8_t idx, ntag_t *ntag) {
    ntag_store_generate(idx, ntag);
    return ntag_store_write(idx, ntag);
}

#endif
