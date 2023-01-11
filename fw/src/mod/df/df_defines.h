#ifndef PROTO_DEFINES_H
#define PROTO_DEFINES_H

#define DF_MAX_LEN 250
#define DF_HEADER_LEN 4
#define DF_PAYLOAD_LEN DF_MAX_LEN - DF_HEADER_LEN

#include <stdint.h>

typedef struct {
    uint8_t cmd;
    uint8_t status;
    uint16_t chunk;
} df_header_t;

typedef struct {
    uint8_t data[DF_PAYLOAD_LEN];
    uint8_t size;
} df_data_t;

typedef struct {
    df_header_t header;
    df_data_t data;
} df_frame_t;

typedef enum {
    DF_EVENT_LINK_CONNECTED,
    DF_EVENT_LINK_DISCONNECTED,
    DF_EVENT_DATA_RECEVIED,
    DF_EVENT_DATA_TRANSMITTED,
} df_event_type_t;

typedef struct {
    df_event_type_t type;
    df_frame_t df;
} df_event_t;


#endif