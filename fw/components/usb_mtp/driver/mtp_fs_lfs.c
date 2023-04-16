/**
* Copyright (C) 2019 Bosch Sensortec GmbH
*
* SPDX-License-Identifier: BSD-3-Clause
*
* @file    lfs_glue.c
* @date    Jun 30, 2019
* @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
* @brief
*/
/**********************************************************************************/
/* header includes */
/**********************************************************************************/
#include <stdint.h>
#include <string.h>
#include "mtp_config.h"
#include "mtp_fs.h"
#include "lfs.h"
#include "mtp_fs_lfs.h"
#include "sdk_macros.h"
#include "mtp_types.h"
#include "mtp_obj.h"

#include "nrf_log.h"


extern lfs_t lfs;
extern struct lfs_config lfs_config;

#define ROOT_HANDLE 0xFFFFFFFF

int32_t lfs_glue_init();

int32_t lfs_glue_format();

int32_t lfs_glue_get_storage_info(mtp_storage_info_t *storage_info);

int32_t lfs_glue_get_object_handles(uint32_t parent_handle, uint32_t *handles, uint32_t *len);

int32_t lfs_glue_get_object_info_by_handle(uint32_t handle, mtp_file_object_t *fs_object);

int32_t lfs_glue_file_read(uint32_t handle, uint32_t offset, uint8_t *buff, uint32_t len);

int32_t lfs_glue_file_delete(uint32_t handle);

int32_t lfs_glue_file_close(uint32_t handle);

int32_t lfs_glue_file_open(uint32_t handle, uint32_t flag);

int32_t lfs_glue_send_object_info_by_handle(uint32_t *handle, mtp_file_object_t *fs_object);

int32_t lfs_glue_file_write(uint32_t handle, uint32_t offset, uint8_t *buff, uint32_t len);

int32_t lfs_glue_file_rename(uint32_t handle, char *file_name);

int32_t lfs_glue_file_move(uint32_t handle, uint32_t parent_handle);

int32_t lfs_glue_deinit();

int32_t lfs_glue_delete_invalid_file_block(void);

const mtp_fs_driver_t lfs_glue_op = {
   .fs_init = lfs_glue_init,
   .fs_format = lfs_glue_format,
   .fs_get_storage_info = lfs_glue_get_storage_info,
   .fs_get_object_handles = lfs_glue_get_object_handles,
   .fs_get_object_info_by_handle = lfs_glue_get_object_info_by_handle,
   .fs_file_read = lfs_glue_file_read,
   .fs_file_delete = lfs_glue_file_delete,
   .fs_file_open = lfs_glue_file_open,
   .fs_file_close = lfs_glue_file_close,
   .fs_send_object_info_by_handle = lfs_glue_send_object_info_by_handle,
   .fs_file_write = lfs_glue_file_write,
   .fs_file_rename = lfs_glue_file_rename,
   .fs_file_move = lfs_glue_file_move,
   .fs_deinit = lfs_glue_deinit,
   .fs_pending_task = lfs_glue_delete_invalid_file_block,
};


lfs_file_t lfs_file;
mtp_obj_pool_t obj_pool;

static uint32_t make_file_path(uint32_t handle, char *path, size_t path_len) {
   uint32_t next = handle;
   uint32_t pos = path_len - 1;
   mtp_file_object_t file_object;
   memset(path, 0, path_len);
   while (next != ROOT_HANDLE) {
       lfs_glue_get_object_info_by_handle(next, &file_object);
       size_t len = strlen(file_object.name);
       pos = pos - len;
       if (pos < 0) {
           //overflow
           return -1;
       }
       memcpy(path + pos, file_object.name, len);
       path[--pos] = '/';
       next = file_object.parent;
   }

   //ROOT
   if (pos == path_len) {
       path[--pos] = '/';
   }

   memcpy(path, path + pos, path_len - pos);
   path[pos + 1] = '\0';

   return pos;


}

/*!
*
* @brief       : Handler to create default files after format
*
* @param[in]   : None
*
* @return      : None
*/

