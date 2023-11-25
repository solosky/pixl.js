#ifndef FDS_UTILS_H
#define FDS_UTILS_H

#include "vfs_meta.h"

#define CHELEMEON_DUMP_FOLDER "/chameleon/dump"

bool fds_config_file_exists();
int32_t fds_read_meta(uint16_t id, uint16_t key, vfs_meta_t *meta);
int32_t fds_write_meta(uint16_t id, uint16_t key, vfs_meta_t *meta);
#endif 