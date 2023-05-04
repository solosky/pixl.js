#include "hal_spi_flash.h"
#include "hal_spi_bus.h"
#include "nordic_common.h"
#include "nrf_drv_spi.h"
#include "nrf_error.h"
#include "nrf_gpio.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define FLASH_CS_PIN 18

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
#define MTC_MX25L25645_GM2I (0x2019)      /* MX25L25645GM2I-10G */

static spi_device_t m_dev;

static inline void hal_spi_flash_write_read(uint8_t *tx_data, uint8_t tx_len,
                                            uint8_t *rx_data, uint32_t rx_len) {
    hal_spi_bus_aquire(&m_dev);

    spi_transaction_t trans_tx = {
        .p_tx_buffer = tx_data,
        .tx_length = tx_len,
        .p_rx_buffer = NULL,
        .rx_length = 0,
    };

    ret_code_t result = hal_spi_bus_xfer(&trans_tx);
    APP_ERROR_CHECK(result);

    if (rx_len > 0) {
        spi_transaction_t trans_rx = {
            .p_tx_buffer = NULL,
            .tx_length = 0,
            .p_rx_buffer = rx_data,
            .rx_length = rx_len,
        };

        ret_code_t result = hal_spi_bus_xfer(&trans_rx);
        APP_ERROR_CHECK(result);
    }

    hal_spi_bus_release(&m_dev);
}

static inline void hal_spi_flash_write_write(uint8_t *tx_data, uint8_t tx_len,
                                             uint8_t *tx_data2, uint32_t tx_len2) {
    hal_spi_bus_aquire(&m_dev);

    spi_transaction_t trans_tx = {
        .p_tx_buffer = tx_data,
        .tx_length = tx_len,
        .p_rx_buffer = NULL,
        .rx_length = 0,
    };

    ret_code_t result = hal_spi_bus_xfer(&trans_tx);
    APP_ERROR_CHECK(result);

    if (tx_len2 > 0) {
        spi_transaction_t trans_rx = {
            .p_tx_buffer = tx_data2,
            .tx_length = tx_len2,
            .p_rx_buffer = NULL,
            .rx_length = 0,
        };

        ret_code_t result = hal_spi_bus_xfer(&trans_rx);
        APP_ERROR_CHECK(result);
    }

    hal_spi_bus_release(&m_dev);
}

static inline uint8_t hal_spi_flash_read_status(uint8_t cmd) {
    uint8_t tx[1];
    uint8_t rx[1];
    tx[0] = cmd;
    hal_spi_flash_write_read(tx, 1, rx, 1);
    return rx[0];
}

static inline void hal_spi_flash_write_cmd(uint8_t cmd) {
    uint8_t tx[1];
    tx[0] = cmd;
    hal_spi_flash_write_read(tx, 1, NULL, 0);
}

static inline void hal_spi_flash_wait_busy() {
    while (hal_spi_flash_read_status(CMD_RDSR1) & 0x01)
        ;
}

ret_code_t hal_spi_flash_init() {
    ret_code_t err_code;

    m_dev.cs_pin = FLASH_CS_PIN;
    hal_spi_bus_attach(&m_dev);

    hal_spi_flash_write_cmd(CMD_RDP);

    return NRF_SUCCESS;
}

ret_code_t hal_spi_flash_info(flash_info_t *info) {
    uint8_t rx[3] = {0};
    uint8_t tx[1] = {0x9f};
    uint16_t memory_type_capacity;

    hal_spi_flash_write_read(tx, 1, rx, 3);
    NRF_LOG_DEBUG("FLASH ID:");
    NRF_LOG_HEXDUMP_DEBUG(rx, 3);

    info->block_size = 4096;
    info->page_size = 256;

    /* get memory type and capacity */
    memory_type_capacity = rx[1];
    memory_type_capacity = (memory_type_capacity << 8) | rx[2];

    if (memory_type_capacity == MTC_MX25L25645_GM2I) {
        NRF_LOG_INFO("MX25L25645GM2I-10G detection");
        info->block_count = 8192;
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
        info->block_count = 512;
    }  else {
        NRF_LOG_INFO("Memory Capacity error! %d", memory_type_capacity);
        info->block_count = 0;
        return NRF_ERROR_INVALID_PARAM;
    }
    return NRF_SUCCESS;
}

ret_code_t hal_spi_flash_read(uint32_t address, void *buffer, size_t size) {

    uint8_t tx[4];

    hal_spi_flash_wait_busy();
    hal_spi_flash_write_cmd(CMD_WRDI);
    hal_spi_flash_wait_busy();

    tx[0] = CMD_READ;
    tx[1] = (address >> 16);
    tx[2] = (address >> 8);
    tx[3] = (address);
    hal_spi_flash_write_read(tx, 4, buffer, size);

    return NRF_SUCCESS;
}

ret_code_t hal_spi_flash_prog(uint32_t address, const void *buffer, size_t size) {

    hal_spi_flash_wait_busy();
    hal_spi_flash_write_cmd(CMD_WREN);
    hal_spi_flash_wait_busy();

    uint8_t tx[4];
    tx[0] = CMD_PP;
    tx[1] = (address >> 16);
    tx[2] = (address >> 8);
    tx[3] = (address);
    hal_spi_flash_write_write(tx, 4, buffer, size);
    hal_spi_flash_wait_busy();

    return NRF_SUCCESS;
}

ret_code_t hal_spi_flash_erase(uint32_t address) {
    hal_spi_flash_wait_busy();
    hal_spi_flash_write_cmd(CMD_WREN);
    hal_spi_flash_wait_busy();

    uint8_t tx[4];
    tx[0] = CMD_ERASE_4K;
    tx[1] = (address >> 16);
    tx[2] = (address >> 8);
    tx[3] = (address);
    hal_spi_flash_write_read(tx, 4, NULL, 0);
    hal_spi_flash_wait_busy();

    return NRF_SUCCESS;
}


void hal_spi_flash_sleep(){
    hal_spi_flash_write_cmd(CMD_DP);
}