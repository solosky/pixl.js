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

static void amiidb_api_fav_format_folder(char *path, const char *folder) { sprintf(path, "/amiibo/fav/%s", folder); }

int32_t amiidb_api_fav_list_dir(const char *folder, amiidb_fav_cb_t cb, void* ctx) {
    char path[VFS_MAX_PATH_LEN];
    vfs_driver_t *p_vfs_driver;
    vfs_dir_t dir;
    vfs_obj_t obj;
    amiidb_fav_info_t info;

    strcpy(path, "/amiibo/fav");
    if (strlen(folder) > 0) {
        strcat(path, "/");
        strcat(path, folder);
    }
    p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);
    int32_t res = p_vfs_driver->open_dir(path, &dir);
    if (res < 0) {
        return -1;
    }
    while (p_vfs_driver->read_dir(&dir, &obj) == VFS_OK) {
        if (obj.type == VFS_TYPE_DIR) {
            info.fav_type = 0;
            strcpy(info.fav_data.folder_name, obj.name);
            cb(&info, ctx);
        } else {
            uint8_t game_id;
            uint32_t head, tail;

            if (sscanf(obj.name, "%d_%08X_%08X.fav", &game_id, &head, &tail) == 3) {
                const db_amiibo_t *p_amiibo = get_amiibo_by_id(head, tail);

                info.fav_type = 1;
                info.fav_data.fav.amiibo_tail = tail;
                info.fav_data.fav.amiibo_head = head;
                info.fav_data.fav.game_id = game_id;
                cb(&info, ctx);
            }
        }
    }
    p_vfs_driver->close_dir(&dir);
    return 0;
}

int32_t amiidb_api_fav_remove_dir(const char *folder) {
    char path[VFS_MAX_PATH_LEN];
    vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
    amiidb_api_fav_format_folder(path, folder);
    if (p_driver->remove_dir(path) < 0) {
        return -1;
    }
    return 0;
}
int32_t amiidb_api_fav_empty_dir(const char *folder) {
    char path[VFS_MAX_PATH_LEN];
    char sub_path[VFS_MAX_PATH_LEN];
    vfs_dir_t dir;
    vfs_obj_t obj;

    vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
    sprintf(path, "/amiibo/fav/%s", folder);
    int32_t res = p_driver->open_dir(path, &dir);
    if (res < 0) {
        return -1;
    }

    while (p_driver->read_dir(&dir, &obj) == VFS_OK) {
        sprintf(sub_path, "%s/%s", path, obj.name);
        if (obj.type == VFS_TYPE_DIR) {
            p_driver->remove_dir(sub_path);
        } else {
            p_driver->remove_file(sub_path);
        }
    }
    p_driver->close_dir(&dir);
    return 0;
}

int32_t amiidb_api_fav_create_dir(const char *folder){
    char path[VFS_MAX_PATH_LEN];
    sprintf(path, "/amiibo/fav/%s", folder);
    vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
    int32_t res = p_driver->create_dir(path);
    if(res < 0){
        return -1;
    }
    return 0;
}

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