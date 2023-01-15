#include "df_core.h"

#include "ble_df_driver.h"
#include "df_driver.h"

#include "df_proto_info.h"

#define PROTO_HANDLER_MAX_GROUP 3

typedef struct {
    df_driver_t *driver;
    df_proto_handler_entry_t *current_handler;
    df_proto_handler_entry_t *handler_entries[PROTO_HANDLER_MAX_GROUP];
} df_context_t;

static df_context_t df_ctx;

static df_proto_handler_entry_t *df_core_find_proto_handler_entry(uint8_t cmd) {
    for (int32_t i = 0; i < PROTO_HANDLER_MAX_GROUP; i++) {
        df_proto_handler_entry_t *e = handler_entries[i];
        while (e->cmd > 0) {
            if (e->cmd == cmd) {
                return e;
            }
        }
    }

    return NULL;
}

static void df_core_on_event(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_ctx.current_handler = df_core_find_proto_handler(evt->df->cmd);
        if (df_ctx.current_handler) {
            df_ctx.current_handler->handler(evt);
        }
    } else {
        if (df_ctx.current_handler) {
            df_ctx.current_handler->handler(evt);
        }
    }
}

void df_core_init() {
    df_ctx.driver = ble_get_df_driver();
    df_ctx.driver->init(df_core_on_event);
    df_ctx.handler_entries[0] = df_proto_handler_info_entries;
    // df_ctx.handler_entries[1] = df_proto_handler_vfs_entries;
    // df_ctx.handler_entries[2] = df_proto_handler_amii_entries;
}
void df_core_send_frame(df_frame_t *out) { df_ctx.driver->send(out); }