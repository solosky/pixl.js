#ifndef CACHE_H_
#define CACHE_H_

#include "ntag_def.h"
#include "vfs.h"

#define CACHEDATASIZE 312
#define CACHE_MAGIC 0x564D5649

// Lives in the .noinit region (NOINIT in application.ld, 0x20002bd0, 0xA30
// bytes) and survives System OFF only because cache_save() retains RAM1
// (0x20002000-0x20003FFF). If this struct grows, keep the whole region inside
// RAM1, or extend the retention in cache_save(); otherwise its tail is lost
// in sleep and Fast Wakeup silently stops working.
typedef struct {
    int32_t enabled;
    uint32_t id;
    ntag_t ntag;
    uint8_t retain_data[CACHEDATASIZE];
} cache_data_t;

int32_t cache_clean();
int32_t cache_save();
cache_data_t *cache_get_data();
bool cache_valid();

#endif /* CACHE_H_ */