//
// Created by solos on 2021/11/7.
//

#ifndef AMIIBO_HELPER_H
#define AMIIBO_HELPER_H

#include "app_error.h"
#include "ntag_def.h"

ret_code_t amiibo_helper_sign_new_ntag(ntag_t* old_ntag, ntag_t* new_ntag);
ret_code_t amiibo_helper_load_keys(const uint8_t* data);
bool amiibo_helper_is_key_loaded();

ret_code_t amiibo_helper_ntag_generate(ntag_t* ntag);
void amiibo_helper_try_load_amiibo_keys_from_vfs();


#endif //AMIIBO_HELPER_H
