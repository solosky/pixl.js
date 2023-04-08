/**
 * Copyright (C) 2019 Bosch Sensortec GmbH
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file    mtp.c
 * @date    Jun 22, 2019
 * @author  Jabez Winston Christopher <christopher.jabezwinston@in.bosch.com>
 * @brief   The minimal USB MTP stack (Doesn't implement the complete specification !)
 */

/**********************************************************************************/
/* header includes */
/**********************************************************************************/

#include "nrf_log.h"
#include "mtp_buffer.h"
#include "mtp_fs_lfs.h"
#include "mtp_core.h"
#include "mtp_proto.h"


static void mtp_core_process_command(mtp_core_t *mtp, mtp_codec_event_t *event) {
    mtp_cmd_t *cmd = mtp_proto_find_cmd_by_code(event->transaction->op_code);
    if (cmd == NULL) {
        NRF_LOG_ERROR("unknown cmd: %04X", event->transaction->op_code);
        mtp_container_header_t header;
        header.type = MTP_CONTAINER_TYPE_RESPONSE;
        header.length = MTP_CONTAINER_HEADER_SIZE;
        header.code = MTP_RESPONSE_UNDEFINED;
        header.transaction_id = event->transaction->id;
        mtp_codec_send_response(event->codec, &header, NULL);
        return;
    }

    //call event header
    mtp->cmd = cmd;
    cmd->handler(mtp, event);
}

static void mtp_core_process_data(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (mtp->cmd != NULL) {
        mtp->cmd->handler(mtp, event);
    }
}


static void mtp_core_codec_event_handler(mtp_codec_event_t *event) {
    mtp_core_t *mtp = (mtp_core_t *) event->context;
    switch (event->type) {
        case MTP_CODEC_EVENT_COMMAND_RX_DONE:
            mtp_core_process_command(mtp, event);
            break;
        case MTP_CODEC_EVENT_DATA_CHUNK_TX_READY:
        case MTP_CODEC_EVENT_DATA_HEADER_RX_DONE:
        case MTP_CODEC_EVENT_DATA_ALL_RX_DONE:
        case MTP_CODEC_EVENT_DATA_ALL_TX_DONE:
        case MTP_CODEC_EVENT_DATA_CHUNK_RX_DONE:
            mtp_core_process_data(mtp, event);
            break;
        case MTP_CODEC_EVENT_RESPONSE_RX_DONE:
            mtp->cmd = NULL;
            break;
        case MTP_CODEC_EVENT_ERROR:
            mtp_codec_reset(event->codec);
            break;
        case MTP_CODEC_EVENT_RESET:
            break;
        case MTP_CODEC_EVENT_CANCEL:
            break;
    }
}

/*!
 *  @brief init mtp core
 */
void mtp_core_init(mtp_core_t *mtp, mtp_init_t *init) {
    mtp->fs_driver = lfs_glue_driver();
    mtp->codec.comm_ep_in = init->comm_ep_in;
    mtp->codec.data_ep_out = init->data_ep_out;
    mtp->codec.data_ep_in = init->data_ep_in;
    mtp->codec.evt_handler = mtp_core_codec_event_handler;
    mtp->codec.evt_context = mtp;
    mtp->cmd = NULL;

    mtp_codec_init(&mtp->codec);
}


