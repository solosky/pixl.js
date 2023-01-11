#include "df_proto_info.h"

void df_proto_handler_info_get_version(df_core_t *p_core, df_frame_t *p_frame) {}

void df_proto_handler_info_enter_dfu(df_core_t *p_core, df_frame_t *p_frame) {}

extern const df_proto_handler_entry_t df_proto_handler_info_entries[] = {
    {DF_PROTO_CMD_INFO_VERSION_INFO, df_proto_handler_info_get_version},
    {DF_PROTO_CMD_INFO_ENTER_DFU, df_proto_handler_info_get_version},
    {0, NULL}};