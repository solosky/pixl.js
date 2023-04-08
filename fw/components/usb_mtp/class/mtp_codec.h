//
// Created by solos on 2022/1/2.
//

#ifndef FW_MTP_CODEC_H
#define FW_MTP_CODEC_H

#include <stdint.h>
#include "mtp_buffer.h"
#include "mtp_types.h"
#include "mtp_config.h"
#include "app_usbd_types.h"

struct mtp_codec;

typedef enum {
    NONE, /** No data phrase*/
    DEVICE_TO_HOST, /** data sends to host in data phrase */
    HOST_TO_DEVICE /** data receives from host in data phrase */
} mtp_direction_t;


/**
 * core states
 *
 * state transition:
 * IDLE -> COMMAND: when a command container is received
 * COMMAND -> DATA_IN: when data container is pending receive from host
 * COMMAND -> DATA_OUT: when data container is pending send to host
 * DATA_IN -> RESPONSE: when data container is full received from host
 * DATA_OUT -> RESPONSE: when data container is full sent to host
 * RESPONSE -> IDLE: when response container is sent to host
 */
typedef enum {
    IDLE, /** waiting for incoming command */
    COMMAND, /** a command container is received */
    DATA_IN, /** receiving data from host */
    DATA_OUT, /** send data to host */
    RESPONSE /** sending response to host */
} mtp_codec_state_t;


/**
 * mtp codec event type defines
 */
typedef enum {
    MTP_CODEC_EVENT_COMMAND_RX_DONE, /** a command container is fully received from host */
    MTP_CODEC_EVENT_DATA_CHUNK_TX_READY, /** a data chunk is sent to host, ready for next chunk sending */
    MTP_CODEC_EVENT_DATA_ALL_TX_DONE, /** all data chunks is sent to host*/
    MTP_CODEC_EVENT_DATA_HEADER_RX_DONE, /** a data container header is received from host*/
    MTP_CODEC_EVENT_DATA_CHUNK_RX_DONE, /** a data chunk is is received from host */
    MTP_CODEC_EVENT_DATA_ALL_RX_DONE, /** all data container is received from host */
    MTP_CODEC_EVENT_RESPONSE_RX_DONE, /** a response container is sent to host */
    MTP_CODEC_EVENT_RESET, /** a reset is issued*/
    MTP_CODEC_EVENT_CANCEL, /** cancel current transaction*/
    MTP_CODEC_EVENT_ERROR /** error accrued when communicating with host */
} mtp_codec_event_type_t;






/**
 * stream mode in data phrase
 */
typedef enum {
    STREAM_MODE_BUFFER, /** chunk data stored in buffer and commit in one event */
    STREAM_MODE_CHUNK /** chunk data commit by each chunk received */
} stream_mode_t;

typedef struct {
    mtp_buffer_t* buffer;
} mtp_data_chunk_t;

/**
 * stream state in data phrase
 */
typedef struct {
    mtp_direction_t direction; /**data direction in data phrase */
    stream_mode_t stream_mode; /** stream mode in data phrase*/
    size_t total_bytes; /** total bytes of the data phrase*/
    size_t transfer_bytes; /** already transfer bytes */
    bool header_received;
} mtp_data_stream_t;

/**
 * mtp transaction state
 */
typedef enum {
    MTP_TRX_NONE,
    MTP_TRX_BEGIN,
    MTP_TRX_DATA,
    MTP_TRX_END,
    MTP_TRX_CANCEL
} mtp_transaction_state_t;

/**
 * mtp transaction
 */
typedef struct {
    mtp_transaction_state_t state;
    uint32_t id;
    uint32_t op_code;
    mtp_container_parameter_t parameter;
    mtp_data_stream_t data_stream;
} mtp_transaction_t;


/**
 * mtp codec event object defines
 */
typedef struct {
    mtp_codec_event_type_t type; /** the mtp codec event type */
    struct mtp_codec *codec; /** the mtp codec instance */
    void *context; /** the event handler associated context */
    mtp_data_chunk_t* chunk;
    mtp_transaction_t *transaction;
} mtp_codec_event_t;


/**
 * mtp codec event callback
 */
typedef void (*mtp_codec_event_handler_t)(mtp_codec_event_t *event);

/**
 * core mtp codec class
 */
typedef struct mtp_codec {
    mtp_codec_state_t state; /** core state */
    mtp_buffer_t tx_buffer;
    mtp_buffer_t rx_buffer;

    uint8_t tx_buffer_data[MTP_BUFF_SIZE];/** actual tx data holder */
    uint8_t rx_buffer_data[MTP_BUFF_SIZE]; /** actual rx data holder */

    mtp_codec_event_handler_t evt_handler;
    void *evt_context;

    mtp_transaction_t transaction;
    size_t data_ep_size;

    uint8_t comm_ep_in;          //!< COMM subclass IN endpoint.
    uint8_t data_ep_out;         //!< DATA subclass OUT endpoint.
    uint8_t data_ep_in;          //!< DATA subclass IN endpoint.
} mtp_codec_t;


/**
 * initialize mtp_codec
 * @param codec
 */

void mtp_codec_init(mtp_codec_t *codec);

/**
 * force reset the codec to IDLE state
 * it will clear all state and reset to IDLE state for receiving next command
 * @param codec
 */
void mtp_codec_reset(mtp_codec_t *codec);


/**
 * cancel current on going transaction
 * @param codec
 * @param transaction_id transaction id
 */
void mtp_codec_cancel(mtp_codec_t *codec, uint32_t transaction_id);

/**
 * setup data phrase for next data phrase preparation
 * it must be called in MTP_CODEC_EVENT_COMMAND_RX_READY event
 * @param codec
 * @param direction
 * @param mode
 */
void mtp_codec_setup_data_phrase(mtp_codec_t *codec, mtp_direction_t direction, stream_mode_t mode);


/**
 * return internal tx data buffer for write data to internal data buffer directly
 * @param codec
 * @return
 */
void mtp_codec_get_internal_tx_data_buffer(mtp_codec_t *codec, mtp_buffer_t *buffer);

/**
 * initial data container send to host in data phrase
 * @param codec
 * @param header
 * @param buffer
 */
void mtp_codec_send_data_container(mtp_codec_t *codec, mtp_container_header_t *header, mtp_buffer_t *buffer);

/**
 * send a data chunk to host in data phrase
 * @param codec
 * @param buffer
 */
void mtp_codec_send_data_chunk(mtp_codec_t *codec, mtp_buffer_t *buffer);


/**
 * send response to host
 * @param codec
 * @param header
 * @param param
 */
void mtp_codec_send_response(mtp_codec_t *codec, mtp_container_header_t *header,
                             mtp_container_parameter_t *param);

/**
 * send a event to host
 * @param codec
 * @param header
 * @param param
 */
void mtp_codec_send_event(mtp_codec_t *codec, mtp_container_header_t *header,
                          mtp_container_parameter_t *param);



////USBD HAL

/**
 * called if usbd ep out is ready to receive
 * @param codec
 */
void mtp_codec_usbd_ep_out_data_ready(mtp_codec_t *codec, app_usbd_complex_evt_t const *p_event);

/**
 * called if usbd ep out is received
 * @param codec
 */
void mtp_codec_usbd_ep_out_data_received(mtp_codec_t *codec, app_usbd_complex_evt_t const *p_event);

/**
 * called if usbd ep in is sent
 * @param codec
 */
void mtp_codec_usbd_ep_in_data_sent(mtp_codec_t *codec, app_usbd_complex_evt_t const *p_event);

#endif //FW_MTP_CODEC_H
