#include "boards_defines.h"

#ifdef QSPI_FLASH
#include "app_error.h"
#include "app_util_platform.h"
#include "hal_spi_flash.h"
#include "nrf_delay.h"
#include "nrf_drv_qspi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "sdk_config.h"

#define PAGE_SIZE 256
#define CMD_ADDR_SIZE 4

/* command list */
#define CMD_WRSR (0x01)       /* Write Status Register */
#define CMD_PP (0x02)         /* Page Program */
#define CMD_READ (0x03)       /* Read Data */
#define CMD_WRDI (0x04)       /* Write Disable */
#define CMD_RDSR1 (0x05)      /* Read Status Register-1 */
#define CMD_WREN (0x06)       /* Write Enable */
#define CMD_FAST_READ (0x0B)  /* Fast Read */
#define CMD_ERASE_4K (0x20)   /* Sector Erase:4K */
#define CMD_RDSR2 (0x35)      /* Read Status Register-2 */
#define CMD_ERASE_32K (0x52)  /* 32KB Block Erase */
#define CMD_JEDEC_ID (0x9F)   /* Read JEDEC ID */
#define CMD_ERASE_full (0xC7) /* Chip Erase */
#define CMD_ERASE_64K (0xD8)  /* 64KB Block Erase */
#define CMD_DP (0xB9)         /* Deep Power Down Mode */
#define CMD_RDP (0xAB)        /* Release from Deep Power-Down */

/* JEDEC Manufacturer¡¯s ID */
#define MF_ID (0xEF)
/* JEDEC Device ID: Memory type and Capacity */
#define MTC_W25Q16_BV_CL_CV (0x4015) /* W25Q16BV W25Q16CL W25Q16CV  */
#define MTC_W25Q16_DW (0x6015)       /* W25Q16DW  */
#define MTC_W25Q32_BV (0x4016)       /* W25Q32BV */
#define MTC_W25Q32_DW (0x6016)       /* W25Q32DW */
#define MTC_W25Q64_BV_CV (0x4017)    /* W25Q64BV W25Q64CV */
#define MTC_W25Q64_DW (0x4017)       /* W25Q64DW */
#define MTC_W25Q128_BV (0x4018)      /* W25Q128BV */
#define MTC_W25Q256_FV (TBD)         /* W25Q256FV */
#define MTC_MX25L25645_GM2I (0x2019) /* MX25L25645GM2I-10G */
#define MTC_MX25L12845_GM2I (0x2018) /* MX25L25645GM2I-10G */

#define QSPI_STD_CMD_WRSR 0x01
#define QSPI_STD_CMD_WRSR_2 0x31
#define QSPI_STD_CMD_RSTEN 0x66
#define QSPI_STD_CMD_RST 0x99

#define QSPI_SR_QUAD_ENABLE_BYTE 0x2
#define QSPI_TEST_DATA_SIZE 256

#define WAIT_FOR_PERIPH()                                                                                              \
    do {                                                                                                               \
        while (!m_finished) {                                                                                          \
        }                                                                                                              \
        m_finished = false;                                                                                            \
    } while (0)

static volatile bool m_finished = false;

static void qspi_handler(nrf_drv_qspi_evt_t event, void *p_context) {
    UNUSED_PARAMETER(event);
    UNUSED_PARAMETER(p_context);
    m_finished = true;
}

static void configure_memory(void) {
    uint32_t err_code;
    uint8_t data;
    uint8_t id[3] = {0};
    nrf_qspi_cinstr_conf_t cinstr_cfg = {.opcode = QSPI_STD_CMD_RSTEN,
                                         .length = NRF_QSPI_CINSTR_LEN_1B,
                                         .io2_level = true,
                                         .io3_level = true,
                                         .wipwait = true,
                                         .wren = true};

    // Send reset enable
    err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Send reset command
    cinstr_cfg.opcode = QSPI_STD_CMD_RST;
    err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Switch to qspi mode
    data = QSPI_SR_QUAD_ENABLE_BYTE;
    cinstr_cfg.opcode = QSPI_STD_CMD_WRSR_2;
    cinstr_cfg.length = NRF_QSPI_CINSTR_LEN_2B;
    err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, &data, NULL);
    APP_ERROR_CHECK(err_code);

    while (nrfx_qspi_mem_busy_check()) {
    }
}

