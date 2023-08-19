//
// Created by solos on 8/19/2023.
//
#include "amiidb_api_slot.h"
#include "amiibo_helper.h"
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
    if(res < 0){
        return -1;
    }

    if(meta.has_notes) {
        strcpy(p_ntag->notes, meta.notes);
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

    vfs_meta_encode(meta_encoded, sizeof(meta_encoded), &meta);
    res = p_vfs_driver->update_file_meta(path, meta_encoded, sizeof(meta_encoded));
    if (res < 0) {
        return -1;
    }

    return 0;
}