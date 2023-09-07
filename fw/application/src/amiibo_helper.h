//
// Created by solos on 2021/11/7.
//

#ifndef AMIIBO_HELPER_H
#define AMIIBO_HELPER_H

#include "app_error.h"
#include "ntag_def.h"

ret_code_t amiibo_helper_load_keys(const uint8_t* data);
bool amiibo_helper_is_key_loaded();
void amiibo_helper_try_load_amiibo_keys_from_vfs();
//随机uuid并重新签名amiibo
ret_code_t amiibo_helper_rand_amiibo_uuid(ntag_t* ntag);
//根据head和tail生成ntag数据
ret_code_t amiibo_helper_generate_amiibo(uint32_t head, uint32_t tail, ntag_t* ntag);

bool is_valid_amiibo_ntag(const ntag_t* ntag);

uint32_t to_little_endian_int32(const uint8_t* data);


#endif //AMIIBO_HELPER_H
