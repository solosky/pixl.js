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

#include "hal_internal_flash.h"




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
    .start_addr = IFS_FLASH_BEGIN_ADDRESS,
    .end_addr = IFS_FLASH_END_ADDRESS,
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

ret_code_t hal_internal_flash_init() { return storage_init(); }

ret_code_t hal_internal_flash_read(uint32_t address, uint8_t *buffer, size_t size) {
    memcpy(buffer, (void *)(address + IFS_FLASH_BEGIN_ADDRESS), size);
}

ret_code_t hal_internal_flash_prog(uint32_t address, uint8_t *buffer, size_t size) {
    ret_code_t rc;

    uint32_t dest = IFS_FLASH_BEGIN_ADDRESS + address;

    m_op_result_code = 0;
    rc = nrf_fstorage_write(&m_fstorage, dest, buffer, size, NULL);
    if (rc != NRF_SUCCESS) {
        NRF_LOG_INFO("P:%X,%d,%d", dest, size, rc);
        return -1;
    }
    wait_for_flash_ready(&m_fstorage);

    return m_op_result_code;
}

ret_code_t hal_internal_flash_erase(uint32_t address) {
    ret_code_t rc;

    uint32_t dest = address;

    m_op_result_code = 0;
    rc = nrf_fstorage_erase(&m_fstorage, dest, 1, NULL);
    if (rc != NRF_SUCCESS) {
        NRF_LOG_INFO("E:%X,%d", dest, rc);
        return -1;
    }
    wait_for_flash_ready(&m_fstorage);

    return m_op_result_code;
}