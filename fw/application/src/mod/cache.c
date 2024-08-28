#include "cache.h"

#include "ntag_emu.h"

#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "vfs.h"
#include "vfs_meta.h"

#include "nrf_pwr_mgmt.h"
#include "settings2.h"
#include "crc32.h"

#define NOINIT_RAM_INDEX 1


extern uint32_t __start_noinit;
extern uint32_t __stop_noinit;

static __attribute__((section(".noinit"))) cache_data_t m_cache_data;
static __attribute__((section(".noinit"))) int32_t m_cache_crc32;

bool cache_valid(){
    size_t noinit_size = &(__stop_noinit) - &(__start_noinit);
    NRF_LOG_INFO("noinit area: [0x%X, 0x%X], %d bytes",  &__start_noinit, &__stop_noinit, noinit_size);
    NRF_LOG_INFO("m_cache_data address: 0x%X", &m_cache_data);
    return m_cache_crc32 == crc32_compute((const int8_t *)&m_cache_data, sizeof(cache_data_t), NULL);
}

int32_t cache_clean() {
    NRF_LOG_INFO("Cleaning cache...")
    // 重置一下noinit ram区域
    uint32_t *noinit_addr = &__start_noinit;
    size_t noinit_size = (&__stop_noinit - &__start_noinit);
    memset(noinit_addr, 0x0, noinit_size);
    m_cache_crc32 = crc32_compute(&m_cache_data, sizeof(cache_data_t), NULL);

    NRF_LOG_INFO("Reset noinit ram done.");
    return NRF_SUCCESS;
}

int32_t cache_save() {
    NRF_LOG_INFO("Saving cache...");
    m_cache_crc32 = crc32_compute(&m_cache_data, sizeof(cache_data_t), NULL);
    NRF_LOG_INFO("Cache data: enabled = %d, id = %d", m_cache_data.enabled, m_cache_data.id);

    //set ram retention
    uint32_t ram1_power = 0;
    sd_power_ram_power_get(NOINIT_RAM_INDEX, &ram1_power);
    NRF_LOG_INFO("RAM1 power: 0x%X", ram1_power);
    ram1_power = POWER_RAM_POWER_S0RETENTION_On << POWER_RAM_POWER_S0RETENTION_Pos;
    sd_power_ram_power_set(NOINIT_RAM_INDEX, ram1_power);

    return NRF_SUCCESS;
}

cache_data_t *cache_get_data() {
    NRF_LOG_INFO("Cache data: enabled = %d, id = %d", m_cache_data.enabled, m_cache_data.id);
    return &m_cache_data;
}
