#ifndef FDS_UTILS_H
#define FDS_UTILS_H

#include "vfs_meta.h"

bool fds_config_file_exists();
bool fds_read_meta(uint16_t id, uint16_t key, vfs_meta_t *meta);

#endif 