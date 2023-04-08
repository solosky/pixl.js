/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _MTP_FS_H
#define _MTP_FS_H
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    FS_FILE_TYPE_REG,
    FS_FILE_TYPE_DIR
}file_type_t;

/*!
 * @brief   File object representation
 */
typedef struct
{
    uint32_t handle; /*< handle*/
    uint32_t parent; /*<parent handle>*/
    char name[32]; /*< file name */
    uint32_t file_type; /* 0: regular file, 1: folder */
    uint32_t size; /*< size */
} mtp_file_object_t;

/*!
 * @brief   File system capacity information
 */
typedef struct
{
    uint64_t capacity; /*< total capacity */
    uint64_t free_space; /*< free space */
} mtp_storage_info_t;

typedef enum{
    FS_FLAG_READ = 1,
    FS_FLAG_WRITE = 2
} fs_flag_t;




/*!
 * @brief   MTP protocol - file system interface handlers
 */
typedef struct
{
    int32_t (*fs_init)(); /*< MTP OpenSession callback (D.2.2)*/
    int32_t (*fs_format)(); /*< MTP FormatStore callback (D.2.15)*/
    int32_t (*fs_get_storage_info)(mtp_storage_info_t * storage_info); /*< MTP GetStorageInfo callback (D.2.5)*/
    int32_t (*fs_get_object_handles)(uint32_t parent_handle, uint32_t *handles, uint32_t *len); /*< MTP GetObjectHandles callback (D.2.7)*/
    int32_t (*fs_get_object_info_by_handle)(uint32_t handle, mtp_file_object_t *fs_object); /*< MTP GetObjectInfo callback (D.2.8)*/
    int32_t (*fs_file_read)(uint32_t handle, uint32_t offset, uint8_t *buff, uint32_t len); /*< MTP GetObject callback (D.2.9)*/
    int32_t (*fs_file_close)(uint32_t handle); /*< MTP GetObject callback (D.2.9)*/
    int32_t (*fs_file_open)(uint32_t handle, uint32_t flags); /*< MTP GetObject callback (D.2.9)*/
    int32_t (*fs_file_delete)(uint32_t handle); /*< MTP DeleteObject callback (D.2.11)*/
    int32_t (*fs_send_object_info_by_handle)(uint32_t *handle, mtp_file_object_t *fs_object); /*< MTP SendObjectInfo callback (D.2.12)*/
    int32_t (*fs_file_write)(uint32_t handle, uint32_t offset, uint8_t *buff, uint32_t len); /*< MTP SendObject callback (D.2.13)*/
    int32_t (*fs_file_rename)(uint32_t handle, char* new_name);
    int32_t (*fs_file_move)(uint32_t handle, uint32_t new_parent);
    int32_t (*fs_deinit)(); /*< MTP CloseSession callback (D.2.3)*/
    int32_t (*fs_pending_task)(); /*< Do any deferred/pending tasks in FS*/
} mtp_fs_driver_t;



#endif // _MTP_H