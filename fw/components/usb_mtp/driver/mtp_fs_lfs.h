/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    lfs_glue.h
 * @date    Jun 30, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief
 */

#ifndef LFS_GLUE_H_
#define LFS_GLUE_H_

#include <stdint.h>
#include "mtp_fs.h"

const mtp_fs_driver_t* lfs_glue_driver();


#define STORAGE_CAPACITY (128*1024*1024)

#endif /* LFS_GLUE_H_ */