ret_code_t hal_spi_flash_init(void) {
    uint32_t i;
    uint32_t err_code;
    nrf_drv_qspi_config_t config = NRF_DRV_QSPI_DEFAULT_CONFIG;

    config.pins.sck_pin = QSPI_SCK_PIN;
    config.pins.csn_pin = QSPI_CS_PIN;
    config.pins.io0_pin = QSPI_IO0_PIN;
    config.pins.io1_pin = QSPI_IO1_PIN;
    config.pins.io2_pin = QSPI_IO2_PIN;
    config.pins.io3_pin = QSPI_IO3_PIN;

    config.prot_if.readoc = NRF_QSPI_READOC_READ4IO;
    config.prot_if.addrmode = NRF_QSPI_ADDRMODE_24BIT;

    err_code = nrf_drv_qspi_init(&config, qspi_handler, NULL);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("QSPI example started.");

    configure_memory();

    m_finished = false;

    return err_code;
}

ret_code_t hal_spi_flash_info(flash_info_t *info) {

    uint32_t err_code;
    uint8_t rx[3] = {0};

    nrf_qspi_cinstr_conf_t cinstr_cfg = {.opcode = CMD_JEDEC_ID,
                                         .length = QSPI_CINSTRCONF_LENGTH_4B,
                                         .io2_level = true,
                                         .io3_level = true,
                                         .wipwait = true,
                                         .wren = true};
    err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, rx);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    NRF_LOG_DEBUG("FLASH ID:");
    NRF_LOG_HEXDUMP_DEBUG(rx, 3);

    info->block_size = 4096;
    info->page_size = 256;

    /* get memory type and capacity */
    uint16_t memory_type_capacity;
    memory_type_capacity = rx[1];
    memory_type_capacity = (memory_type_capacity << 8) | rx[2];

    if (memory_type_capacity == MTC_MX25L25645_GM2I) {
        NRF_LOG_INFO("MX25L25645GM2I-10G detection");
        info->block_count = 8192;
    } else if (memory_type_capacity == MTC_MX25L12845_GM2I) {
        NRF_LOG_INFO("MTC_MX25L256_GM2I detection");
        info->block_count = 4096;
    } else if (memory_type_capacity == MTC_W25Q128_BV) {
        NRF_LOG_INFO("W25Q128BV detection");
        info->block_count = 4096;
    } else if (memory_type_capacity == MTC_W25Q64_BV_CV) {
        NRF_LOG_INFO("W25Q64BV or W25Q64CV detection");
        info->block_count = 2048;
    } else if (memory_type_capacity == MTC_W25Q64_DW) {
        NRF_LOG_INFO("W25Q64DW detection");
        info->block_count = 2048;
    } else if (memory_type_capacity == MTC_W25Q32_BV) {
        NRF_LOG_INFO("W25Q32BV detection");
        info->block_count = 1024;
    } else if (memory_type_capacity == MTC_W25Q32_DW) {
        NRF_LOG_INFO("W25Q32DW detection");
        info->block_count = 1024;
    } else if (memory_type_capacity == MTC_W25Q16_BV_CL_CV) {
        NRF_LOG_INFO("W25Q16BV or W25Q16CL or W25Q16CV detection");
        info->block_count = 512;
    } else if (memory_type_capacity == MTC_W25Q16_DW) {
        NRF_LOG_INFO("W25Q16DW detection");
        info->block_count = 2048;
    } else {
        NRF_LOG_INFO("Memory Capacity error! %d", memory_type_capacity);
        info->block_count = 0;
        return NRF_ERROR_INVALID_PARAM;
    }
    return NRF_SUCCESS;

    return err_code;
}
ret_code_t hal_spi_flash_read(uint32_t address, void *buffer, size_t size) {
    ret_code_t err_code = nrf_drv_qspi_read(buffer, size, address);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    WAIT_FOR_PERIPH();
    return err_code;
}
ret_code_t hal_spi_flash_prog(uint32_t address, const void *buffer, size_t size) {
    ret_code_t err_code = nrf_drv_qspi_write(buffer, size, address);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    WAIT_FOR_PERIPH();
    return err_code;
}
ret_code_t hal_spi_flash_erase(uint32_t address) {
    ret_code_t err_code = nrf_drv_qspi_erase(NRF_QSPI_ERASE_LEN_4KB, address);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    WAIT_FOR_PERIPH();
}
void hal_spi_flash_sleep(void) {}
void hal_spi_flash_wakeup(void) {
    // nrf_drv_qspi_uninit();
}

#endif