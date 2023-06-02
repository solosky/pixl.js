/*
 * ntag_def.h
 *
 *  Created on: 2021年9月1日
 *      Author: solos
 */

#ifndef NTAG_DEF_H_
#define NTAG_DEF_H_

#include <stdint.h>
#include <stdbool.h>

#define NTAG_DATA_SIZE 540

typedef enum
{
	NTAG_213,
	NTAG_215,
	NTAG_216
} ntag_type_t;

typedef struct  {
	uint8_t data[NTAG_DATA_SIZE];
	uint8_t notes[128];
} ntag_t;


#endif /* NTAG_DEF_H_ */
