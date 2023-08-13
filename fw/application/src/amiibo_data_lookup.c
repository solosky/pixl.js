/*
 * amiibo_data.c
 *
 *  Created on: 2021年9月13日
 *      Author: solos
 */

#include "amiibo_data.h"
#include <stddef.h>
#include "nrf_log.h"

uint32_t to_little_endian_int32(uint8_t* data){
	uint32_t val = 0;
	val += data[0];
	val <<= 8;
	val += data[1];
	val <<= 8;
	val += data[2];
	val <<=8;
	val += data[3];
	return val;
}


