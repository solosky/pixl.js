//
// Created by solos on 2022/1/8.
//

#ifndef FW_MTP_OBJ_H
#define FW_MTP_OBJ_H

#include "mtp_config.h"
#include "mtp_fs.h"

typedef enum {
    MTP_OBJ_SUCCESS,
    MTP_OBJ_NO_MEM
} mtp_obj_err_t;

typedef struct {
    uint8_t used;
    mtp_file_object_t file;
} mtp_obj_item_t;


typedef struct {
    uint32_t next_handle;
    mtp_obj_item_t items[MTP_MAX_NUM_OF_FILES];
} mtp_obj_pool_t;

/**
 * put a file object to the pool
 * @param pool
 * @param file_object object to put, new handle will save to handle field
 * @return result
 */
mtp_obj_err_t mtp_obj_put_object(mtp_obj_pool_t *pool, mtp_file_object_t *file_object);

/**
 * find object by parent handle and name
 * @param pool
 * @param parent_handle
 * @param file name
 * @return NULL if not found
 */
mtp_file_object_t *mtp_obj_find_object(mtp_obj_pool_t *pool, uint32_t parent_handle, char *name);

/**
 * get object by handle
 * @param pool
 * @param handle
 * @return NULL if not found
 */
mtp_file_object_t *mtp_obj_get_object(mtp_obj_pool_t *pool, uint32_t handle);

/**
 * remove a object by handle
 * @param pool
 * @param handle
 */
void mtp_obj_remove_object(mtp_obj_pool_t *pool, uint32_t handle);


/**
 * init the object pool
 * @param pool
 */
void mtp_obj_init_pool(mtp_obj_pool_t * pool);

#endif //FW_MTP_OBJ_H
