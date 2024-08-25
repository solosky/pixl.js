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

#define NTAG_DATA_SIZE 540
#define NTAG_TAGMO_DATA_SIZE 532
#define NTAG_THENAYA_DATA_SIZE 572

typedef enum { NTAG_213, NTAG_215, NTAG_216 } ntag_type_t;

typedef struct {
    uint8_t data[NTAG_DATA_SIZE];
    uint8_t notes[128];
    bool read_only;
} ntag_t;

#endif /* NTAG_DEF_H_ */
