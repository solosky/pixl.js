#ifndef HAL_SPI_FLASH_H
#define HAL_SPI_FLASH_H

#include "nrf_error.h"
#include "nrf_log.h"
#include "nrfx.h"
#include "vfs.h"

typedef struct {
    uint32_t block_size;
    uint32_t block_count;
    uint32_t page_size;
} flash_info_t;

ret_code_t hal_spi_flash_init();
vfs_driver_t *hal_spi_flash_driver();
ret_code_t hal_spi_flash_info(flash_info_t *info);
ret_code_t hal_spi_flash_read(uint32_t address, void *buffer, size_t size);
ret_code_t hal_spi_flash_prog(uint32_t address, const void *buffer, size_t size);
ret_code_t hal_spi_flash_erase(uint32_t address);
void hal_spi_flash_sleep(void);
void hal_spi_flash_wakeup(void);

#endif