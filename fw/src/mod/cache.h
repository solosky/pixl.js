#ifndef CACHE_H_
#define CACHE_H_

#include "m-string.h"

#include "ntag_def.h"
#include "vfs.h"

typedef struct {
    bool enabled;
    vfs_drive_t current_drive;
    char current_folder[128];
    char current_file[64];
    ntag_t tag;
} cache_data_t;

int32_t cache_init();
int32_t cache_clean();
int32_t cache_save();
cache_data_t *cache_get_data();

#endif /* CACHE_H_ */