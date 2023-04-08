//
// Created by solos on 2022/1/2.
//

#ifndef FW_MTP_PROTO_H
#define FW_MTP_PROTO_H

#include "mtp_core.h"
#include "mtp_codec.h"

mtp_cmd_t *mtp_proto_find_cmd_by_code(uint16_t code);

#endif //FW_MTP_PROTO_H
