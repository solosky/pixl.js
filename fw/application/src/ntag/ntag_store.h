/*
 * ntag_store.h
 *
 *  Created on: 2021年9月1日
 *      Author: solos
 */

#ifndef NTAG_STORE_H_
#define NTAG_STORE_H_

#include "compiler_abstraction.h"
#include "ntag_def.h"
#include "sdk_errors.h"
#include <stdbool.h>
#include <stdint.h>

// 填充默认ntag数据并随机uuid
void ntag_store_new_rand(ntag_t *ntag);
// 随机uuid数据(不修改其他)
ret_code_t ntag_store_uuid_rand(ntag_t *ntag);
void ntag_store_set_uuid(ntag_t *ntag, uint8_t *uuid);
void ntag_store_get_uuid(ntag_t *ntag, uint8_t *uuid);

ret_code_t ntag_store_read(uint8_t idx, ntag_t *ntag);
ret_code_t ntag_store_write(uint8_t idx, ntag_t *ntag);
ret_code_t ntag_store_reset(uint8_t idx, ntag_t *ntag);

#endif /* NTAG_STORE_H_ */
