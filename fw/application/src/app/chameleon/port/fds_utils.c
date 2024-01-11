//
// Created by solos on 8/27/2023.
//
#include "nrf_log.h"
#include "vfs.h"
#include "vfs_meta.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fds_ids.h"
#include "tag_helper.h"

static void fds_map_file_name(uint16_t id, uint16_t key, char *path) {
    if (id == FDS_EMULATION_CONFIG_FILE_ID) {
        strcpy(path, "/chameleon/slots/config.bin");
    } else if (id >= FDS_SLOT_TAG_DUMP_FILE_ID_BASE && id < FDS_SLOT_TAG_DUMP_FILE_ID_BASE + SLOT_MAX) {
        sprintf(path, "/chameleon/slots/%02d.bin", id - FDS_SLOT_TAG_DUMP_FILE_ID_BASE);
    } else {
        sprintf(path, "/chameleon/slots/%d_%d.bin", id, key);
    }
}

bool fds_read_sync(uint16_t id, uint16_t key, uint16_t* max_length, uint8_t *buffer) {
    char path[VFS_MAX_PATH_LEN];
    fds_map_file_name(id, key, path);
    int32_t bytes_read = vfs_get_default_driver()->read_file_data(path, buffer, *max_length);
    *max_length = bytes_read;
    NRF_LOG_INFO("fds_read_sync: id=%X, key=%d, bytes_read=%d", id, key, bytes_read);
    return bytes_read > 0;
}
bool fds_write_sync(uint16_t id, uint16_t key, uint16_t data_length_words, void *buffer) {
    char path[VFS_MAX_PATH_LEN];
    fds_map_file_name(id, key, path);
    int32_t bytes_written = vfs_get_default_driver()->write_file_data(path, buffer, data_length_words * 4);
    NRF_LOG_INFO("fds_write_sync: id=%X, key=%d, bytes_written=%d", id, key, bytes_written);
    return bytes_written > 0;
}

bool fds_is_exists(uint16_t id, uint16_t key) {
    char path[VFS_MAX_PATH_LEN];
    vfs_obj_t obj;
    fds_map_file_name(id, key, path);
    int32_t ret = vfs_get_default_driver()->stat_file(path, &obj);
    NRF_LOG_INFO("fds_is_exists: id=%X, key=%d, ret=%d", id, key, ret);
    return ret == 0;
}

bool fds_config_file_exists() { return fds_is_exists(FDS_EMULATION_CONFIG_FILE_ID, FDS_EMULATION_CONFIG_RECORD_KEY); }

int32_t fds_read_meta(uint16_t id, uint16_t key, vfs_meta_t *meta) {
    char path[VFS_MAX_PATH_LEN];
    vfs_obj_t obj;
    fds_map_file_name(id, key, path);
    int32_t ret = vfs_get_default_driver()->stat_file(path, &obj);
    if (ret == VFS_OK) {
        vfs_meta_decode(obj.meta, sizeof(obj.meta), meta);
        return 0;
    } else {
        return -1;
    }
}

int32_t fds_write_meta(uint16_t id, uint16_t key, vfs_meta_t *meta) {
    char path[VFS_MAX_PATH_LEN];
    uint8_t meta_buf[VFS_MAX_META_LEN];
    vfs_obj_t obj;
    fds_map_file_name(id, key, path);

    vfs_meta_encode(meta_buf, sizeof(meta_buf), meta);
    return vfs_get_default_driver()->update_file_meta(path, meta_buf, sizeof(meta_buf));
}
