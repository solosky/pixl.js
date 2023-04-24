#include "cache.h"

#include "ntag_emu.h"

#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "vfs.h"
#include "vfs_meta.h"

#include "nrf_pwr_mgmt.h"
#include "settings.h"

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
    uint32_t *noinit_addr = (uint32_t *)0x2000FC00;
    memset(noinit_addr, 0xFF, 0x400);
    NRF_LOG_INFO("Reset noinit ram done.");
    m_cache_data.enabled = 0;
    m_cache_data.id = 0;
    memset(m_cache_data.retain_data, 0, sizeof(uint8_t[CACHEDATASIZE]));
    memset(&(m_cache_data.ntag), 0, sizeof(ntag_t));
    return NRF_SUCCESS;
}

int32_t cache_save() {
    if (m_cache_data.id == 0 || cache_empty(m_cache_data.retain_data)) {
        return NRF_SUCCESS;
    }
    m_cache_data.enabled = 1;
    NRF_LOG_INFO("Saving cache...");
    NRF_LOG_INFO("Cache data: enabled = %d, id = %d", m_cache_data.enabled, m_cache_data.id);
    return NRF_SUCCESS;
}

cache_data_t *cache_get_data() {
    NRF_LOG_INFO("Cache data: enabled = %d, id = %d",
                 m_cache_data.enabled, m_cache_data.id);
    cache_init();
    return &m_cache_data;
}

bool cache_empty(uint8_t *data) {
    if (data == NULL) {
        return false;
    }
    return memcmp(data, (uint8_t[CACHEDATASIZE]) {0}, CACHEDATASIZE) == 0;
}