#include "cache.h"

#include "ntag_emu.h"

#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "vfs.h"
#include "vfs_meta.h"

#include "nrf_pwr_mgmt.h"
#include "settings.h"

#define CACHE_FILE_NAME "/cache.bin"

static __attribute__((section(".noinit"))) cache_data_t m_cache_data;

int32_t cache_init() {
    if (m_cache_data.enabled != 1 && m_cache_data.enabled != 0) {
        cache_clean();
    }
    return NRF_SUCCESS;
}

int32_t cache_clean() {
    NRF_LOG_INFO("Cleaning cache...")
    // 重置一下noinit ram区域
    uint32_t *noinit_addr = (uint32_t *)0x2000FC80;
    memset(noinit_addr, 0xFF, 0x380);
    NRF_LOG_INFO("Reset noinit ram done.");
    m_cache_data.enabled = 0;
    m_cache_data.current_drive = VFS_DRIVE_MAX;
    strcpy(&(m_cache_data.current_file), "");
    strcpy(&(m_cache_data.current_folder), "");
    memset(&(m_cache_data.tag), 0, sizeof(ntag_t));

    return NRF_SUCCESS;
}

int32_t cache_save() {
    if (strcmp(m_cache_data.current_file, "") == 0 || strcmp(m_cache_data.current_folder, "") == 0 || m_cache_data.current_drive == VFS_DRIVE_MAX) {
        return NRF_SUCCESS;
    }
    m_cache_data.enabled = 1;
    return NRF_SUCCESS;
}

cache_data_t *cache_get_data() {
    NRF_LOG_INFO("Cache data: enabled = %d, drive = %d, folder = %s, file = %s",
                 m_cache_data.enabled, m_cache_data.current_drive,
                 m_cache_data.current_folder, m_cache_data.current_file);
    return &m_cache_data;
}