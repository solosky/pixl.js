#ifndef VFS_H
#define VFS_H

#include <stddef.h>
#include <stdint.h>
// Virtual File System
#define VFS_MAX_NAME 64

typedef struct {
    uint8_t type;
    size_t size;
    char name[VFS_MAX_NAME];
} vfs_obj_t;

typedef struct {
    void *handle;
} vfs_scan_t;

typedef struct {
    uint8_t avaliable;
    uint32_t total_bytes;
    uint32_t free_bytes;
} vfs_stat_t;

typedef struct {
    void* handle;
} vfs_handle_t;

typedef struct {
    void (*vfs_init)();
    void (*vfs_deinit)();

    int32_t (*vfs_stat)(vfs_stat_t *p_stat);

    int32_t (*list_dir)(const char *dir, vfs_scan_t *p_scan, vfs_obj_t *p_obj);
    int32_t (*stat_dir)(const char *dir);
    int32_t (*create_dir)(const char *dir);
    int32_t (*remove_dir)(const char *dir);

    int32_t (*list_file)(const char *dir, vfs_scan_t *p_scan, vfs_obj_t *p_obj);
    int32_t (*stat_file)(const char *path, vfs_obj_t *p_obj);
    int32_t (*remove_file)(const char *path);

    int32_t (*open_file)(const char *path, vfs_handle_t *p_handle, uint32_t flags);
    int32_t (*close_file)(vfs_handle_t *p_handle);
    int32_t (*read_file)(vfs_handle_t *p_handle, uint8_t *buffer, size_t buffer_size);
    int32_t (*write_file)(vfs_handle_t *p_handle, uint8_t *buffer, size_t buffer_size);

} vfs_driver_t;

#endif