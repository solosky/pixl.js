//
// Created by solos on 2021/11/21.
//

#include "lfs_port.h"

#ifdef LFS_PORT_INTERNAL_FLASH

#include "lfs.h"
#include "nrf_fstorage.h"
#include "nrf_log.h"

#include "nrf.h"
#include "nrf_soc.h"

#ifdef SOFTDEVICE_PRESENT
#include "nrf_fstorage_sd.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#else
#include "nrf_drv_clock.h"
#include "nrf_fstorage_nvmc.h"
#endif

#define LFS_READ_SIZE 128
#define LFS_PROG_SIZE 128
#define LFS_BLOCK_SIZE 4096
#define LFS_CACHE_SIZE 512
#define LFS_BLOCK_COUNT 16

#define LFS_FLASH_BEGIN_ADDRESS 0x70000
#define LFS_FLASH_BEGIN_BLOCK 0x1B
#define LFS_FLASH_END_ADDRESS LFS_FLASH_BEGIN_ADDRESS + LFS_BLOCK_COUNT *LFS_BLOCK_SIZE

static void fstorage_evt_handler(nrf_fstorage_evt_t *p_evt);

volatile uint32_t m_op_result_code = 0;
nrf_fstorage_api_t *m_p_fs_api;

NRF_FSTORAGE_DEF(nrf_fstorage_t m_fstorage) = {
    /* Set a handler for fstorage events. */
    .evt_handler = fstorage_evt_handler,

    /* These below are the boundaries of the flash space assigned to this instance of
     * fstorage. You must set these manually, even at runtime, before nrf_fstorage_init()
     * is called. The function nrf5_flash_end_addr_get() can be used to retrieve the last
     * address on the last page of flash available to write data. */
    .start_addr = LFS_FLASH_BEGIN_ADDRESS,
    .end_addr = LFS_FLASH_END_ADDRESS,
};

ret_code_t storage_init() {

#ifdef SOFTDEVICE_PRESENT
    NRF_LOG_INFO("SoftDevice is present.");
    NRF_LOG_INFO("Initializing nrf_fstorage_sd implementation...");
    /* Initialize an fstorage instance using the nrf_fstorage_sd backend.
     * nrf_fstorage_sd uses the SoftDevice to write to flash. This implementation can
     * safely be used whenever there is a SoftDevice, regardless of its status
     * (enabled/disabled). */
    m_p_fs_api = &nrf_fstorage_sd;
#else
    NRF_LOG_INFO("SoftDevice not present.");
    NRF_LOG_INFO("Initializing nrf_fstorage_nvmc implementation...");
    /* Initialize an fstorage instance using the nrf_fstorage_nvmc backend.
     * nrf_fstorage_nvmc uses the NVMC peripheral. This implementation can be used when
     * the SoftDevice is disabled or not present.
     *
     * Using this implementation when the SoftDevice is enabled results in a hardfault. */
    m_p_fs_api = &nrf_fstorage_nvmc;
#endif

    return nrf_fstorage_init(&m_fstorage, m_p_fs_api, NULL);
}

/**@brief   Sleep until an event is received. */
static void power_manage(void) {
#ifdef SOFTDEVICE_PRESENT
    (void)sd_app_evt_wait();
#else
    __WFE();
#endif
}

void wait_for_flash_ready(nrf_fstorage_t const *p_fstorage) {
    /* While fstorage is busy, sleep and wait for an event. */
    while (nrf_fstorage_is_busy(p_fstorage)) {
        power_manage();
    }
}

static void fstorage_evt_handler(nrf_fstorage_evt_t *p_evt) {
    if (p_evt->result != NRF_SUCCESS) {
        m_op_result_code = -1;
        NRF_LOG_INFO("--> FS ERROR: %d", p_evt->result);
        return;
    }

    switch (p_evt->id) {
    case NRF_FSTORAGE_EVT_WRITE_RESULT: {
        m_op_result_code = 0;
        // NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.", p_evt->len,
        //              p_evt->addr);
    } break;

    case NRF_FSTORAGE_EVT_ERASE_RESULT: {
        m_op_result_code = 0;
        // NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.", p_evt->len,
        //              p_evt->addr);
    } break;

    default:
        break;
    }
}

int block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                      void *buffer, lfs_size_t size) {
    uint32_t dest = (LFS_FLASH_BEGIN_ADDRESS + (block * c->block_size + off));
    memcpy(buffer, (void *)dest, size);
    //NRF_LOG_INFO("R:%d,%d", block, off);
    return 0;
}

int block_device_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                      const void *buffer, lfs_size_t size) {

    ret_code_t rc;

    uint32_t dest = (LFS_FLASH_BEGIN_ADDRESS + (block * c->block_size + off));
    

    m_op_result_code = 0;
    rc = nrf_fstorage_write(&m_fstorage, dest, buffer, size, NULL);
    if (rc != NRF_SUCCESS) {
        NRF_LOG_INFO("P:%X,%d,%d", dest, size, rc);
        return -1;
    }
    wait_for_flash_ready(&m_fstorage);

    return m_op_result_code;
}

int block_device_erase(const struct lfs_config *c, lfs_block_t block) {
    ret_code_t rc;

    uint32_t dest = (LFS_FLASH_BEGIN_ADDRESS + (block * c->block_size));

    m_op_result_code = 0;
    rc = nrf_fstorage_erase(&m_fstorage, dest, 1, NULL);
    if (rc != NRF_SUCCESS) {
        NRF_LOG_INFO("E:%X,%d", dest,rc);
        return -1;
    }
    wait_for_flash_ready(&m_fstorage);

    return m_op_result_code;
}

int block_device_sync(const struct lfs_config *c) { return 0; }

lfs_t lfs;
struct lfs_config lfs_config;

uint8_t lfs_read_buf[LFS_CACHE_SIZE];
uint8_t lfs_prog_buf[LFS_CACHE_SIZE];
uint8_t lfs_lookahead_buf[16]; // 128/8=16

void lfs_create_directory(char *path) {
    int err = lfs_mkdir(&lfs, path);
    if (!err) {
        NRF_LOG_INFO("created directory: %s", path);
    }
}

ret_code_t lfs_port_init(void) {
    ret_code_t err_code;

    err_code = storage_init();
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_INFO("storage init failed!");
    }

    // block device operations
    lfs_config.read = block_device_read;
    lfs_config.prog = block_device_prog;
    lfs_config.erase = block_device_erase;
    lfs_config.sync = block_device_sync;

    // block device configuration
    lfs_config.read_size = LFS_READ_SIZE;
    lfs_config.prog_size = LFS_PROG_SIZE;
    lfs_config.block_size = LFS_BLOCK_SIZE;
    lfs_config.block_count = LFS_BLOCK_COUNT;
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

        lfs_create_directory("amiibo");

        // test only ///
        //  char path[16];
        //  sprintf(path, "amiibo/%02d.bin", 0);
        //  lfs_file_t file;
        //  err = lfs_file_open(&lfs, &file, path, LFS_O_RDONLY);
        //  NRF_LOG_INFO("open bin: %d", err);
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


#endif