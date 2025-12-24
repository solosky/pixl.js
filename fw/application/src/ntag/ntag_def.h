/*
 * ntag_def.h
 *
 *  Created on: 2021年9月1日
 *      Author: solos
 */

#ifndef NTAG_DEF_H_
#define NTAG_DEF_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define NTAG_DATA_SIZE 540
#define NTAG_I2C_2K_DATA_SIZE 2048
#define NTAG_TAGMO_DATA_SIZE 532
#define NTAG_THENAYA_DATA_SIZE 572

typedef enum { NTAG_215, NTAG_I2C_PLUS_2K } ntag_type_t;

typedef struct {
    // picking largest size for internal buffer
    // internal struct for storing i2c 2k data is contiguous wrt nfc pages, including storing sram pages
    uint8_t data[NTAG_I2C_2K_DATA_SIZE];
    
    uint8_t notes[128];
    bool read_only;
    ntag_type_t type;
} ntag_t;

static inline size_t _ntag_data_size(ntag_t* tag) {
    if (tag->type == NTAG_215) {
        return NTAG_DATA_SIZE;
    } else {
        return NTAG_I2C_2K_DATA_SIZE;
    }
}

static inline bool is_valid_amiibo_ntag_by_size(size_t size) {
    return size == NTAG_DATA_SIZE || size == NTAG_I2C_2K_DATA_SIZE || size == NTAG_TAGMO_DATA_SIZE || size == NTAG_THENAYA_DATA_SIZE;
}

static inline ntag_type_t _ntag_type(size_t size) {
    if (size == NTAG_DATA_SIZE || size == NTAG_TAGMO_DATA_SIZE || size == NTAG_THENAYA_DATA_SIZE) {
        return NTAG_215;
    } else {
        return NTAG_I2C_PLUS_2K;
    }
}

#endif /* NTAG_DEF_H_ */
