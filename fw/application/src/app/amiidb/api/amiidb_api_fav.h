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
    vfs_dir_t dir;
    vfs_obj_t fd;
    vfs_meta_t meta;
} amiidb_fav_dir_t;

int32_t amiidb_api_fav_open_dir(const char *folder, amiidb_fav_dir_t *dir);
int32_t amiidb_api_fav_read_dir(amiidb_fav_dir_t *dir, const db_link_t *link);
int32_t amiidb_api_fav_close_dir(amiidb_fav_dir_t *dir);
int32_t amiidb_api_fav_remove_dir(const char *folder);
int32_t amiidb_api_fav_empty_dir(const char *folder);
int32_t amiidb_api_fav_add(const char *folder, const db_link_t *link);
int32_t amiidb_api_fav_remove(const char *folder, const db_link_t *link);
int32_t amiidb_api_fav_remove_dir(const char *folder);

#endif // FW_AMIIDB_API_SLOT_H
