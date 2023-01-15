#include "df_core.h"

#include "ble_df_driver.h"
#include "df_driver.h"

#include "df_proto_info.h"
#include "df_proto_vfs.h"

#include "nrf_log.h"

#define PROTO_HANDLER_MAX_GROUP 2

typedef struct {
    df_driver_t *driver;
    df_cmd_entry_t *cmd;
    df_cmd_entry_t * cmd_entries[PROTO_HANDLER_MAX_GROUP];
} df_context_t;

static df_context_t m_df_ctx;

static df_cmd_entry_t *df_core_find_proto_handler_entry(uint8_t cmd) {
    for (int32_t i = 0; i < PROTO_HANDLER_MAX_GROUP; i++) {
        df_cmd_entry_t *e = m_df_ctx.cmd_entries[i];
        while (e->cmd > 0) {
            if (e->cmd == cmd) {
                return e;
            }
            e++;
        }
    }

    return NULL;
}

static void df_core_on_event(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        m_df_ctx.cmd = df_core_find_proto_handler_entry(evt->df->cmd);
        NRF_LOG_INFO("cmd entry:%X", m_df_ctx.cmd);
        if (m_df_ctx.cmd) {
            m_df_ctx.cmd->handler(evt);
        }
    } else {
        if (m_df_ctx.cmd) {
            m_df_ctx.cmd->handler(evt);
        }
    }
}

void df_core_init() {
    m_df_ctx.driver = ble_get_df_driver();
    m_df_ctx.driver->init(df_core_on_event);
    m_df_ctx.cmd_entries[0] = df_proto_handler_info_entries;
    m_df_ctx.cmd_entries[1] = df_proto_handler_vfs_entries;
    // m_df_ctx.handler_entries[2] = df_proto_handler_amii_entries;
}
void df_core_send_frame(df_frame_t *out) { m_df_ctx.driver->send(out); }