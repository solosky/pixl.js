#include "df_proto_vfs.h"
#include "df_buffer.h"
#include "df_core.h"
#include "df_defines.h"
#include "nrf_log.h"
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

    if (chunk_state->dir_closed) {
        return;
    }

    if (!chunk_state->obj_consumed) {
        buff_put_string(&buff, chunk_state->obj.name);
        buff_put_u32(&buff, chunk_state->obj.size);
        buff_put_u8(&buff, chunk_state->obj.type);
        chunk_state->obj_consumed = true;
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
    if (chunk_state->obj_consumed) {
        out->chunk = chunk_state->chunk;
        chunk_state->dir_closed = true;
        chunk_state->driver->close_dir(&chunk_state->dir);
    } else {
        out->chunk = 0x8000 | chunk_state->chunk;
    }
    out->status = DF_STATUS_OK;
    out->length = buff_get_size(&buff);

    chunk_state->chunk++;
    df_core_send_frame(out);
}

void df_proto_handler_vfs_dir_read(df_event_t *evt) {

    static dir_chunk_state_t chunk_state;
    df_frame_t out;

    if (evt->type == DF_EVENT_DATA_RECEVIED) {

        NEW_BUFFER(buff, evt->df->data, evt->df->length);

        char path[VFS_MAX_FULL_PATH_LEN];
        memset(path, 0, sizeof(path));
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

void df_proto_handler_vfs_dir_create(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_frame_t out;

        NEW_BUFFER(buff, evt->df->data, evt->df->length);
        char path[VFS_MAX_FULL_PATH_LEN];
        memset(path, 0, sizeof(path));
        buff_get_string(&buff, path, sizeof(path));

        vfs_driver_t *p_driver = get_driver_by_path(path);
        if (p_driver == NULL) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }

        if (p_driver->create_dir(get_file_path(path)) == VFS_OK) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_OK);
        } else {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
        }
        df_core_send_frame(&out);
    } else if (evt->type == DF_EVENT_DATA_TRANSMIT_READY) {
    }
}

void df_proto_handler_vfs_remove(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_frame_t out;

        NEW_BUFFER(buff, evt->df->data, evt->df->length);
        char path[VFS_MAX_FULL_PATH_LEN];
        memset(path, 0, sizeof(path));
        buff_get_string(&buff, path, sizeof(path));

        vfs_driver_t *p_driver = get_driver_by_path(path);
        if (p_driver == NULL) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }

        vfs_obj_t obj;
        if (p_driver->stat_file(get_file_path(path), &obj) != VFS_OK) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }

        int32_t err = 0;
        if (obj.type == VFS_TYPE_DIR) {
            err = p_driver->remove_dir(get_file_path(path));
        } else {
            err = p_driver->remove_file(get_file_path(path));
        }

        if (err != VFS_OK) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }
        OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_OK);
        df_core_send_frame(&out);

    } else if (evt->type == DF_EVENT_DATA_TRANSMIT_READY) {
    }
}

typedef struct {
    bool opened;
    vfs_file_t vfs_fd;
    vfs_driver_t *vfs_driver;
    int32_t err_code;
    uint16_t chunk;
} file_chunk_state_t;

static file_chunk_state_t file_chunk_state = {0};

void df_proto_handler_vfs_file_open(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_frame_t out;
        uint32_t flags;
        int32_t err_code;

        if (file_chunk_state.opened) {
            file_chunk_state.vfs_driver->close_file(&file_chunk_state.vfs_fd);
            file_chunk_state.opened = false;
        }

        NEW_BUFFER_READ(buff, evt->df->data, evt->df->length);
        char path[VFS_MAX_FULL_PATH_LEN];
        memset(path, 0, sizeof(path));
        buff_get_string(&buff, path, sizeof(path));
        flags = buff_get_u32(&buff);

        vfs_driver_t *p_driver = get_driver_by_path(path);
        if (p_driver == NULL) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }

        err_code = p_driver->open_file(get_file_path(path), &file_chunk_state.vfs_fd, flags);
        if (err_code != VFS_OK) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }

        file_chunk_state.opened = true;
        file_chunk_state.vfs_driver = p_driver;
        file_chunk_state.err_code = VFS_OK;

        NEW_BUFFER_ZERO(out_buff, out.data, sizeof(out.data));
        buff_put_u8(&out_buff, 0); // always 0

        OUT_FRAME_WITH_DATA_0(out, evt->df->cmd, DF_STATUS_OK, buff_get_size(&out_buff));
        df_core_send_frame(&out);

        return;
    } else if (evt->type == DF_EVENT_DATA_TRANSMIT_READY) {
    }
}

