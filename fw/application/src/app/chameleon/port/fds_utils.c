//
// Created by solos on 8/27/2023.
//
#include "nrf_log.h"
#include "vfs.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

bool fds_read_sync(uint16_t id, uint16_t key, uint16_t max_length, uint8_t *buffer) {
    char path[VFS_MAX_PATH_LEN];
    sprintf(path, "/chameleon/%d_%d.bin", id, key);
    int32_t bytes_read = vfs_get_default_driver()->read_file_data(path, buffer, max_length);
    NRF_LOG_INFO("fds_read_sync: id=%X, key=%d, bytes_read=%d", id, key, bytes_read);
    return bytes_read > 0;
}
bool fds_write_sync(uint16_t id, uint16_t key, uint16_t data_length_words, void *buffer) {
    char path[VFS_MAX_PATH_LEN];
    sprintf(path, "/chameleon/%d_%d.bin", id, key);
    int32_t bytes_written = vfs_get_default_driver()->write_file_data(path, buffer, data_length_words*4);
    NRF_LOG_INFO("fds_write_sync: id=%X, key=%d, bytes_written=%d", id, key, bytes_written);
    return bytes_written > 0;
}

bool fds_is_exists(uint16_t id, uint16_t key) {
    char path[VFS_MAX_PATH_LEN];
    vfs_obj_t obj;
    sprintf(path, "/chameleon/%d_%d.bin", id, key);
    int32_t ret = vfs_get_default_driver()->stat_file(path, &obj);
    NRF_LOG_INFO("fds_is_exists: id=%X, key=%d, ret=%d", id, key, ret);
    return ret == 0;
}
