#include "hal_spi_flash.h"
#include "spiffs_internal.h"

#include "nrf_log.h"

#define LOG_PAGE_SIZE 256
#define LOG_BLOCK_SIZE 4096
#define LOG_FLASH_SIZE
#define IFS_MAX_FD 4

static u8_t spiffs_work_buf[LOG_PAGE_SIZE * 2];
static u8_t spiffs_fds[32 * IFS_MAX_FD];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE + 32) * 4];

static s32_t spiffs_block_read(u32_t addr, u32_t size, u8_t *dst) { return hal_spi_flash_read(addr, dst, size); }

static s32_t spiffs_block_write(u32_t addr, u32_t size, u8_t *src) { return hal_spi_flash_prog(addr, src, size); }

static s32_t spiffs_block_erase(u32_t addr, u32_t size) { return hal_spi_flash_erase(addr); }

static spiffs fs;

ret_code_t spiffs_external_mount() {

    flash_info_t flash_info;
    ret_code_t err_code;

    err_code = hal_spi_flash_init();
    if (err_code) {
        return err_code;
    }

    // read flash info
    err_code = hal_spi_flash_info(&flash_info);
    if (err_code) {
        return err_code;
    }

    spiffs_config cfg;
    cfg.phys_size = flash_info.block_count * flash_info.block_size;
    cfg.phys_addr = 0;                            // start spiffs at start of spi flash
    cfg.phys_erase_block = flash_info.block_size; // according to datasheet
    cfg.log_block_size = flash_info.block_size;   // let us not complicate things
    cfg.log_page_size = flash_info.page_size;     // as we said

    cfg.hal_read_f = spiffs_block_read;
    cfg.hal_write_f = spiffs_block_write;
    cfg.hal_erase_f = spiffs_block_erase;

    int res = SPIFFS_mount(&fs, &cfg, spiffs_work_buf, spiffs_fds, sizeof(spiffs_fds), spiffs_cache_buf,
                           sizeof(spiffs_cache_buf), 0);
    NRF_LOG_INFO("external fs mount: %d", res);

    int force_format = 0;

    if (res || force_format) {
        if (SPIFFS_mounted(&fs)) {
            SPIFFS_unmount(&fs);
        }

        res = SPIFFS_format(&fs);
        NRF_LOG_INFO("external fs format: %d", res);
        if (!res) {
            res = SPIFFS_mount(&fs, &cfg, spiffs_work_buf, spiffs_fds, sizeof(spiffs_fds), spiffs_cache_buf,
                               sizeof(spiffs_cache_buf), 0);
            NRF_LOG_INFO("external fs re-mount: %d", res);
        }
    }
    return res;
}
spiffs *spiffs_external_get_fs() { return &fs; }