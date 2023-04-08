/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 *  
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mtp_config.h
 * @date    June 24, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   MTP stack configuration file
 */
#ifndef MTP_CONFIG_H_
#define MTP_CONFIG_H_

#define EP_DATA_SIZE 64
#define MTP_BUFF_SIZE 1024

#define MTP_MAX_FILENAME_LEN 32
#define MTP_MAX_PATH LFS_NAME_MAX*3

#define MTP_STR_MANUFACTURER_NAME "NFC LAB"
#define MTP_STR_MODEL_NAME "Pixl.js"
#define MTP_STR_DEVICE_VERSION "1.1"
#define MTP_STR_DEVICE_SERIAL "30031993"

#define MTP_STR_VOLUME_ID "FLASH MEMORY"
#define MTP_STR_STORAGE_DESCRIPTOR MTP_STR_VOLUME_ID
#define MTP_STR_DEVICE_FRIENDLY_NAME MTP_STR_MODEL_NAME

#define MTP_MAX_NUM_OF_FILES 128


#endif /* MTP_CONFIG_H_ */
