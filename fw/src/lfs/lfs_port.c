//
// Created by solos on 2021/11/21.
//
#include "hal_spi_flash.h"
#include "lfs.h"
#include "lfs_port.h"

#define LFS_SECTOR_SIZE 256
#define LFS_BLOCK_SIZE 4096

int block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                      void *buffer, lfs_size_t size) {
    return hal_spi_flash_read((block * c->block_size + off), buffer, size);
}

int block_device_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                      const void *buffer, lfs_size_t size) {
    return hal_spi_flash_prog((block * c->block_size + off), buffer, size);
}

int block_device_erase(const struct lfs_config *c, lfs_block_t block) {
    return hal_spi_flash_erase(block * c->block_size);
}

int block_device_sync(const struct lfs_config *c) { return 0; }

#define LFS_CACHE_SIZE 256

lfs_t lfs;
struct lfs_config lfs_config;

uint8_t lfs_read_buf[LFS_CACHE_SIZE];
uint8_t lfs_prog_buf[LFS_CACHE_SIZE];
uint8_t lfs_lookahead_buf[16]; // 128/8=16

ret_code_t lfs_port_init(void) {
    flash_info_t flash_info;
    ret_code_t err_code;

    // read flash info
    err_code = hal_spi_flash_info(&flash_info);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    // block device operations
    lfs_config.read = block_device_read;
    lfs_config.prog = block_device_prog;
    lfs_config.erase = block_device_erase;
    lfs_config.sync = block_device_sync;

    // block device configuration
    lfs_config.read_size = flash_info.page_size;
    lfs_config.prog_size = flash_info.page_size;
    lfs_config.block_size = flash_info.block_size;
    lfs_config.block_count = flash_info.block_count;
    lfs_config.cache_size = LFS_CACHE_SIZE;
    lfs_config.lookahead_size = 16;
    lfs_config.block_cycles = 500;

    lfs_config.read_buffer = lfs_read_buf;
    lfs_config.prog_buffer = lfs_prog_buf;
    lfs_config.lookahead_buffer = lfs_lookahead_buf;

    int err = lfs_mount(&lfs, &lfs_config);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        NRF_LOG_INFO("mount error, format as new device: %d", err);
        err = lfs_format(&lfs, &lfs_config);
        if (err) {
            NRF_LOG_INFO("lfs format error: %d", err);
            return err;
        }
        err = lfs_mount(&lfs, &lfs_config);
        if (err) {
            NRF_LOG_INFO("re mount error: %d", err);
            return err;
        }
    }

    if (!err) {
        uint32_t total_space = 0, free_space = 0;
        err_code = lfs_storage_stat(&total_space, &free_space);
        if (err_code == NRF_SUCCESS) {
            NRF_LOG_INFO("flash: total %d KB, free %d KB", total_space / 1024,
                         free_space / 1024);
        }
    }

    return err;
}

static int _traverse_df_cb(void *p, lfs_block_t block) {
    uint32_t *nb = p;
    *nb += 1;
    return 0;
}

ret_code_t lfs_storage_stat(uint32_t *p_total_space, uint32_t *p_free_space) {
    ret_code_t err;

    uint32_t _df_nballocatedblock = 0;
    err = lfs_fs_traverse(&lfs, _traverse_df_cb, &_df_nballocatedblock);
    if (err < 0) {
        return err;
    }

    uint32_t available = lfs_config.block_count * lfs_config.block_size -
                         _df_nballocatedblock * lfs_config.block_size;
    *p_free_space = available;
    *p_total_space = lfs_config.block_count * lfs_config.block_size;

    return err;
}