#ifndef HAL_INTERNAL_FLASH_HIFS
#define HAL_INTERNAL_FLASH_H


#include "nrfx.h"

#define IFS_PAGE_SIZE 256
#define IFS_BLOCK_SIZE 4096
#define IFS_BLOCK_COUNT 15

#define IFS_FLASH_BEGIN_ADDRESS 0x70000
#define IFS_FLASH_BEGIN_BLOCK 0x1B
#define IFS_FLASH_END_ADDRESS IFS_FLASH_BEGIN_ADDRESS + IFS_BLOCK_COUNT *IFS_BLOCK_SIZE
#define IFS_FLASH_SIZE IFS_BLOCK_COUNT *IFS_BLOCK_SIZE

ret_code_t hal_internal_flash_init();
ret_code_t hal_internal_flash_read(uint32_t address, uint8_t *buffer, size_t size);
ret_code_t hal_internal_flash_prog(uint32_t address, uint8_t* buffer, size_t size);
ret_code_t hal_internal_flash_erase(uint32_t address);


#endif 