/*!
*
* @brief       : Handler to start lfs
*/
const mtp_fs_driver_t *lfs_glue_driver() {
   return &lfs_glue_op;
}

/*!
*
* @brief       : Handler to initialize lfs
*
* @param[in]   : None
*
* @return      : 0
*/
int32_t lfs_glue_init() {
   //lfs should be initialized by lfs_port.c
   mtp_obj_init_pool(&obj_pool);
   return 0;
}

/*!
*
* @brief       : Handler for formatting flash memory as lfs
*
* @param[in]   : None
*
* @return      : 0 or 1
*/
int32_t lfs_glue_format() {
   return lfs_format(&lfs, &lfs_config);
}

/*!
*
* @brief       : Handler to get storage information of flash memory
*
* @param[in]   : mtp_storage_info_t * storage_info
*
* @return      : 0
*/
int32_t lfs_glue_get_storage_info(mtp_storage_info_t *storage_info) {
   uint32_t total_space, free_space;
   ret_code_t err = lfs_storage_stat(&total_space, &free_space);
   if (NRF_SUCCESS == err) {
       storage_info->capacity = total_space;
       storage_info->free_space = free_space;
   }
   return err;
}

/*!
*
* @brief       : Handler to get object handles of files
*
* @param[in]   : *handles, *len
*
* @return      : 0
*/
int32_t lfs_glue_get_object_handles(uint32_t parent_handle, uint32_t *handles, uint32_t *len) {

   lfs_dir_t dir;
   struct lfs_info lfs_info;
   int err_code;
   mtp_file_object_t file_obj;
   uint32_t num_of_files = 0;

   char path[MTP_MAX_PATH];
   if (make_file_path(parent_handle, path, sizeof(path)) < 0) {
       *len = 0;
       return -1;
   }
   err_code = lfs_dir_open(&lfs, &dir, path);
   VERIFY_SUCCESS(err_code);
   while (lfs_dir_read(&lfs, &dir, &lfs_info) > 0) {
       if (strcmp(lfs_info.name, ".") != 0 && strcmp(lfs_info.name, "..") != 0) {
           file_obj.parent = parent_handle;
           file_obj.size = lfs_info.size;
           if (lfs_info.type == LFS_TYPE_DIR) {
               file_obj.file_type = FS_FILE_TYPE_DIR;
           } else {
               file_obj.file_type = FS_FILE_TYPE_REG;
           }
           strcpy(file_obj.name,  lfs_info.name);

           if (mtp_obj_put_object(&obj_pool, &file_obj) != MTP_OBJ_SUCCESS) {
               return -1;
           }
           handles[num_of_files++] = file_obj.handle;
       }
   }

   lfs_dir_close(&lfs, &dir);
   *len = num_of_files;
   return 0;
}

/*!
*
* @brief       : Handler to get object information by handle
*
* @param[in]   : handle,mtp_file_object_t *fs_object
*
* @return      : 0
*/
int32_t lfs_glue_get_object_info_by_handle(uint32_t handle, mtp_file_object_t *fs_object) {
   mtp_file_object_t *object = mtp_obj_get_object(&obj_pool, handle);
   if (object == NULL) {
       return -1;
   }
   memcpy(fs_object, object, sizeof(mtp_file_object_t));
   return 0;
}

/*!
*
* @brief       : Handler to read file by object handle
*
* @param[in]   : handle,mtp_file_object_t *fs_object
*
* @return      : Number of bytes read
*/
int32_t lfs_glue_file_read(uint32_t handle, uint32_t offset, uint8_t *buff, uint32_t len) {
   mtp_file_object_t *object = mtp_obj_get_object(&obj_pool, handle);
   if (object != NULL && offset < object->size) {
       lfs_file_seek(&lfs, &lfs_file, offset, LFS_SEEK_SET);
       return lfs_file_read(&lfs, &lfs_file, buff, len);
   } else {
       return -1;
   }

}

