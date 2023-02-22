#ifndef VFS_META_H
#define VFS_META_H

#include <stdint.h>
#include <stdbool.h>


#define VFS_META_MAX_NOTES_SIZE 90
#define VFS_META_MAX_SIZE VFS_MAX_META_LEN - 1

typedef enum {
    VFS_OBJ_FLAG_HIDDEN = 1 << 0,
    VFS_OBJ_FLAG_SYSTEM = 1 << 1,
} vfs_obj_flags_t;

typedef enum {
    VFS_META_TYPE_NOTES = 1,
    VFS_META_TYPE_FLAGS = 2
} vfs_meta_type_t;


typedef struct {
    bool has_notes;
    char notes[VFS_META_MAX_NOTES_SIZE];
    bool has_flags;
    uint8_t flags;
} vfs_meta_t;


void vfs_meta_decode(uint8_t* p_meta, uint8_t size, vfs_meta_t* p_out);
void vfs_meta_encode(uint8_t* p_meta, uint8_t size, vfs_meta_t* p_in);


#endif