//
// Created by solos on 8/19/2023.
//
#include "amiidb_api_slot.h"
#include "amiibo_helper.h"
#include "settings.h"
#include "vfs.h"
#include "vfs_meta.h"
#include <stdio.h>
#include <string.h>

int32_t amiidb_api_slot_read(uint8_t slot, ntag_t *p_ntag) {
    char path[VFS_MAX_PATH_LEN];
    vfs_obj_t fd;
    vfs_meta_t meta;
    sprintf(path, "/amiibo/data/%02d.bin", slot);
    vfs_driver_t *p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);
    if (p_vfs_driver->stat_file(path, &fd) != VFS_OK) {
        return -1;
    }

    memset(&meta, 0, sizeof(vfs_meta_t));
    vfs_meta_decode(fd.meta, sizeof(fd.meta), &meta);
    int32_t res = p_vfs_driver->read_file_data(path, p_ntag->data, sizeof(p_ntag->data));
    if (res < 0) {
        return -1;
    }

    if (meta.has_notes) {
        strcpy(p_ntag->notes, meta.notes);
    }

    if (meta.has_flags && meta.flags & VFS_OBJ_FLAG_READONLY) {
        p_ntag->readonly = true;
    }

    return 0;
}

int32_t amiidb_api_slot_write(uint8_t slot, ntag_t *p_ntag) {
    char path[VFS_MAX_PATH_LEN];
    vfs_meta_t meta;
    uint8_t meta_encoded[VFS_META_MAX_SIZE];

    sprintf(path, "/amiibo/data/%02d.bin", slot);
    vfs_driver_t *p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);
    int32_t res = p_vfs_driver->write_file_data(path, p_ntag->data, sizeof(p_ntag->data));
    if (res < 0) {
        // TODO msg box
        return -1;
    }

    memset(&meta, 0, sizeof(meta));
    meta.has_amiibo_id = true;
    meta.amiibo_head = to_little_endian_int32(&p_ntag->data[84]);
    meta.amiibo_tail = to_little_endian_int32(&p_ntag->data[88]);

    // TODO write only flag???

    vfs_meta_encode(meta_encoded, sizeof(meta_encoded), &meta);
    res = p_vfs_driver->update_file_meta(path, meta_encoded, sizeof(meta_encoded));
    if (res < 0) {
        return -1;
    }

    return 0;
}

int32_t amiidb_api_slot_remove(uint8_t slot) {
    char path[VFS_MAX_PATH_LEN];
    sprintf(path, "/amiibo/data/%02d.bin", slot);
    vfs_driver_t *p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);
    int32_t res = p_vfs_driver->remove_file(path);
    if (res < 0) {
        return -1;
    }
    return 0;
}

int32_t amiidb_api_slot_info(uint8_t slot, amiidb_slot_info_t *p_info) {
    char path[VFS_MAX_PATH_LEN];
    vfs_meta_t meta;
    vfs_obj_t obj;

    sprintf(path, "/amiibo/data/%02d.bin", slot);
    vfs_driver_t *p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);
    int32_t res = p_vfs_driver->stat_file(path, &obj);
    if (res < 0) {
        p_info->slot = slot;
        p_info->is_empty = true;
        return 0;
    }

    memset(&meta, 0, sizeof(vfs_meta_t));
    vfs_meta_decode(obj.meta, sizeof(obj.meta), &meta);

    p_info->slot = slot;
    p_info->is_empty = false;
    p_info->is_readonly = meta.has_flags && meta.flags & VFS_OBJ_FLAG_READONLY;
    p_info->amiibo_head = meta.amiibo_head;
    p_info->amiibo_tail = meta.amiibo_tail;

    return 0;
}

int32_t amiidb_api_slot_list(amiibo_slot_info_cb_t cb, void *ctx) {
    vfs_dir_t dir;
    vfs_obj_t obj;
    vfs_meta_t meta;

    uint8_t max_slot_num = settings_get_data()->amiidb_data_slot_num;

    amiidb_slot_info_t slots[MAX_SLOT_COUNT];
    for (uint8_t i = 0; i < MAX_SLOT_COUNT; i++) {
        slots[i].slot = i;
        slots[i].is_empty = true;
        slots[i].is_readonly = false;
        slots[i].amiibo_head = 0;
        slots[i].amiibo_tail = 0;
    }

    vfs_driver_t *p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);
    int32_t res = p_vfs_driver->open_dir("/amiibo/data", &dir);
    if (res < 0) {
        return -1;
    }
    while (p_vfs_driver->read_dir(&dir, &obj) == VFS_OK) {
        if (obj.type == VFS_TYPE_REG) {
            int32_t index = 0;
            if (sscanf(obj.name, "%02d.bin", &index) == 1 && index >= 0 && index < max_slot_num) {
                memset(&meta, 0, sizeof(vfs_meta_t));
                vfs_meta_decode(obj.meta, sizeof(obj.meta), &meta);
                if (meta.has_amiibo_id) {
                    slots[index].is_empty = false;
                    slots[index].amiibo_head = meta.amiibo_head;
                    slots[index].amiibo_tail = meta.amiibo_tail;
                }
                if (meta.has_flags && meta.flags & VFS_OBJ_FLAG_READONLY) {
                    slots[index].is_readonly = true;
                }
            }
        }
    }
    p_vfs_driver->close_dir(&dir);

    for (uint8_t i = 0; i < max_slot_num; i++) {
        cb(&slots[i], ctx);
    }

    return 0;
}

int32_t amiidb_api_slot_set_readonly(uint8_t slot, bool readonly) {
    char path[VFS_MAX_PATH_LEN];
    vfs_meta_t meta;
    vfs_obj_t obj;

    sprintf(path, "/amiibo/data/%02d.bin", slot);
    vfs_driver_t *p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);
    int32_t res = p_vfs_driver->stat_file(path, &obj);
    if (res < 0) {
        return -1;
    }

    memset(&meta, 0, sizeof(vfs_meta_t));
    vfs_meta_decode(obj.meta, sizeof(obj.meta), &meta);

    meta.has_flags = true;
    if (readonly) {
        meta.flags |= VFS_OBJ_FLAG_READONLY;
    } else {
        meta.flags &= ~VFS_OBJ_FLAG_READONLY;
    }

    return p_vfs_driver->update_file_meta(path, obj.meta, sizeof(obj.meta));
}