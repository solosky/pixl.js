/*
 * amiibo_data.c
 *
 *  Created on: 2021年9月13日
 *      Author: solos
 */

#include "amiibo_data.h"
#include <stddef.h>
#include "nrf_log.h"


extern const amiibo_data_t amiibo_data[];

const amiibo_data_t* find_amiibo_data(uint32_t head, uint32_t tail){
	amiibo_data_t* pos = (amiibo_data_t*) amiibo_data;
	while(pos->name != NULL){
		if(pos->head == head && pos->tail == tail){
			return pos;
		}
		pos += 1;

	}
	return NULL;
}

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


