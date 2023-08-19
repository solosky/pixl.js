//
// Created by solos on 8/19/2023.
//
#include "amiidb_api_fav.h"
#include "amiibo_helper.h"
#include "vfs.h"
#include "vfs_meta.h"
#include <stdio.h>
#include <string.h>

static void amiidb_api_fav_format_filename(char *path, const char *folder, const amiidb_fav_t *p_fav) {
    sprintf(path, "/amiibo/fav/%s/%d_%08X_%08X.fav", folder, p_fav->game_id, p_fav->amiibo_head, p_fav->amiibo_tail);
}

int32_t amiidb_api_fav_open_dir(const char *folder, amiidb_fav_dir_t *dir);
int32_t amiidb_api_fav_read_dir(amiidb_fav_dir_t *dir, const amiidb_fav_t *fav);
int32_t amiidb_api_fav_close_dir(amiidb_fav_dir_t *dir);
int32_t amiidb_api_fav_remove_dir(const char *folder);
int32_t amiidb_api_fav_empty_dir(const char *folder);


int32_t amiidb_api_fav_add(const char *folder, const amiidb_fav_t *p_fav) {
    char path[VFS_MAX_PATH_LEN];
    vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
    amiidb_api_fav_format_filename(path, folder, p_fav);
    if (p_driver->write_file_data(path, NULL, 0) < 0) {
        return -1;
    }

    return 0;
}

int32_t amiidb_api_fav_remove(const char *folder, const amiidb_fav_t *p_fav) {
    char path[VFS_MAX_PATH_LEN];
    vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
    amiidb_api_fav_format_filename(path, folder, p_fav);
    if (p_driver->remove_file(path) < 0) {
        return -1;
    }
    return 0;
}
int32_t amiidb_api_fav_remove_dir(const char *folder);