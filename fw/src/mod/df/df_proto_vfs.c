#include "df_proto_vfs.h"
#include "df_buffer.h"
#include "df_defines.h"
#include "vfs.h"

static vfs_driver_t *get_driver_by_path(char *path) {
    if (path[0] == 'I') {
        return vfs_get_driver(VFS_DRIVE_INT);
    } else if (path[0] == 'E') {
        return vfs_get_driver(VFS_DRIVE_EXT);
    } else {
        return NULL;
    }
}

static char *get_file_path(char *path) { return path + VFS_DRIVE_LABEL_LEN; }

void df_proto_handler_vfs_drive_list(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_frame_t out;

        NEW_BUFFER_ZERO(buff, out.data, sizeof(out.data));

        uint8_t drv_cnt = vfs_drive_enabled(VFS_DRIVE_INT) + vfs_drive_enabled(VFS_DRIVE_EXT);
        buff_put_u8(&buff, drv_cnt); // drive count

        if (vfs_drive_enabled(VFS_DRIVE_INT)) {
            vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
            vfs_stat_t stat;
            if (p_driver->stat(&stat) == VFS_OK && !stat.avaliable) {
                p_driver->mount();
            }

            if (p_driver->stat(&stat) == VFS_OK && !stat.avaliable) {
                buff_put_u8(&buff, stat.avaliable); // drive status code
                buff_put_char(&buff, 'I');          // drive label
                buff_put_string(&buff, "Internal Flash");
                buff_put_u32(&buff, stat.total_bytes); // total space
                buff_put_u32(&buff, stat.free_bytes);  // free space
            } else {
                buff_put_u8(&buff, 1);     // drive status code
                buff_put_char(&buff, 'I'); // drive label
                buff_put_string(&buff, "Internal Flash");
                buff_put_u32(&buff, 0); // total space
                buff_put_u32(&buff, 0); // free space
            }
        }

        if (vfs_drive_enabled(VFS_DRIVE_EXT)) {
            vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
            vfs_stat_t stat;

            if (p_driver->stat(&stat) == VFS_OK && !stat.avaliable) {
                p_driver->mount();
            }

            if (p_driver->stat(&stat) == VFS_OK) {
                buff_put_u8(&buff, stat.avaliable); // drive status code
                buff_put_char(&buff, 'E');          // drive label
                buff_put_string(&buff, "External Flash");
                buff_put_u32(&buff, stat.total_bytes); // total space
                buff_put_u32(&buff, stat.free_bytes);  // free space
            } else {
                buff_put_u8(&buff, 1);     // drive status code
                buff_put_char(&buff, 'E'); // drive label
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
    }
}

typedef struct {
    vfs_dir_t dir;
    vfs_obj_t obj;
    bool obj_consumed;
    bool dir_closed;
    uint16_t chunk;
    vfs_driver_t *driver;
} dir_chunk_state_t;

static void dir_read_send_chunk(dir_chunk_state_t *chunk_state, df_frame_t *out) {

    NEW_BUFFER_ZERO(buff, out->data, sizeof(out->data));

    if(chunk_state->dir_closed){
        return;
    }

    if (!chunk_state->obj_consumed) {
        buff_put_string(&buff, chunk_state->obj.name);
        buff_put_u32(&buff, chunk_state->obj.size);
        buff_put_u8(&buff, chunk_state->obj.type);
    }

    while ((chunk_state->driver->read_dir(&chunk_state->dir, &chunk_state->obj)) == VFS_OK) {
        uint8_t size_required = strlen(chunk_state->obj.name) + 7;
        if (buffer_get_available_cap(&buff) >= size_required) {
            buff_put_string(&buff, chunk_state->obj.name);
            buff_put_u32(&buff, chunk_state->obj.size);
            buff_put_u8(&buff, chunk_state->obj.type);
            chunk_state->obj_consumed = true;
        } else {
            chunk_state->obj_consumed = false;
            break;
        }
    }

    out->cmd = DF_PROTO_CMD_VFS_DIR_READ;
    if (chunk_state->obj_consumed && !chunk_state->dir_closed) {
        out->chunk = 0x8000 | chunk_state->chunk;
        chunk_state->dir_closed = true;
        chunk_state->driver->close_dir(&chunk_state->dir);
    } else {
        out->chunk =  chunk_state->chunk;
    }
    out->status = DF_STATUS_OK;
    out->length = buff_get_size(&buff);

    out->chunk++;
    df_core_send_frame(out);
}

void df_proto_handler_vfs_dir_read(df_event_t *evt) {

    static dir_chunk_state_t chunk_state;
    df_frame_t out;

    if (evt->type == DF_EVENT_DATA_RECEVIED) {

        NEW_BUFFER(buff, evt->df->data, evt->df->length);
        char path[VFS_MAX_FULL_PATH_LEN];
        buff_get_string(&buff, path, sizeof(path));

        chunk_state.driver = get_driver_by_path(path);
        if (chunk_state.driver == NULL) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }

        int32_t err = chunk_state.driver->open_dir(get_file_path(path), &chunk_state.dir);
        if (err) {
            // TODO mapping error
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }

        chunk_state.chunk = 0;
        chunk_state.dir_closed = false;
        chunk_state.obj_consumed = true;

        dir_read_send_chunk(&chunk_state, &out);
    } else if (evt->type == DF_EVENT_DATA_TRANSMIT_READY) {
        dir_read_send_chunk(&chunk_state, &out);
    } else if (evt->type == DF_EVENT_LINK_DISCONNECTED) {
    }
}

const df_cmd_entry_t df_proto_handler_vfs_entries[] = {
    {DF_PROTO_CMD_VFS_DRIVE_LIST, df_proto_handler_vfs_drive_list},
    {DF_PROTO_CMD_VFS_DRIVE_FORMAT, df_proto_handler_vfs_drive_format},
    {DF_PROTO_CMD_VFS_DIR_READ, df_proto_handler_vfs_dir_read},
    {0, NULL}};