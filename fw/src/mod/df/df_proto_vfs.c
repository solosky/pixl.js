#include "df_proto_vfs.h"
#include "df_buffer.h"
#include "vfs.h"

void df_proto_handler_vfs_drive_list(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_frame_t out;

        NEW_BUFFER_ZERO(buff, out.data, sizeof(out.data));

        uint8_t drv_cnt = vfs_drive_enabled(VFS_DRIVE_INT) + vfs_drive_enabled(VFS_DRIVE_EXT);
        buff_put_u8(&buff, drv_cnt); // drive count

        if (vfs_drive_enabled(VFS_DRIVE_INT)) {
            vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
            vfs_stat_t stat;
            if (p_driver->stat(&stat) == VFS_OK) {
                buff_put_u8(&buff, stat.avaliable); // drive status code
                buff_put_u8(&buff, "I");            // drive label
                buff_put_string(&buff, "Internal Flash");
                buff_put_u32(&buff, stat.total_bytes); // total space
                buff_put_u32(&buff, stat.free_bytes);  // used space
            } else {
                buff_put_u8(&buff, 1);   // drive status code
                buff_put_u8(&buff, "I"); // drive label
                buff_put_string(&buff, "Internal Flash");
                buff_put_u32(&buff, 0); // total space
                buff_put_u32(&buff, 0); // used space
            }
        }

        if (vfs_drive_enabled(VFS_DRIVE_EXT)) {
            vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
            vfs_stat_t stat;
            if (p_driver->stat(&stat) == VFS_OK) {
                buff_put_u8(&buff, stat.avaliable); // drive status code
                buff_put_u8(&buff, "E");            // drive label
                buff_put_string(&buff, "External Flash");
                buff_put_u32(&buff, stat.total_bytes); // total space
                buff_put_u32(&buff, stat.free_bytes);  // used space
            } else {
                buff_put_u8(&buff, 1);   // drive status code
                buff_put_u8(&buff, "E"); // drive label
                buff_put_string(&buff, "External Flash");
                buff_put_u32(&buff, 0); // total space
                buff_put_u32(&buff, 0); // used space
            }
        }

        OUT_FRAME_WITH_DATA_0(out, evt->df->cmd, DF_STATUS_OK, buff_get_size(&buff));

        df_core_send_frame(&out);
    }
}

void df_proto_handler_vfs_drive_format(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_frame_t out;

        NEW_BUFFER(buff, evt->df->data, evt->df->length);
        uint8_t drv = buff_get_u8(&buff);
        vfs_driver_t *p_driver = vfs_get_driver(drv);

        if (p_driver->format() == VFS_OK) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_OK);
        } else {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
        }
        df_core_send_frame(&out);
    } else if (evt->type == DF_EVENT_DATA_TRANSMIT_READY) {
        // do enter dfu
    }
}

const df_proto_handler_entry_t df_proto_handler_vfs_entries[] = {
    {DF_PROTO_CMD_INFO_VERSION_INFO, df_proto_handler_vfs_drive_list},
    {DF_PROTO_CMD_INFO_ENTER_DFU, df_proto_handler_vfs_drive_format},
    {0, NULL}};