void df_proto_handler_vfs_file_close(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_frame_t out;
        int32_t err_code;

        NEW_BUFFER_READ(buff, evt->df->data, evt->df->length);

        uint8_t file_id = buff_get_u8(&buff); // ignore
        if (!file_chunk_state.opened) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }

        err_code = file_chunk_state.vfs_driver->close_file(&file_chunk_state.vfs_fd);
        if (err_code != VFS_OK) {
            OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }

        OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_OK);
        df_core_send_frame(&out);
    } else if (evt->type == DF_EVENT_DATA_TRANSMIT_READY) {
    }
}

void df_proto_handler_vfs_file_write(df_event_t *evt) {
    if (evt->type == DF_EVENT_DATA_RECEVIED) {
        df_frame_t out;

        NEW_BUFFER_READ(buff, evt->df->data, evt->df->length);

        uint8_t file_id = buff_get_u8(&buff); // ignore

        bool chunk_eof = !(evt->df->chunk & 0x8000);

        if (file_chunk_state.opened && file_chunk_state.err_code == VFS_OK) {
            void *data_buff = buff_get_data_ptr_pos(&buff);
            size_t data_size = buff_get_remain_size(&buff);

            int32_t bytes_written =
                file_chunk_state.vfs_driver->write_file(&file_chunk_state.vfs_fd, data_buff, data_size);
            if (bytes_written < 0) {
                file_chunk_state.err_code = bytes_written;
            }
        }

        if (chunk_eof) {
            if (file_chunk_state.opened && file_chunk_state.err_code == VFS_OK) {
                OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_OK);
            } else {
                OUT_FRAME_NO_DATA(out, evt->df->cmd, DF_STATUS_ERR);
            }

            df_core_send_frame(&out);
        }
    } else if (evt->type == DF_EVENT_DATA_TRANSMIT_READY) {
    }
}

void file_read_send_chunk(file_chunk_state_t *chunk_state, df_frame_t *out) {
    if (!chunk_state->opened || chunk_state->err_code != VFS_OK) {
        return;
    }

    void *data_buff = out->data;
    size_t data_size = out->length;
    bool chunk_eof = false;

    int32_t bytes_read = chunk_state->vfs_driver->read_file(&chunk_state->vfs_fd, data_buff, data_size);
    if (bytes_read == VFS_ERR_EOF || bytes_read < data_size) {
        chunk_state->err_code = bytes_read;
        chunk_eof = true;
    } else if (bytes_read < -1) {
        chunk_state->err_code = bytes_read;
        chunk_eof = true;
    }

    out->cmd = DF_PROTO_CMD_VFS_FILE_READ;
    out->status = chunk_state->err_code;
    if (chunk_eof) {
        out->chunk = chunk_state->chunk;
    } else {
        out->chunk = chunk_state->chunk & 0x8000;
    }

    chunk_state->chunk++;
    df_core_send_frame(&out);
}

void df_proto_handler_vfs_file_read(df_event_t *evt) {
    df_frame_t out;
    if (evt->type == DF_EVENT_DATA_RECEVIED) {

        NEW_BUFFER_READ(buff, evt->df->data, evt->df->length);

        uint8_t file_id = buff_get_u8(&buff); // ignore

        file_chunk_state.chunk = 0;
        file_chunk_state.err_code = VFS_OK;

        if (!file_chunk_state.opened) {
            OUT_FRAME_NO_DATA(out, DF_PROTO_CMD_VFS_FILE_READ, DF_STATUS_ERR);
            df_core_send_frame(&out);
            return;
        }

        file_read_send_chunk(&file_chunk_state, &out);

    } else if (evt->type == DF_EVENT_DATA_TRANSMIT_READY) {
        file_read_send_chunk(&file_chunk_state, &out);
    }
}

const df_cmd_entry_t df_proto_handler_vfs_entries[] = {
    {DF_PROTO_CMD_VFS_DRIVE_LIST, df_proto_handler_vfs_drive_list},
    {DF_PROTO_CMD_VFS_DRIVE_FORMAT, df_proto_handler_vfs_drive_format},
    {DF_PROTO_CMD_VFS_DIR_READ, df_proto_handler_vfs_dir_read},
    {DF_PROTO_CMD_VFS_DIR_CREATE, df_proto_handler_vfs_dir_create},
    {DF_PROTO_CMD_VFS_REMOVE, df_proto_handler_vfs_remove},
    {DF_PROTO_CMD_VFS_FILE_OPEN, df_proto_handler_vfs_file_open},
    {DF_PROTO_CMD_VFS_FILE_CLOSE, df_proto_handler_vfs_file_close},
    {DF_PROTO_CMD_VFS_FILE_WRITE, df_proto_handler_vfs_file_write},
    {DF_PROTO_CMD_VFS_FILE_READ, df_proto_handler_vfs_file_read},
    {0, NULL}};