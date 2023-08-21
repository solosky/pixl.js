//
// Created by solos on 8/19/2023.
//

#ifndef FW_AMIIDB_API_FAV_H
#define FW_AMIIDB_API_FAV_H

#include "db_header.h"
#include "ntag_def.h"
#include "vfs.h"
#include "vfs_meta.h"
#include <stdint.h>

typedef struct {
    uint8_t game_id;
    uint32_t amiibo_head;
    uint32_t amiibo_tail;
} amiidb_fav_t;

#define FAV_TYPE_FOLDER 0
#define FAV_TYPE_FAV 1

typedef struct {
    uint8_t fav_type; // 0: dir, 1: fav
    union {
        char folder_name[VFS_MAX_NAME_LEN];
        amiidb_fav_t fav;
    } fav_data;
} amiidb_fav_info_t;

typedef void (*amiidb_fav_cb_t)(amiidb_fav_info_t* fav_info, void* ctx);

int32_t amiidb_api_fav_list_dir(const char *folder, amiidb_fav_cb_t cb, void* ctx);
int32_t amiidb_api_fav_remove_dir(const char *folder);
int32_t amiidb_api_fav_empty_dir(const char *folder);
int32_t amiidb_api_fav_create_dir(const char *folder);
int32_t amiidb_api_fav_add(const char *folder, const amiidb_fav_t *fav);
int32_t amiidb_api_fav_remove(const char *folder, const amiidb_fav_t *fav);
int32_t amiidb_api_fav_remove_dir(const char *folder);

#endif // FW_AMIIDB_API_SLOT_H
