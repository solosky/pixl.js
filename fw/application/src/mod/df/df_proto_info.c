#include "df_proto_info.h"
#include "df_buffer.h"
#include "utils2.h"

#include "ble_main.h"
#include "nrf_log.h"
#include "version2.h"

#include "mui_core.h"
#include "mui_input.h"

typedef struct {
    uint16_t offset;
    uint16_t chunk;
} screenshot_chunk_state_t;

void df_proto_handler_info_get_version(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_frame_t out;

        NEW_BUFFER_ZERO(buff, out.data, sizeof(out.data));
        buff_put_string(&buff, version_get_version(version_get()));
        char ble_addr[24];
        ble_get_addr_str(ble_addr);
        buff_put_string(&buff, ble_addr);

        OUT_FRAME_WITH_DATA_0(out, DF_PROTO_CMD_INFO_VERSION_INFO, DF_STATUS_OK, buff_get_size(&buff));

        df_core_send_frame(&out);
    }
}

void df_proto_handler_info_enter_dfu(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_frame_t out;
        OUT_FRAME_NO_DATA(out, DF_PROTO_CMD_INFO_ENTER_DFU, DF_STATUS_OK);
        df_core_send_frame(&out);
    } else if (evt->type == DF_EVENT_DATA_TRANSMIT_READY) {
        NRF_LOG_INFO("enter dfu ...");
        ble_disable();
        enter_dfu();
    }
}

void screen_shot_send_chunk(df_frame_t *out, screenshot_chunk_state_t *state) {
    u8g2_t *u8g2 = &(mui()->u8g2);
    uint8_t *fb = u8g2_GetBufferPtr(u8g2);
    uint16_t fb_size = 8 * u8g2_GetBufferTileHeight(u8g2) * u8g2_GetBufferTileWidth(u8g2);

    if (state->offset < fb_size) {
        uint16_t chunk_size = MIN(fb_size - state->offset, sizeof(out->data));
        memcpy(out->data, fb + state->offset, chunk_size);
        uint16_t chunk = state->offset + chunk_size < fb_size ? ( 0x8000 | state->chunk) : state->chunk;
        OUT_FRAME_WITH_DATA_CHUNK(*out, DF_PROTO_CMD_INFO_SCREEN_SHOT, DF_STATUS_OK, chunk, chunk_size);
        state->chunk++;
        state->offset += chunk_size;
        df_core_send_frame(out);
    } 
    
}

void df_proto_handler_screen_shot(df_event_t *evt) {
    static screenshot_chunk_state_t state;
    df_frame_t out;

    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        state.offset = 0;
        screen_shot_send_chunk(&out, &state);
    } else if (evt->type == DF_EVENT_DATA_TRANSMIT_READY) {
        NRF_LOG_INFO("screen shot ...");
        screen_shot_send_chunk(&out, &state);
    }
}

void df_proto_handler_info_send_key(df_event_t *evt) {

    df_frame_t out;

    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        NEW_BUFFER(buff, evt->df->data, evt->df->length);

        uint8_t key = buff_get_u8(&buff);
        uint8_t type = buff_get_u8(&buff);
        
        if (key >= INPUT_KEY_MAX || type >= INPUT_TYPE_MAX) {
            OUT_FRAME_NO_DATA(out, DF_PROTO_CMD_INFO_SEND_KEY, DF_STATUS_ERR);
            df_core_send_frame(&out);
        }

        mui_input_event_t input_event = {
            .key = key,
            .type = type,
        };

        mui_input_post_event(&input_event);

        OUT_FRAME_NO_DATA(out, DF_PROTO_CMD_INFO_SEND_KEY, DF_STATUS_OK);
        df_core_send_frame(&out);
    }
}

const df_cmd_entry_t df_proto_handler_info_entries[] = {
    {DF_PROTO_CMD_INFO_VERSION_INFO, df_proto_handler_info_get_version},
    {DF_PROTO_CMD_INFO_ENTER_DFU, df_proto_handler_info_enter_dfu},
    {DF_PROTO_CMD_INFO_SCREEN_SHOT, df_proto_handler_screen_shot},
    {DF_PROTO_CMD_INFO_SEND_KEY, df_proto_handler_info_send_key},
    {0, NULL}};