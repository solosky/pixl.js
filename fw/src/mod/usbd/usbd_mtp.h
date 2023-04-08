//
// Created by solos on 2021/11/23.
//

#ifndef FW_USBD_MTP_H
#define FW_USBD_MTP_H

#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_string_desc.h"

void usbd_mtp_init(void);
app_usbd_class_inst_t const * usbd_mtp_class_inst_get();
#endif //FW_USBD_MTP_H
