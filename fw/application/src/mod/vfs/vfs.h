#ifndef VFS_H
#define VFS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "boards_defines.h"

// Virtual Object Storage

#define VFS_MAX_FOLDER_SIZE 32
#define VFS_MAX_OBJECT_SIZE 64
#define VFS_MAX_NAME_LEN 48
#define VFS_MAX_PATH_LEN 64
#define VFS_MAX_META_LEN 128

#define VFS_DRIVE_LABEL_LEN 2
#define VFS_MAX_FULL_PATH_LEN (VFS_MAX_PATH_LEN + VFS_DRIVE_LABEL_LEN)

enum vfs_error_t {
    VFS_OK = 0,
    VFS_ERR_FAIL = -1,
    VFS_ERR_NOMEM = -2,
    VFS_ERR_EOF = -3,
    VFS_ERR_OBJEX = -4,
    VFS_ERR_MAXNM = -5,
    VFS_ERR_NODEV = -6,
    VFS_ERR_CRPT = -7,
    VFS_ERR_NOOBJ = -90,
    VFS_ERR_NOSPC = -91,
    VFS_ERR_UNSUPT = -99
};

enum vfs_mode_t {
    VFS_MODE_APPEND = 1 << 0,
    VFS_MODE_TRUNC = 1 << 1,
    VFS_MODE_CREATE = 1 << 2,
    VFS_MODE_READONLY = 1 << 3,
    VFS_MODE_WRITEONLY = 1 << 4,
};

typedef enum { VFS_DRIVE_INT, VFS_DRIVE_EXT, VFS_DRIVE_MAX } vfs_drive_t;

typedef enum { VFS_TYPE_REG, VFS_TYPE_DIR } vfs_type_t;

typedef struct {
    uint8_t type;
    size_t size;
    char name[VFS_MAX_NAME_LEN];
    char meta[VFS_MAX_META_LEN];
} vfs_obj_t;

typedef struct {
    void *handle;
} vfs_file_t;

typedef struct {
    void *handle;
    char path[VFS_MAX_FULL_PATH_LEN];
} vfs_dir_t;

typedef struct {
    uint8_t avaliable;
    uint32_t total_bytes;
    uint32_t free_bytes;
} vfs_stat_t;

typedef struct {
    int32_t (*mount)();
    int32_t (*umount)();
    int32_t (*format)();

    bool (*mounted)();

    int32_t (*stat)(vfs_stat_t *p_stat);

    int32_t (*stat_file)(const char *file, vfs_obj_t *obj);

    /**directory operations*/
    int32_t (*open_dir)(const char *dir, vfs_dir_t *fd);
    int32_t (*read_dir)(vfs_dir_t *fd, vfs_obj_t *obj);
    int32_t (*close_dir)(vfs_dir_t *fd);

    int32_t (*create_dir)(const char *dir);
    int32_t (*remove_dir)(const char *dir);
    int32_t (*rename_dir)(const char *dir, const char *new_dir);

    /**file operations*/
    int32_t (*open_file)(const char *file, vfs_file_t *fd, uint32_t flags);
    int32_t (*close_file)(vfs_file_t *fd);
    int32_t (*read_file)(vfs_file_t *fd, void *buff, size_t buff_size);
    int32_t (*write_file)(vfs_file_t *fd, void *buff, size_t buff_size);
    int32_t (*update_file_meta)(const char* file, void* meta, size_t meta_size);


    /**short opearation*/
    int32_t (*write_file_data)(const char *file, void *buff, size_t buff_size);
    int32_t (*read_file_data)(const char *file, void *buff, size_t buff_size);

    int32_t (*rename_file)(const char *file, const char *new_file);
    int32_t (*remove_file)(const char *file);

} vfs_driver_t;

bool vfs_drive_enabled(vfs_drive_t drive);
vfs_driver_t *vfs_get_driver(vfs_drive_t drive);
vfs_driver_t* vfs_get_default_driver();
vfs_drive_t vfs_get_default_drive();

#endif