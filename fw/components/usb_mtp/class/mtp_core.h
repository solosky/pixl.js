//
// Created by solos on 2022/1/2.
//

#ifndef FW_MTP_CORE_H
#define FW_MTP_CORE_H

#include "mtp_codec.h"
#include "mtp_fs.h"


struct mtp_core;

typedef void (*mtp_op_handler_t)(struct mtp_core *mtp, mtp_codec_event_t *event);

typedef struct {
    uint32_t op;
    mtp_op_handler_t handler;
} mtp_cmd_t;

typedef struct mtp_core {
    mtp_codec_t codec;
    mtp_fs_driver_t *fs_driver;
    mtp_cmd_t *cmd;
    uint32_t file_handle;
} mtp_core_t;

typedef struct {
    uint8_t comm_ep_in;          //!< COMM subclass IN endpoint.
    uint8_t data_ep_out;         //!< DATA subclass OUT endpoint.
    uint8_t data_ep_in;          //!< DATA subclass IN endpoint.
} mtp_init_t;

void mtp_core_init(mtp_core_t *mtp, mtp_init_t *init);

#endif //FW_MTP_CORE_H
