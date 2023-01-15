#ifndef PROTO_DEFINES_H
#define PROTO_DEFINES_H

#define DF_MAX_LEN 250
#define DF_HEADER_LEN 4
#define DF_PAYLOAD_LEN DF_MAX_LEN - DF_HEADER_LEN

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint8_t cmd;
    uint8_t status;
    uint16_t chunk;
    uint8_t data[DF_PAYLOAD_LEN];
    uint8_t length;
} df_frame_t;

typedef enum {
    DF_EVENT_LINK_CONNECTED,
    DF_EVENT_LINK_DISCONNECTED,
    DF_EVENT_DATA_RECEVIED,
    DF_EVENT_DATA_TRANSMIT_READY,
} df_event_type_t;

typedef struct {
    df_event_type_t type;
    df_frame_t *df;
} df_event_t;

typedef void (*df_event_handler_t)(df_event_t *evt);

typedef struct {
    uint8_t cmd;
    df_event_handler_t handler;
} df_cmd_entry_t;

typedef enum { DF_STATUS_OK = 0, DF_STATUS_ERR = 1 } df_status_t;

#define OUT_FRAME_NO_DATA(_out, _cmd, _status)                                                                         \
    (_out).cmd = (_cmd);                                                                                               \
    (_out).status = (_status);                                                                                         \
    (_out).chunk = 0;                                                                                                  \
    (_out).length = 0;

#define OUT_FRAME_WITH_DATA_0(_out, _cmd, _status, _size)                                                              \
    (_out).cmd = (_cmd);                                                                                               \
    (_out).status = (_status);                                                                                         \
    (_out).chunk = 0;                                                                                                  \
    (_out).length = _size;

#define OUT_FRAME_WITH_DATA_CHUNK(_out, _cmd, _status, _chunk, _size)                                                  \
    (_out).cmd = (_cmd);                                                                                               \
    (_out).status = (_status);                                                                                         \
    (_out).chunk = (_chunk);                                                                                           \
    (_out).length = (_size);

#endif