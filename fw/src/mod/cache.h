#ifndef CACHE_H_
#define CACHE_H_

#include "m-string.h"

#include "ntag_def.h"
#include "vfs.h"

#define CACHEDATASIZE 316

typedef struct {
    int32_t enabled;
    uint32_t id;
    ntag_t ntag;
    uint8_t retain_data[CACHEDATASIZE];
} cache_data_t;

bool cache_empty(uint8_t *data);

int32_t cache_init();
int32_t cache_clean();
int32_t cache_save();
cache_data_t *cache_get_data();
uint8_t *get_empty_data();

#endif /* CACHE_H_ */