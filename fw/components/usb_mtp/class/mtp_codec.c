//
// Created by solos on 2022/1/2.
//

#include <assert.h>
#include "mtp_config.h"
#include "mtp_types.h"
#include "mtp_codec.h"
#include "mtp_buffer.h"
#include "app_usbd_mtp.h"
#include "nrf_log.h"

static void mtp_codec_usbd_ep_in_transfer(mtp_codec_t *codec);

///////////////////////////////////////////////////////////////////////////////////
static void mtp_codec_decode_container_header(mtp_buffer_t *buffer, mtp_container_header_t *header) {
    header->length = mtp_buff_get_u32(buffer);
    header->type = mtp_buff_get_u16(buffer);
    header->code = mtp_buff_get_u16(buffer);
    header->transaction_id = mtp_buff_get_u32(buffer);
}

static void mtp_codec_decode_container_parameter(mtp_buffer_t *buffer, mtp_container_header_t *header,
                                                 mtp_container_parameter_t *parameter) {
    parameter->count = (header->length - MTP_CONTAINER_HEADER_SIZE) / 4;
    for (uint32_t i = 0; i < parameter->count; i++) {
        parameter->parameter[i] = mtp_buff_get_u32(buffer);
    }
}

static void mtp_codec_encode_container_header(mtp_buffer_t *buffer, mtp_container_header_t *header) {
    mtp_buff_put_u32(buffer, header->length);
    mtp_buff_put_u16(buffer, header->type);
    mtp_buff_put_u16(buffer, header->code);
    mtp_buff_put_u32(buffer, header->transaction_id);
}

static void mtp_codec_encode_container_payload(mtp_buffer_t *buffer, void *payload, size_t payload_size) {
    mtp_buff_put_byte_array(buffer, payload, payload_size);
}

static void mtp_codec_encode_container_parameter(mtp_buffer_t *buffer, mtp_container_parameter_t *parameter) {
    for (uint32_t i = 0; i < parameter->count; i++) {
        mtp_buff_put_u32(buffer, parameter->parameter[i]);
    }
}

static inline void mtp_codec_call_event_handler(mtp_codec_t *codec, mtp_codec_event_t *event) {
    event->codec = codec;
    event->context = codec->evt_context;
    event->transaction = &codec->transaction;
    codec->evt_handler(event);
}

static inline bool mtp_codec_is_internal_tx_buffer(mtp_codec_t *codec, mtp_buffer_t *buffer) {
    return (codec->tx_buffer_data + MTP_CONTAINER_HEADER_SIZE) == mtp_buff_get_data(buffer);
}

static void mtp_codec_reset_status(mtp_codec_t *codec) {
    codec->state = IDLE;
    codec->rx_buffer.pos = 0;
    codec->rx_buffer.limit = 0;
    codec->tx_buffer.pos = 0;
    codec->tx_buffer.limit = 0;

    codec->transaction.id = 0;
    codec->transaction.state = MTP_TRX_NONE;
    codec->transaction.data_stream.header_received = false;
    codec->transaction.data_stream.total_bytes = 0;
    codec->transaction.data_stream.transfer_bytes = 0;
    codec->transaction.data_stream.direction = NONE;
    codec->transaction.data_stream.stream_mode = STREAM_MODE_BUFFER;
}


void mtp_codec_init(mtp_codec_t *codec) {
    codec->state = IDLE;
    codec->rx_buffer.buff = codec->rx_buffer_data;
    codec->rx_buffer.pos = 0;
    codec->rx_buffer.limit = 0;
    codec->rx_buffer.cap = sizeof(codec->rx_buffer_data);

    codec->tx_buffer.buff = codec->tx_buffer_data;
    codec->tx_buffer.pos = 0;
    codec->tx_buffer.limit = 0;
    codec->tx_buffer.cap = sizeof(codec->tx_buffer_data);

    codec->transaction.id = 0;
    codec->transaction.state = MTP_TRX_NONE;
    codec->transaction.data_stream.header_received = false;
    codec->transaction.data_stream.total_bytes = 0;
    codec->transaction.data_stream.transfer_bytes = 0;
    codec->transaction.data_stream.direction = NONE;
    codec->transaction.data_stream.stream_mode = STREAM_MODE_BUFFER;

}

void mtp_codec_reset(mtp_codec_t *codec) {
    mtp_codec_event_t event = {
            .type = MTP_CODEC_EVENT_RESET,
    };
    mtp_codec_call_event_handler(codec, &event);
    mtp_codec_reset_status(codec);
}

void mtp_codec_cancel(mtp_codec_t *codec, uint32_t transaction_id) {
    mtp_codec_event_t event = {
            .type = MTP_CODEC_EVENT_CANCEL,
    };
    mtp_codec_call_event_handler(codec, &event);
    mtp_codec_reset_status(codec);
}

