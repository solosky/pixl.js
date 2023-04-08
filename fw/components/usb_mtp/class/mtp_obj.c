//
// Created by solos on 2022/1/8.
//

#include <string.h>
#include "mtp_obj.h"

static mtp_obj_item_t *mtp_obj_add_item(mtp_obj_pool_t *pool) {
    for (uint32_t i = 0; i < MTP_MAX_NUM_OF_FILES; i++) {
        mtp_obj_item_t *item = &pool->items[i];
        if (!item->used) {
            return item;
        }
    }
    return NULL;
}

static mtp_obj_item_t *mtp_obj_find_item_by_handle(mtp_obj_pool_t *pool, uint32_t handle) {
    for (uint32_t i = 0; i < MTP_MAX_NUM_OF_FILES; i++) {
        mtp_obj_item_t *item = &pool->items[i];
        if (item->used && item->file.handle == handle) {
            return item;
        }
    }
    return NULL;
}

mtp_obj_err_t mtp_obj_put_object(mtp_obj_pool_t *pool, mtp_file_object_t *file_object) {
    mtp_obj_item_t *item = mtp_obj_add_item(pool);
    if (item == NULL) {
        return MTP_OBJ_NO_MEM;
    }

    item->used = 1;
    file_object->handle = pool->next_handle++;
    memcpy(&item->file, file_object, sizeof(mtp_file_object_t));


    return MTP_OBJ_SUCCESS;
}

mtp_file_object_t *mtp_obj_find_object(mtp_obj_pool_t *pool, uint32_t parent_handle, char *name) {
    for (uint32_t i = 0; i < MTP_MAX_NUM_OF_FILES; i++) {
        mtp_obj_item_t *item = &pool->items[i];
        if (item->used && item->file.parent == parent_handle && strcmp(item->file.name, name) == 0) {
            return &item->file;
        }
    }
    return NULL;
}

mtp_file_object_t *mtp_obj_get_object(mtp_obj_pool_t *pool, uint32_t handle) {
    mtp_obj_item_t *item = mtp_obj_find_item_by_handle(pool, handle);
    if (item != NULL) {
        return &item->file;
    }
    return NULL;
}

void mtp_obj_remove_object(mtp_obj_pool_t *pool, uint32_t handle) {
    mtp_obj_item_t *item = mtp_obj_find_item_by_handle(pool, handle);
    if (item != NULL) {
        item->used = 0;
    }
}

void mtp_obj_init_pool(mtp_obj_pool_t *pool) {
    memset(pool, 0, sizeof(mtp_obj_pool_t));
    pool->next_handle = 1;
}
