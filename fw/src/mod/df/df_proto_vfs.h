#ifndef DF_PROTO_VFS_H
#define DF_PROTO_VFS_H

#include "df_defines.h"

/** info proto command defines */
typedef enum {
    DF_PROTO_CMD_VFS_DRIVE_LIST = 0x10,
    DF_PROTO_CMD_VFS_DRIVE_FORMAT = 0x11,
    DF_PROTO_CMD_VFS_FILE_OPEN = 0x12,
    DF_PROTO_CMD_VFS_FILE_CLOSE = 0x13,
    DF_PROTO_CMD_VFS_FILE_READ = 0x14,
    DF_PROTO_CMD_VFS_FILE_WRITE = 0x15,
    DF_PROTO_CMD_VFS_DIR_READ = 0x16,
} df_proto_cmd_vfs_t;

extern const df_cmd_entry_t df_proto_handler_vfs_entries[];

#endif