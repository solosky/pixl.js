//
// Created by solos on 2021/11/7.
//

#ifndef AMIIBO_HELPER_H
#define AMIIBO_HELPER_H

#include "app_error.h"
#include "ntag_def.h"

ret_code_t amiibo_helper_sign_new_ntag(ntag_t* old_ntag, ntag_t* new_ntag);
ret_code_t amiibo_helper_load_keys(const uint8_t* data);


#endif //AMIIBO_HELPER_H
