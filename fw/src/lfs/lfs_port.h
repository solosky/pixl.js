//
// Created by solos on 2021/11/28.
//

#ifndef FW_LFS_PORT_H
#define FW_LFS_PORT_H

#include "lfs.h"
#include "sdk_errors.h"

#define LFS_PORT_EXTERNAL_FLASH
//#define LFS_PORT_INTERNAL_FLASH

extern lfs_t lfs;
extern struct lfs_config lfs_config;

ret_code_t lfs_port_init(void);
ret_code_t lfs_storage_stat(uint32_t *p_total_space, uint32_t *p_free_space);

#endif // FW_LFS_PORT_H