void mtp_codec_setup_data_phrase(mtp_codec_t *codec, mtp_direction_t direction, stream_mode_t mode) {
    assert(codec->state == COMMAND);
    codec->transaction.data_stream.stream_mode = mode;
    codec->transaction.data_stream.direction = direction;
    codec->transaction.state = MTP_TRX_DATA;
    if (direction == DEVICE_TO_HOST) {
        codec->state = DATA_OUT;
    } else if (direction == HOST_TO_DEVICE) {
        codec->state = DATA_IN;
    } else {
        codec->state = RESPONSE;
    }
}

void mtp_codec_get_internal_tx_data_buffer(mtp_codec_t *codec, mtp_buffer_t *buffer) {
    buffer->buff = codec->tx_buffer_data + MTP_CONTAINER_HEADER_SIZE;
    buffer->limit = 0;
    buffer->pos = 0;
    buffer->cap = codec->tx_buffer.cap - MTP_CONTAINER_HEADER_SIZE;
}

void mtp_codec_send_data_container(mtp_codec_t *codec, mtp_container_header_t *header, mtp_buffer_t *buffer) {
    assert(codec->state == DATA_OUT);
    codec->transaction.data_stream.total_bytes = header->length - MTP_CONTAINER_HEADER_SIZE;
    codec->transaction.data_stream.transfer_bytes = mtp_buff_get_size(buffer);
    mtp_buff_reset_limit(&codec->tx_buffer);
    mtp_codec_encode_container_header(&codec->tx_buffer, header);
    if (mtp_codec_is_internal_tx_buffer(codec, buffer)) {
        mtp_buff_set_limit(&codec->tx_buffer,
                           mtp_buff_get_size(&codec->tx_buffer) + mtp_buff_get_size(buffer));
    } else {
        mtp_codec_encode_container_payload(&codec->tx_buffer,
                                           mtp_buff_get_data_ptr_pos(buffer),
                                           mtp_buff_get_size(buffer));
    }
    mtp_codec_usbd_ep_in_transfer(codec);
}

void mtp_codec_send_data_chunk(mtp_codec_t *codec, mtp_buffer_t *buffer) {
    assert(codec->state = DATA_OUT);
    mtp_buff_reset_limit(&codec->tx_buffer);
    codec->transaction.data_stream.transfer_bytes += mtp_buff_get_size(buffer);
    if (mtp_codec_is_internal_tx_buffer(codec, buffer)) {
        mtp_buff_set_limit(&codec->tx_buffer,
                           mtp_buff_get_size(&codec->tx_buffer) + mtp_buff_get_size(buffer));
    } else {
        mtp_codec_encode_container_payload(&codec->tx_buffer,
                                           mtp_buff_get_data_ptr_pos(buffer),
                                           mtp_buff_get_size(buffer));
    }
    mtp_codec_usbd_ep_in_transfer(codec);
}


void mtp_codec_send_response(mtp_codec_t *codec, mtp_container_header_t *header,
                             mtp_container_parameter_t *param) {

    //force to response state
    if (codec->state != RESPONSE) {
        codec->state = RESPONSE;
    }
    mtp_buff_reset_pos(&codec->tx_buffer);
    mtp_codec_encode_container_header(&codec->tx_buffer, header);
    if (param != NULL) {
        mtp_codec_encode_container_parameter(&codec->tx_buffer, param);
    }
    codec->transaction.state = MTP_TRX_END;
    mtp_codec_usbd_ep_in_transfer(codec);
}



///////////////////////////////////////////////////////////////////////////////////

static void mtp_codec_usbd_ep_in_transfer(mtp_codec_t *codec) {
    NRF_DRV_USBD_TRANSFER_IN(transfer, mtp_buff_get_data(&codec->tx_buffer),
                             mtp_buff_get_size(&codec->tx_buffer));
    nrf_drv_usbd_ep_transfer(codec->data_ep_in, &transfer);
}

void mtp_codec_usbd_ep_out_data_ready(mtp_codec_t *codec, app_usbd_complex_evt_t const *p_event) {
    NRF_DRV_USBD_TRANSFER_OUT(transfer, mtp_buff_get_data_ptr_limit(&codec->rx_buffer),
                              nrfx_usbd_epout_size_get(p_event->drv_evt.data.eptransfer.ep));

    size_t transfer_size = nrfx_usbd_epout_size_get(p_event->drv_evt.data.eptransfer.ep);
    codec->data_ep_size = transfer_size;
    nrf_drv_usbd_ep_transfer(codec->data_ep_out, &transfer);
}

