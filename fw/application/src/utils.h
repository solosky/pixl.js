/*
 * utils.h
 *
 *  Created on: 2021年10月10日
 *      Author: solos
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include "sdk_errors.h"

ret_code_t utils_rand_bytes(uint8_t rand[], uint8_t bytes);
void int32_to_bytes_le(uint32_t val, uint8_t* data) ;
void utils_get_device_id(uint8_t* p_device_id);
void enter_dfu();
void reboot_system();




#endif /* UTILS_H_ */
