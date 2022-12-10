#include "spiffs_internal.h"
#include "hal_internal_flash.h"

#define LOG_PAGE_SIZE IFS_PAGE_SIZE
#define IFS_MAX_FD 4

static u8_t spiffs_work_buf[LOG_PAGE_SIZE * 2];
static u8_t spiffs_fds[32 * IFS_MAX_FD];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE + 32) * 4];

static s32_t spiffs_block_read(u32_t addr, u32_t size, u8_t *dst) { return hal_internal_flash_read(addr, dst, size); }

static s32_t spiffs_block_write(u32_t addr, u32_t size, u8_t *src) { return hal_internal_flash_prog(addr, src, size); }

static s32_t spiffs_block_erase(u32_t addr, u32_t size) { return hal_internal_flash_erase(addr); }

static spiffs fs;

ret_code_t spiffs_internal_mount() {

    ret_code_t err = hal_internal_flash_init();
    if (err) {
        return err;
    }

    spiffs_config cfg;
    cfg.phys_size = IFS_FLASH_SIZE;
    cfg.phys_addr = 0;                     // start spiffs at start of spi flash
    cfg.phys_erase_block = IFS_BLOCK_SIZE; // according to datasheet
    cfg.log_block_size = IFS_BLOCK_SIZE;   // let us not complicate things
    cfg.log_page_size = LOG_PAGE_SIZE;     // as we said

    cfg.hal_read_f = spiffs_block_read;
    cfg.hal_write_f = spiffs_block_write;
    cfg.hal_erase_f = spiffs_block_erase;

    int res = SPIFFS_mount(&fs, &cfg, spiffs_work_buf, spiffs_fds, sizeof(spiffs_fds), spiffs_cache_buf,
                           sizeof(spiffs_cache_buf), 0);
    return res;
}
spiffs *spiffs_internal_get_fs() { return &fs; }