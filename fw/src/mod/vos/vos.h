#ifndef VOS_H
#define VOS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Virtual Object Storage

#define VOS_MAX_FOLDER_SIZE 32
#define VOS_MAX_OBJECT_SIZE 64
#define VOS_MAX_NAME_LEN 24

enum vos_error_t {
    VOS_OK = 0,
    VOS_ERR_FAIL = -1,
    VOS_ERR_NOMEM = -2,
    VOS_ERR_EOF = -3,
    VOS_ERR_OBJEX = -4,
    VOS_ERR_MAXNM = -5,
    VOS_ERR_NODEV = -6,
    VOS_ERR_CRPT = -7,
    VOS_ERR_NOOBJ = -90,
    VOS_ERR_NOSPC = -91
};

typedef enum { VOS_DRIVE_INT, VOS_DRIVE_EXT, VOS_DRIVE_MAX } vos_drive_t;

typedef enum {
    VOS_BUCKET_AMIIBO, /** store amiibo data*/
    VOS_BUCKET_SYSTEM  /** store system data*/
} vos_bucket_t;

typedef struct {
    uint8_t type;
    size_t size;
    char name[VOS_MAX_NAME_LEN];
} vos_obj_t;

typedef struct {
    uint8_t avaliable;
    uint32_t total_bytes;
    uint32_t free_bytes;
} vos_stat_t;

typedef struct {
    int32_t (*mount)();
    int32_t (*umount)();
    int32_t (*format)();

    bool (*mounted)();

    int32_t (*stat)(vos_stat_t *p_stat);

    /* folder operations */
    int32_t (*list_folder)(vos_bucket_t bucket, vos_obj_t *folders, size_t folder_size);
    int32_t (*create_folder)(vos_bucket_t bucket, const char *name);
    int32_t (*remove_folder)(vos_bucket_t bucket, const char *name);

    /* obj operations */
    int32_t (*list_object)(vos_bucket_t bucket, const char *folder_name, vos_obj_t *objects, size_t object_size);
    int32_t (*write_object)(vos_bucket_t bucket, const char *folder_name, const char *object_name, void *buffer,
                            size_t buffer_size);
    int32_t (*read_object)(vos_bucket_t bucket, const char *folder_name, const char *object_name, void *buffer,
                           size_t buffer_size);
    int32_t (*remove_object)(vos_bucket_t bucket, const char *folder_name, const char *object_name);

} vos_driver_t;

bool vos_drive_enabled(vos_drive_t drive);
vos_driver_t *vos_get_driver(vos_drive_t drive);

#endif