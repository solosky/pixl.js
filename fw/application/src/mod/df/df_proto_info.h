#ifndef DF_PROTO_INFO_H
#define DF_PROTO_INFO_H

#include "df_core.h"


/** info proto command defines */
typedef enum {
    DF_PROTO_CMD_INFO_VERSION_INFO = 0x01,
    DF_PROTO_CMD_INFO_ENTER_DFU = 0x02,
    DF_PROTO_CMD_INFO_SCREEN_SHOT = 0x03,
    DF_PROTO_CMD_INFO_SEND_KEY = 0x04,
} df_proto_cmd_info_t;



extern const df_cmd_entry_t df_proto_handler_info_entries[];

#endif