/*!
*
* @brief       : Handler to delete a file
*
* @param[in]   : handle
*
* @return      : 0 or -1
*/
int32_t lfs_glue_file_delete(uint32_t handle) {
   char path[MTP_MAX_PATH];
   if (make_file_path(handle, path, sizeof(path)) < 0) {
       return -1;
   }
   int err = lfs_remove(&lfs, path);
   if (!err) {
       mtp_obj_remove_object(&obj_pool, handle);
   }
   return err;
}

int32_t lfs_glue_file_open(uint32_t handle, uint32_t flag) {
   int lfs_flag = 0;
   if (flag & FS_FLAG_READ) {
       lfs_flag = LFS_O_RDONLY;
   } else if (flag & FS_FLAG_WRITE) {
       lfs_flag = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC;
   }
   char path[MTP_MAX_PATH];
   if (make_file_path(handle, path, sizeof(path)) < 0) {
       return -1;
   }
   return lfs_file_open(&lfs, &lfs_file, path, lfs_flag);
}


int32_t lfs_glue_file_close(uint32_t handle) {
   return lfs_file_close(&lfs, &lfs_file);
}

/*!
*
* @brief       : Handler to send the object information by handle
*
* @param[in]   : handle
*
* @return      : 0
*/
int32_t lfs_glue_send_object_info_by_handle(uint32_t *handle, mtp_file_object_t *fs_object) {
   if (mtp_obj_put_object(&obj_pool, fs_object) != MTP_OBJ_SUCCESS) {
       return -1;
   }
   *handle = fs_object->handle;
   if (fs_object->file_type == FS_FILE_TYPE_DIR) {
       char path[MTP_MAX_PATH];
       if (make_file_path(*handle, path, sizeof(path)) < 0) {
           return -1;
       }
       lfs_mkdir(&lfs, path);
   }

   return 0;
}

/*!
*
* @brief       : Handler to send the object information by handle
*
* @param[in]   : handle,offset,*buff,len
*
* @return      : Number of bytes written
*/
int32_t lfs_glue_file_write(uint32_t handle, uint32_t offset, uint8_t *buff, uint32_t len) {
   mtp_file_object_t *object = mtp_obj_get_object(&obj_pool, handle);
   if (object != NULL && offset < object->size) {
       return lfs_file_write(&lfs, &lfs_file, buff, len);
   }

   return -1;

}

int32_t lfs_glue_file_rename(uint32_t handle, char *file_name) {
   mtp_file_object_t *object = mtp_obj_get_object(&obj_pool, handle);
   if (object == NULL) {
       return -1;
   }

   char old_file_path[MTP_MAX_PATH];
   char new_file_path[MTP_MAX_PATH];

   make_file_path(handle, old_file_path, sizeof(old_file_path));
   make_file_path(object->parent, new_file_path, sizeof(new_file_path));
   strcat(new_file_path, "/");
   strcat(new_file_path, file_name);

   if (lfs_rename(&lfs, old_file_path, new_file_path) < 0) {
       return -1;
   }

   strcpy(object->name, file_name);
   return 0;
}


int32_t lfs_glue_file_move(uint32_t handle, uint32_t parent_handle) {
   mtp_file_object_t *object = mtp_obj_get_object(&obj_pool, handle);
   mtp_file_object_t *parent_object = mtp_obj_get_object(&obj_pool, parent_handle);
   if (object == NULL || parent_object == NULL) {
       return -1;
   }
   char old_file_path[MTP_MAX_PATH];
   char new_file_path[MTP_MAX_PATH];

   make_file_path(handle, old_file_path, sizeof(old_file_path));
   make_file_path(parent_handle, new_file_path, sizeof(new_file_path));
   strcat(new_file_path, "/");
   strcat(new_file_path, object->name);

   if (lfs_rename(&lfs, old_file_path, new_file_path) < 0) {
       return -1;
   }

   object->parent = parent_handle;
   return 0;

}


/*!
*
* @brief       : Handler to deinitialize filesystem
*
* @param[in]   : None
*
* @return      : 0
*/
int32_t lfs_glue_deinit() {
   return 0;
}

/*!
*
* @brief       : Handler to delete invalidated file block
*
* @param[in]   : None
*
* @return      : 0
*/
int32_t lfs_glue_delete_invalid_file_block(void) {
   return 0;
}
