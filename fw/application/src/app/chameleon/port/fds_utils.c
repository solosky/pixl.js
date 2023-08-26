//
// Created by solos on 8/27/2023.
//
#include<stdint.h>
#include<stdbool.h>
#include "nrf_log.h"

bool fds_read_sync(uint16_t id, uint16_t key, uint16_t max_length, uint8_t *buffer){
    NRF_LOG_INFO("fds_read_sync: id=%X, key=%d", id, key);
    return 0;
}
bool fds_write_sync(uint16_t id, uint16_t key, uint16_t data_length_words, void *buffer){
    NRF_LOG_INFO("fds_write_sync: id=%X, key=%d", id, key);
    return 0;
}

bool fds_is_exists(uint16_t id, uint16_t key) {
    NRF_LOG_INFO("fds_is_exists: id=%X, key=%d", id, key);
    return 0;
}