void mtp_codec_usbd_ep_out_data_received(mtp_codec_t *codec, app_usbd_complex_evt_t const *p_event) {
    size_t transfer_size = codec->data_ep_size;
    mtp_buff_set_limit(&codec->rx_buffer, mtp_buff_get_size(&codec->rx_buffer) + transfer_size);
    size_t data_size = mtp_buff_get_remain_size(&codec->rx_buffer);

    switch (codec->state) {
        // command phrase
        case IDLE: {
            if (data_size >= MTP_CONTAINER_HEADER_SIZE) {
                mtp_container_header_t header;
                mtp_codec_decode_container_header(&codec->rx_buffer, &header);
                if (header.type == MTP_CONTAINER_TYPE_COMMAND) {
                    mtp_codec_decode_container_parameter(&codec->rx_buffer, &header, &codec->transaction.parameter);
                    mtp_buff_reset(&codec->rx_buffer);

                    mtp_codec_event_t event = {
                            .codec = codec,
                            .type = MTP_CODEC_EVENT_COMMAND_RX_DONE,
                    };
                    codec->transaction.op_code = header.code;
                    codec->transaction.state = MTP_TRX_BEGIN;
                    codec->transaction.id = header.transaction_id;
                    codec->transaction.data_stream.direction = NONE;
                    codec->transaction.data_stream.stream_mode = STREAM_MODE_BUFFER;
                    codec->transaction.data_stream.total_bytes = -1;
                    codec->transaction.data_stream.transfer_bytes = -1;
                    codec->state = COMMAND;
                    mtp_codec_call_event_handler(codec, &event);
                }
            }
            break;
            // data phrase 
            case DATA_IN: {
                // first header
                if (!codec->transaction.data_stream.header_received && data_size >= MTP_CONTAINER_HEADER_SIZE) {
                    mtp_container_header_t header;
                    mtp_codec_decode_container_header(&codec->rx_buffer, &header);
                    codec->transaction.data_stream.total_bytes = header.length - MTP_CONTAINER_HEADER_SIZE;
                    codec->transaction.data_stream.transfer_bytes = data_size - MTP_CONTAINER_HEADER_SIZE;
                    codec->transaction.data_stream.header_received = true;
                    codec->transaction.state = MTP_TRX_DATA;

                    mtp_data_chunk_t chunk = {
                            .buffer = &codec->rx_buffer
                    };

                    mtp_codec_event_t event = {
                            .codec = codec,
                            .type = MTP_CODEC_EVENT_DATA_HEADER_RX_DONE,
                            .chunk = &chunk
                    };

                    mtp_codec_call_event_handler(codec, &event);

                    if (codec->transaction.data_stream.stream_mode == STREAM_MODE_CHUNK) {
                        mtp_buff_reset(&codec->rx_buffer);
                    }

                } else if ( codec->transaction.data_stream.header_received) { //chunk data
                    codec->transaction.data_stream.transfer_bytes += transfer_size;
                    if (codec->transaction.data_stream.stream_mode == STREAM_MODE_CHUNK) {

                        mtp_data_chunk_t chunk = {
                                .buffer = &codec->rx_buffer
                        };

                        mtp_codec_event_t event = {
                                .codec = codec,
                                .type = MTP_CODEC_EVENT_DATA_CHUNK_RX_DONE,
                                .chunk = &chunk
                        };
                        mtp_codec_call_event_handler(codec, &event);
                        mtp_buff_reset(&codec->rx_buffer);
                    }
                }

                //is all data received
                if (codec->transaction.data_stream.total_bytes > 0 &&
                    codec->transaction.data_stream.total_bytes == codec->transaction.data_stream.transfer_bytes) {

                    MTP_NEW_BUFFER(buffer,
                                   mtp_buff_get_data_ptr_pos(&codec->rx_buffer),
                                   mtp_buff_get_remain_size(&codec->rx_buffer));

                    mtp_data_chunk_t chunk = {
                            .buffer = &buffer
                    };

                    mtp_codec_event_t event = {
                            .codec = codec,
                            .type = MTP_CODEC_EVENT_DATA_ALL_RX_DONE,
                            .chunk = &chunk
                    };
                    mtp_codec_call_event_handler(codec, &event);
                    codec->transaction.data_stream.total_bytes = -1;
                    codec->transaction.data_stream.transfer_bytes = -1;
                    codec->transaction.data_stream.direction = NONE;
                    codec->transaction.data_stream.stream_mode = STREAM_MODE_BUFFER;
                    mtp_buff_reset(&codec->rx_buffer);
                    codec->state = RESPONSE; //enter response phrase
                }
            }
            break;
        }
    }

}

void mtp_codec_usbd_ep_in_data_sent(mtp_codec_t *codec, app_usbd_complex_evt_t const *p_event) {

    mtp_buff_reset(&codec->tx_buffer);

    switch (codec->state) {
        case DATA_OUT: {
            if (codec->transaction.data_stream.transfer_bytes == codec->transaction.data_stream.total_bytes) {
                mtp_codec_event_t event = {
                        .codec = codec,
                        .type = MTP_CODEC_EVENT_DATA_ALL_TX_DONE,
                };
                mtp_codec_call_event_handler(codec, &event);
            } else {
                mtp_codec_event_t event = {
                        .codec = codec,
                        .type = MTP_CODEC_EVENT_DATA_CHUNK_TX_READY,
                };
                mtp_codec_call_event_handler(codec, &event);
            }
        }
            break;
        case RESPONSE: {
            mtp_codec_reset_status(codec);
        }
            break;
    }
}