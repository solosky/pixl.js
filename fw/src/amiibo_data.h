/*
 * amiibo_data.h
 *
 *  Created on: 2021年9月13日
 *      Author: solos
 */

#ifndef AMIIBO_DATA_H_
#define AMIIBO_DATA_H_

#include <stdint.h>
#include <stdbool.h>


typedef struct  {
	uint32_t head;
	uint32_t tail;
	const char* amiibo_series;
	const char* game_series;
	const char* character;
	const char* name;
	const char* notes;
} amiibo_data_t;


const amiibo_data_t* find_amiibo_data(uint32_t head, uint32_t tail);
uint32_t to_little_endian_int32(uint8_t* data);


#endif /* AMIIBO_DATA_H_ */
