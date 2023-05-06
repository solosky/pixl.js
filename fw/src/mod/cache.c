#include "cache.h"

#include "ntag_emu.h"

#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "vfs.h"
#include "vfs_meta.h"

#include "nrf_pwr_mgmt.h"
#include "settings.h"
#include "crc32.h"

static __attribute__((section(".noinit"))) cache_data_t m_cache_data;
static __attribute__((section(".noinit"))) int32_t m_cache_crc32;

bool cache_valid(){
    return m_cache_crc32 == crc32_compute(&m_cache_data, sizeof(cache_data_t), NULL);
}

int32_t cache_clean() {
    NRF_LOG_INFO("Cleaning cache...")
    // 重置一下noinit ram区域
    uint32_t *noinit_addr = (uint32_t *)0x2000FC00;
    memset(noinit_addr, 0x0, 0x400);
    m_cache_crc32 = crc32_compute(&m_cache_data, sizeof(cache_data_t), NULL);

    NRF_LOG_INFO("Reset noinit ram done.");
    return NRF_SUCCESS;
}

int32_t cache_save() {
    NRF_LOG_INFO("Saving cache...");
    m_cache_crc32 = crc32_compute(&m_cache_data, sizeof(cache_data_t), NULL);
    NRF_LOG_INFO("Cache data: enabled = %d, id = %d", m_cache_data.enabled, m_cache_data.id);
    return NRF_SUCCESS;
}

cache_data_t *cache_get_data() {
    NRF_LOG_INFO("Cache data: enabled = %d, id = %d", m_cache_data.enabled, m_cache_data.id);
    return &m_cache_data;
}
