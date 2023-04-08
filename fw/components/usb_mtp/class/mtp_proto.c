//
// Created by solos on 2022/1/2.
//

#include "mtp_proto.h"
#include "nrf_log.h"

#define DEFAULT_DATETIME "19700101T000000"

///*! global variables */
/*! List of MTP operations supported by device */
uint16_t mtp_supported_operations[] = {
        MTP_OPERATION_GET_DEVICE_INFO,
        MTP_OPERATION_OPEN_SESSION,
        MTP_OPERATION_CLOSE_SESSION,
        MTP_OPERATION_GET_STORAGE_IDS,

        MTP_OPERATION_GET_STORAGE_INFO,
        MTP_OPERATION_GET_OBJECT_HANDLES,
        MTP_OPERATION_GET_OBJECT_INFO,

        MTP_OPERATION_GET_OBJECT,
        MTP_OPERATION_DELETE_OBJECT,
        MTP_OPERATION_SEND_OBJECT_INFO,
        MTP_OPERATION_SEND_OBJECT,

        MTP_OPERATION_FORMAT_STORE,
        MTP_OPERATION_GET_DEVICE_PROP_DESC,
        MTP_OPERATION_GET_DEVICE_PROP_VALUE,

        MTP_OPERATION_MOVE_OBJECT,        // 0x1019
//        MTP_OPERATION_COPY_OBJECT,        // 0x101A
//        MTP_OPERATION_GET_PARTIAL_OBJECT, // 0x101B
        MTP_OPERATION_GET_OBJECT_PROPS_SUPPORTED, // 0x9801
        MTP_OPERATION_GET_OBJECT_PROP_DESC,       // 0x9802
        MTP_OPERATION_GET_OBJECT_PROP_VALUE,      // 0x9803
        MTP_OPERATION_SET_OBJECT_PROP_VALUE       // 0x9804
};

static const uint16_t supported_events[] = {
        //    MTP_EVENT_UNDEFINED                         ,//0x4000
        MTP_EVENT_CANCEL_TRANSACTION, // 0x4001
        MTP_EVENT_OBJECT_ADDED,       // 0x4002
        MTP_EVENT_OBJECT_REMOVED,     // 0x4003
        MTP_EVENT_STORE_ADDED,        // 0x4004
        MTP_EVENT_STORE_REMOVED,      // 0x4005
        //    MTP_EVENT_DEVICE_PROP_CHANGED               ,//0x4006
        //    MTP_EVENT_OBJECT_INFO_CHANGED               ,//0x4007
        //    MTP_EVENT_DEVICE_INFO_CHANGED               ,//0x4008
        //    MTP_EVENT_REQUEST_OBJECT_TRANSFER           ,//0x4009
        //    MTP_EVENT_STORE_FULL                        ,//0x400A
        MTP_EVENT_DEVICE_RESET,         // 0x400B
        MTP_EVENT_STORAGE_INFO_CHANGED, // 0x400C
        //    MTP_EVENT_CAPTURE_COMPLETE                  ,//0x400D
        MTP_EVENT_UNREPORTED_STATUS,   // 0x400E
        MTP_EVENT_OBJECT_PROP_CHANGED, // 0xC801
        //    MTP_EVENT_OBJECT_PROP_DESC_CHANGED          ,//0xC802
        //    MTP_EVENT_OBJECT_REFERENCES_CHANGED          //0xC803
};
const int supported_event_num = sizeof(supported_events) / sizeof(supported_events[0]);

static void mtp_proto_send_response_parameter(mtp_core_t *mtp, mtp_codec_event_t *event, uint32_t response_code,
                                              mtp_container_parameter_t *parameter) {
    mtp_container_header_t header;
    header.type = MTP_CONTAINER_TYPE_RESPONSE;
    header.length = MTP_CONTAINER_HEADER_SIZE;
    if (parameter != NULL && parameter->count > 0) {
        header.length += 4 * parameter->count;
    }
    header.code = response_code;
    header.transaction_id = event->transaction->id;
    mtp_codec_send_response(event->codec, &header, parameter);
}

static inline void mtp_proto_send_response_code(mtp_core_t *mtp, mtp_codec_event_t *event, uint32_t response_code) {
    mtp_proto_send_response_parameter(mtp, event, response_code, NULL);
}

static void mtp_proto_send_data_to_host(mtp_core_t *mtp, mtp_codec_event_t *event, mtp_buffer_t *buffer) {
    mtp_container_header_t header;
    header.length = MTP_CONTAINER_HEADER_SIZE + mtp_buff_get_size(buffer);
    header.code = event->transaction->op_code;
    header.transaction_id = event->transaction->id;
    header.type = MTP_CONTAINER_TYPE_DATA;

    mtp_codec_setup_data_phrase(event->codec, DEVICE_TO_HOST, STREAM_MODE_BUFFER);

    mtp_codec_send_data_container(event->codec, &header, buffer);
}


static void mtp_proto_get_device_info_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("get device info");
        MTP_NEW_BUFFER_LOCAL(buffer, MTP_BUFF_SIZE);

        mtp_buff_put_u16(&buffer, MTP_STANDARD_VERSION);
        mtp_buff_put_u32(&buffer, 6);
        mtp_buff_put_u16(&buffer, MTP_STANDARD_VERSION);
        mtp_buff_put_string(&buffer, "microsoft.com: 1.0;bosch-sensortec.com: 1.0;");
        mtp_buff_put_u16(&buffer, 0);
        mtp_buff_put_u32(&buffer, sizeof(mtp_supported_operations) / 2);
        for (int i = 0; i < sizeof(mtp_supported_operations) / 2; i++) {
            mtp_buff_put_u16(&buffer, mtp_supported_operations[i]);
        }

        mtp_buff_put_u32(&buffer, sizeof(supported_events) / sizeof(supported_events[0]));
        for (int i = 0; i < sizeof(supported_events) / 2; i++) {
            mtp_buff_put_u16(&buffer, supported_events[i]);
        }

        mtp_buff_put_u32(&buffer, 1); // Device properties (array of uint16)
        mtp_buff_put_u16(&buffer, MTP_DEVICE_PROPERTY_DEVICE_FRIENDLY_NAME);

        mtp_buff_put_u32(&buffer, 0);       // Capture formats (array of uint16)

        mtp_buff_put_u32(&buffer, 2);       // Playback formats (array of uint16)
        mtp_buff_put_u16(&buffer, MTP_FORMAT_UNDEFINED);
        mtp_buff_put_u16(&buffer, MTP_FORMAT_ASSOCIATION);

        mtp_buff_put_string(&buffer, MTP_STR_MANUFACTURER_NAME);
        mtp_buff_put_string(&buffer, MTP_STR_MODEL_NAME);
        mtp_buff_put_string(&buffer, MTP_STR_DEVICE_VERSION);
        mtp_buff_put_string(&buffer, MTP_STR_DEVICE_SERIAL);


        mtp_proto_send_data_to_host(mtp, event, &buffer);


    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}

static void mtp_proto_open_session_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("open session");
        mtp->fs_driver->fs_init();
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}

static void mtp_proto_close_session_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("close session");
        mtp->fs_driver->fs_deinit();
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}

static void mtp_proto_get_storage_ids_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("get storage ids");
        MTP_NEW_BUFFER_LOCAL(buffer, MTP_BUFF_SIZE);

        mtp_buff_put_u32(&buffer, 1);
        mtp_buff_put_u32(&buffer, 1);

        mtp_proto_send_data_to_host(mtp, event, &buffer);
    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}

static void mtp_proto_get_storage_info_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("get storage info");
        mtp_storage_info_t storage_info;
        MTP_NEW_BUFFER_LOCAL(buffer, MTP_BUFF_SIZE);

        mtp->fs_driver->fs_get_storage_info(&storage_info);
        mtp_buff_put_u16(&buffer, MTP_STORAGE_REMOVABLE_RAM);
        mtp_buff_put_u16(&buffer, MTP_STORAGE_FILESYSTEM_HIERARCHICAL);
        mtp_buff_put_u16(&buffer, MTP_STORAGE_READ_WRITE);
        mtp_buff_put_u64(&buffer, storage_info.capacity);
        mtp_buff_put_u64(&buffer, storage_info.free_space);
        mtp_buff_put_u32(&buffer, 0xFFFFFFFF);
        mtp_buff_put_string(&buffer, MTP_STR_STORAGE_DESCRIPTOR);
        mtp_buff_put_string(&buffer, MTP_STR_VOLUME_ID);

        mtp_proto_send_data_to_host(mtp, event, &buffer);
    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}


static void mtp_proto_get_object_handles_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        mtp_container_parameter_t *parameter = &event->transaction->parameter;
        NRF_LOG_INFO("get object handles: <%X, %X, %X>", parameter->parameter[0], parameter->parameter[1],
                     parameter->parameter[2]);
        if (parameter->parameter[1]) {
            mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_SPECIFICATION_BY_FORMAT_UNSUPPORTED);
        } else {
            uint32_t len = 0;
            uint32_t handles[MTP_MAX_NUM_OF_FILES];
            MTP_NEW_BUFFER_LOCAL(buffer, MTP_BUFF_SIZE);

            mtp->fs_driver->fs_get_object_handles(parameter->parameter[2], handles, &len);
            mtp_buff_put_u32(&buffer, len);
            for (uint32_t i = 0; i < len; ++i) {
                mtp_buff_put_u32(&buffer, handles[i]);
            }
            mtp_proto_send_data_to_host(mtp, event, &buffer);
        }
    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}


static void mtp_proto_get_object_info_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("get object info");
        mtp_container_parameter_t *parameter = &event->transaction->parameter;
        mtp_file_object_t file_object = {};
        mtp_buffer_t buffer;

        mtp_codec_get_internal_tx_data_buffer(event->codec, &buffer);

        mtp->fs_driver->fs_get_object_info_by_handle(parameter->parameter[0], &file_object);

        mtp_buff_put_u32(&buffer, 1);
        if (file_object.file_type == FS_FILE_TYPE_DIR) {
            mtp_buff_put_u16(&buffer, MTP_FORMAT_ASSOCIATION); //object format code
        } else {
            mtp_buff_put_u16(&buffer, MTP_FORMAT_UNDEFINED); //object format code
        }
        mtp_buff_put_u16(&buffer, 0);
        mtp_buff_put_u32(&buffer, file_object.size);
        mtp_buff_put_u16(&buffer, 0); // thumb format
        mtp_buff_put_u32(&buffer, 0); // thumb size
        mtp_buff_put_u32(&buffer, 0); // thumb width
        mtp_buff_put_u32(&buffer, 0); // thumb height
        mtp_buff_put_u32(&buffer, 0); // pix width
        mtp_buff_put_u32(&buffer, 0); // pix height
        mtp_buff_put_u32(&buffer, 0); // bit depth
        mtp_buff_put_u32(&buffer, 0xFFFFFFFF); // parent
        mtp_buff_put_u16(&buffer, file_object.file_type == FS_FILE_TYPE_DIR ? MTP_ASSOCIATION_TYPE_GENERIC_FOLDER
                                                                            : MTP_ASSOCIATION_TYPE_UNDEFINED); // association type
        mtp_buff_put_u32(&buffer, 0); // association description
        mtp_buff_put_u32(&buffer, 0);  // sequence number
        mtp_buff_put_string(&buffer, file_object.name);
        mtp_buff_put_string(&buffer, DEFAULT_DATETIME);  // date created
        mtp_buff_put_string(&buffer, DEFAULT_DATETIME);  // date modified
        mtp_buff_put_string(&buffer, "");  // keywords


        mtp_proto_send_data_to_host(mtp, event, &buffer);
    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}

static void mtp_proto_get_object_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("get object");
        mtp_file_object_t file_obj;
        uint32_t handle;
        int32_t read_size;
        mtp_buffer_t buffer;

        mtp_container_parameter_t *parameter = &event->transaction->parameter;
        handle = parameter->parameter[0];
        mtp->fs_driver->fs_get_object_info_by_handle(handle, &file_obj);

        mtp_container_header_t header;
        header.length = MTP_CONTAINER_HEADER_SIZE + file_obj.size;
        header.code = event->transaction->op_code;
        header.transaction_id = event->transaction->id;
        header.type = MTP_CONTAINER_TYPE_DATA;

        mtp_codec_get_internal_tx_data_buffer(event->codec, &buffer);

        mtp->fs_driver->fs_file_open(handle, FS_FLAG_READ);
        read_size = mtp->fs_driver->fs_file_read(handle, 0, mtp_buff_get_data_ptr_limit(&buffer),
                                                 mtp_buffer_get_available_cap(&buffer));
        if (read_size < 0) {
            mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_ACCESS_DENIED);
            return;
        }

        mtp->file_handle = handle;

        mtp_codec_setup_data_phrase(event->codec, DEVICE_TO_HOST, STREAM_MODE_CHUNK);
        mtp_buff_set_limit(&buffer, read_size);
        mtp_codec_send_data_container(event->codec, &header, &buffer);

        NRF_LOG_INFO("get object data header: %d/%d", event->transaction->data_stream.transfer_bytes,
                     event->transaction->data_stream.total_bytes);
    } else if (event->type == MTP_CODEC_EVENT_DATA_CHUNK_TX_READY) {
        size_t read_size;
        MTP_NEW_BUFFER_LOCAL(buffer, 256);
        read_size = mtp->fs_driver->fs_file_read(mtp->file_handle, event->transaction->data_stream.transfer_bytes,
                                                 mtp_buff_get_data_ptr_limit(&buffer),
                                                 mtp_buffer_get_available_cap(&buffer));
        if (read_size < 0) {
            //TODO .. send error event ..
        }
        mtp_buff_set_limit(&buffer, read_size);
        mtp_codec_send_data_chunk(event->codec, &buffer);

//        NRF_LOG_INFO("get object data chunk: %d/%d", event->transaction->data_stream.transfer_bytes,
//                     event->transaction->data_stream.total_bytes);
    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp->fs_driver->fs_file_close(mtp->file_handle);
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    } else if (event->type == MTP_CODEC_EVENT_CANCEL) {
        mtp->fs_driver->fs_file_close(mtp->file_handle);
    }

}

static void mtp_proto_delete_object_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("delete object");
        mtp_container_parameter_t *parameter = &event->transaction->parameter;
        if (mtp->fs_driver->fs_file_delete(parameter->parameter[0]) == 0) {
            mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
        } else {
            mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_ACCESS_DENIED);
        }
    }
}

static void mtp_proto_send_object_info_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("send object info: parent: %d", event->transaction->parameter.parameter[1]);
        mtp_codec_setup_data_phrase(event->codec, HOST_TO_DEVICE, STREAM_MODE_BUFFER);
    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_RX_DONE) {

        mtp_buffer_t *buffer = event->chunk->buffer;

        mtp_file_object_t file_object = {0};
        uint32_t file_handle;

        file_object.parent = event->transaction->parameter.parameter[1];

        if (file_object.parent == 0) {
            file_object.parent = 0xFFFFFFFF; //fix
        }

        mtp_buff_get_u32(buffer); // storage
        uint16_t obj_format = mtp_buff_get_u16(buffer); // format
        if (obj_format == MTP_FORMAT_ASSOCIATION) {
            file_object.file_type = FS_FILE_TYPE_DIR;
        } else {
            file_object.file_type = FS_FILE_TYPE_REG;
        }
        mtp_buff_get_u16(buffer);  // protection
        file_object.size = mtp_buff_get_u32(buffer); // size
        mtp_buff_get_u16(buffer); // thumb format
        mtp_buff_get_u32(buffer); // thumb size
        mtp_buff_get_u32(buffer); // thumb width
        mtp_buff_get_u32(buffer); // thumb height
        mtp_buff_get_u32(buffer); // pix width
        mtp_buff_get_u32(buffer); // pix height
        mtp_buff_get_u32(buffer); // bit depth
        mtp_buff_get_u32(buffer); // parent
        mtp_buff_get_u16(buffer); // association type
        mtp_buff_get_u32(buffer); // association description
        mtp_buff_get_u32(buffer); // sequence number
        mtp_buff_get_string(buffer, file_object.name);
        mtp_buff_get_string(buffer, NULL);
        mtp_buff_get_string(buffer, NULL);
        mtp_buff_get_string(buffer, NULL);

        NRF_LOG_INFO("send object info: <%s>, %d bytes", NRF_LOG_PUSH(file_object.name), file_object.size);
        mtp->fs_driver->fs_send_object_info_by_handle(&file_handle, &file_object);

        mtp_container_parameter_t parameter;
        parameter.count = 3;
        parameter.parameter[0] = 1;
        parameter.parameter[1] = file_object.parent;
        parameter.parameter[2] = file_handle;

        mtp->file_handle = file_handle;

        mtp_proto_send_response_parameter(mtp, event, MTP_RESPONSE_OK, &parameter);
    }
}

static void mtp_proto_send_object_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("send object");
        mtp_codec_setup_data_phrase(event->codec, HOST_TO_DEVICE, STREAM_MODE_CHUNK);
    } else if (event->type == MTP_CODEC_EVENT_DATA_HEADER_RX_DONE) {

        mtp->fs_driver->fs_file_open(mtp->file_handle, FS_FLAG_WRITE);
        mtp->fs_driver->fs_file_write(mtp->file_handle, 0,
                                      mtp_buff_get_data_ptr_pos(event->chunk->buffer),
                                      mtp_buff_get_remain_size(event->chunk->buffer));

        NRF_LOG_INFO("send object data header: %d/%d", event->transaction->data_stream.transfer_bytes,
                     event->transaction->data_stream.total_bytes);

    } else if (event->type == MTP_CODEC_EVENT_DATA_CHUNK_RX_DONE) {
        mtp->fs_driver->fs_file_write(mtp->file_handle,
                                      event->transaction->data_stream.transfer_bytes -
                                      mtp_buff_get_remain_size(event->chunk->buffer),
                                      mtp_buff_get_data_ptr_pos(event->chunk->buffer),
                                      mtp_buff_get_remain_size(event->chunk->buffer));
//        NRF_LOG_INFO("send object data chunk: %d/%d", event->transaction->data_stream.transfer_bytes,
//                     event->transaction->data_stream.total_bytes);
    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_RX_DONE) {
        mtp->fs_driver->fs_file_close(mtp->file_handle);
        NRF_LOG_INFO("send object data finished: %d", event->transaction->data_stream.transfer_bytes);
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    } else if (event->type == MTP_CODEC_EVENT_CANCEL) {
        mtp->fs_driver->fs_file_close(mtp->file_handle);
        mtp->fs_driver->fs_file_delete(mtp->file_handle);
    }
}


static void mtp_proto_format_storage_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("format storage");
        if (mtp->fs_driver->fs_format() == 0) {
            mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
        } else {
            mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_DEVICE_BUSY);
        }
    }
}

static void mtp_proto_get_device_prop_desc_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("get device prop desc");
        mtp_container_parameter_t *parameter = &event->transaction->parameter;
        MTP_NEW_BUFFER_LOCAL(buffer, MTP_BUFF_SIZE);

        if (parameter->parameter[0] == MTP_DEVICE_PROPERTY_DEVICE_FRIENDLY_NAME) {

            mtp_buff_put_u16(&buffer, MTP_DEVICE_PROPERTY_DEVICE_FRIENDLY_NAME);
            mtp_buff_put_u16(&buffer, MTP_TYPE_STR);
            mtp_buff_put_u8(&buffer, 0);
            mtp_buff_put_string(&buffer, MTP_STR_DEVICE_FRIENDLY_NAME);
            mtp_buff_put_string(&buffer, MTP_STR_DEVICE_FRIENDLY_NAME);
            mtp_buff_put_u8(&buffer, 0);

            mtp_proto_send_data_to_host(mtp, event, &buffer);
        } else {
            mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_DEVICE_PROP_NOT_SUPPORTED);
        }
    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}


static void mtp_proto_get_device_prop_value_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("get device prop value");
        mtp_container_parameter_t *parameter = &event->transaction->parameter;
        MTP_NEW_BUFFER_LOCAL(buffer, MTP_BUFF_SIZE);
        if (parameter->parameter[0] == MTP_DEVICE_PROPERTY_DEVICE_FRIENDLY_NAME) {
            mtp_buff_put_string(&buffer, MTP_STR_DEVICE_FRIENDLY_NAME);
            mtp_proto_send_data_to_host(mtp, event, &buffer);
        } else {
            mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_DEVICE_PROP_NOT_SUPPORTED);
        }
    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}

static void mtp_proto_get_object_prop_supported_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("get object prop supported");
        MTP_NEW_BUFFER_LOCAL(buffer, MTP_BUFF_SIZE);

        const uint16_t property_list[] = {
                MTP_PROPERTY_STORAGE_ID,        // 0xDC01
                MTP_PROPERTY_OBJECT_FORMAT,     // 0xDC02
                MTP_PROPERTY_PROTECTION_STATUS, // 0xDC03
                MTP_PROPERTY_OBJECT_SIZE,       // 0xDC04
                MTP_PROPERTY_OBJECT_FILE_NAME,  // 0xDC07
                MTP_PROPERTY_DATE_CREATED,      // 0xDC08
                MTP_PROPERTY_DATE_MODIFIED,     // 0xDC09
                MTP_PROPERTY_PARENT_OBJECT,     // 0xDC0B
                MTP_PROPERTY_PERSISTENT_UID,    // 0xDC41
                MTP_PROPERTY_NAME               // 0xDC44
        };
        const uint32_t property_list_num = sizeof(property_list) / sizeof(property_list[0]);
        mtp_buff_put_u32(&buffer, property_list_num);
        for (uint32_t i = 0; i < property_list_num; i++) {
            mtp_buff_put_u16(&buffer, property_list[i]);
        }
        mtp_proto_send_data_to_host(mtp, event, &buffer);

    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}

static void mtp_proto_get_object_prop_value_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("get object prop value");
        mtp_container_parameter_t *parameter = &event->transaction->parameter;
        uint32_t handle = parameter->parameter[0];
        uint32_t property = parameter->parameter[1];

        mtp_file_object_t file_obj;
        mtp->fs_driver->fs_get_object_info_by_handle(handle, &file_obj);

        MTP_NEW_BUFFER_LOCAL(buffer, MTP_BUFF_SIZE);
        switch (property) {
            case MTP_PROPERTY_STORAGE_ID:
                mtp_buff_put_u32(&buffer, 1);
                break;
            case MTP_PROPERTY_OBJECT_FORMAT:
                mtp_buff_put_u16(&buffer,
                                 file_obj.file_type == FS_FILE_TYPE_DIR ? MTP_FORMAT_ASSOCIATION
                                                                        : MTP_FORMAT_UNDEFINED);
                break;
            case MTP_PROPERTY_PROTECTION_STATUS:
                mtp_buff_put_u16(&buffer, 0);
                break;
            case MTP_PROPERTY_OBJECT_SIZE:
                mtp_buff_put_u32(&buffer, file_obj.size);
                mtp_buff_put_u32(&buffer, 0);
                break;
            case MTP_PROPERTY_OBJECT_FILE_NAME: // 0xDC07:
            case MTP_PROPERTY_NAME: // 0xDC44:
                mtp_buff_put_string(&buffer, file_obj.name);
                break;
            case MTP_PROPERTY_DATE_CREATED: // 0xDC08:
            case MTP_PROPERTY_DATE_MODIFIED: // 0xDC09:
                mtp_buff_put_string(&buffer, DEFAULT_DATETIME);
                break;
            case MTP_PROPERTY_PARENT_OBJECT:
                mtp_buff_put_u32(&buffer, file_obj.parent);
                break;
            case MTP_PROPERTY_PERSISTENT_UID:
                mtp_buff_put_u32(&buffer, handle);
                mtp_buff_put_u32(&buffer, file_obj.parent);
                mtp_buff_put_u32(&buffer, 1);
                mtp_buff_put_u32(&buffer, 0);
                break;
        }

        mtp_proto_send_data_to_host(mtp, event, &buffer);

    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}


static void mtp_proto_get_object_prop_desc_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("get object prop desc");
        mtp_container_parameter_t *parameter = &event->transaction->parameter;
        uint32_t property = parameter->parameter[0];
        MTP_NEW_BUFFER_LOCAL(buffer, MTP_BUFF_SIZE);

        switch (property) {
            case MTP_PROPERTY_STORAGE_ID: // 0xDC01:
                mtp_buff_put_u16(&buffer, 0xDC01);
                mtp_buff_put_u16(&buffer, 0x006); // 6=uint32_t
                mtp_buff_put_u8(&buffer, 0); // get
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u8(&buffer, 0);
                break;
            case MTP_PROPERTY_OBJECT_FORMAT: // 0xDC02:
                mtp_buff_put_u16(&buffer, 0xDC02);
                mtp_buff_put_u16(&buffer, 0x004); // 4=uint16_t
                mtp_buff_put_u8(&buffer, 0); // get
                mtp_buff_put_u16(&buffer, 0);
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u8(&buffer, 0);
                break;
            case MTP_PROPERTY_PROTECTION_STATUS: // 0xDC03:
                mtp_buff_put_u16(&buffer, 0xDC03);
                mtp_buff_put_u16(&buffer, 0x004); // 4=uint16_t
                mtp_buff_put_u8(&buffer, 0); // get
                mtp_buff_put_u16(&buffer, 0);
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u8(&buffer, 0);
                break;
            case MTP_PROPERTY_OBJECT_SIZE: // 0xDC04:
                mtp_buff_put_u16(&buffer, 0xDC04);
                mtp_buff_put_u16(&buffer, 0x008); // 8=uint64_t
                mtp_buff_put_u8(&buffer, 0); // get
                mtp_buff_put_u64(&buffer, 0);
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u8(&buffer, 0);
                break;
            case MTP_PROPERTY_OBJECT_FILE_NAME: // 0xDC07:
                mtp_buff_put_u16(&buffer, 0xDC07);
                mtp_buff_put_u16(&buffer, 0xFFFF); // FFFF=string
                mtp_buff_put_u8(&buffer, 1); // get/set
                mtp_buff_put_u8(&buffer, 0);
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u8(&buffer, 0);
                break;
            case MTP_PROPERTY_DATE_CREATED: // 0xDC08:
                mtp_buff_put_u16(&buffer, 0xDC08);
                mtp_buff_put_u16(&buffer, 0xFFFF); // FFFF=string
                mtp_buff_put_u8(&buffer, 1); // get
                mtp_buff_put_u8(&buffer, 0);
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u8(&buffer, 0); // TODO: should this be 3 (Form Flag on page 56)
                break;
            case MTP_PROPERTY_DATE_MODIFIED: // 0xDC09:
                mtp_buff_put_u16(&buffer, 0xDC09);
                mtp_buff_put_u16(&buffer, 0xFFFF); // FFFF=string
                mtp_buff_put_u8(&buffer, 1); // may be both get set?
                mtp_buff_put_u8(&buffer, 0);
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u8(&buffer, 0); // TODO: should this be 3 (Form Flag on page 56)
                break;
            case MTP_PROPERTY_PARENT_OBJECT: // 0xDC0B:
                mtp_buff_put_u16(&buffer, 0xDC0B);
                mtp_buff_put_u16(&buffer, 6); // 6=uint32_t
                mtp_buff_put_u8(&buffer, 0); // get
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u8(&buffer, 0);
                break;
            case MTP_PROPERTY_PERSISTENT_UID: // 0xDC41:
                mtp_buff_put_u16(&buffer, 0xDC41);
                mtp_buff_put_u16(&buffer, 0x0A); // A=uint128_t
                mtp_buff_put_u8(&buffer, 0); // get
                mtp_buff_put_u64(&buffer, 0);
                mtp_buff_put_u64(&buffer, 0);
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u8(&buffer, 0);
                break;
            case MTP_PROPERTY_NAME: // 0xDC44:
                mtp_buff_put_u16(&buffer, 0xDC44);
                mtp_buff_put_u16(&buffer, 0xFFFF);
                mtp_buff_put_u8(&buffer, 0); // get
                mtp_buff_put_u8(&buffer, 0);
                mtp_buff_put_u32(&buffer, 0);
                mtp_buff_put_u8(&buffer, 0);
                break;
        }


        mtp_proto_send_data_to_host(mtp, event, &buffer);

    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_TX_DONE) {
        mtp_proto_send_response_code(mtp, event, MTP_RESPONSE_OK);
    }
}

static void mtp_proto_set_object_prop_value_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("set object prop value");
        mtp_codec_setup_data_phrase(event->codec, HOST_TO_DEVICE, STREAM_MODE_BUFFER);
    } else if (event->type == MTP_CODEC_EVENT_DATA_ALL_RX_DONE) {
        mtp_container_parameter_t *parameter = &event->transaction->parameter;
        uint32_t object_id = parameter->parameter[0];
        uint32_t property = parameter->parameter[1];
        char file_name[MTP_MAX_FILENAME_LEN] = {0};
        mtp_buffer_t *buffer = event->chunk->buffer;
        int32_t response_code = MTP_RESPONSE_OK;

        switch (property) {
            case MTP_PROPERTY_OBJECT_FILE_NAME: //rename
                mtp_buff_get_string(buffer, file_name);
                NRF_LOG_INFO("rename file %d to <%s>", object_id, NRF_LOG_PUSH(file_name));
                if (mtp->fs_driver->fs_file_rename(object_id, file_name) < 0) {
                    response_code = MTP_RESPONSE_DEVICE_BUSY;
                    NRF_LOG_INFO("rename file failed");
                }
                break;
            default:
                response_code = MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
        }

        mtp_proto_send_response_code(mtp, event, response_code);

    }
}


static void mtp_proto_move_object_handler(mtp_core_t *mtp, mtp_codec_event_t *event) {
    if (event->type == MTP_CODEC_EVENT_COMMAND_RX_DONE) {
        NRF_LOG_INFO("move object");

        uint32_t handle = event->transaction->parameter.parameter[0];
        uint32_t parent_handle = event->transaction->parameter.parameter[2];
        int32_t response_code = MTP_RESPONSE_OK;

        if (mtp->fs_driver->fs_file_move(handle, parent_handle) < 0) {
            response_code = MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
        }

        mtp_proto_send_response_code(mtp, event, response_code);
    }
}

static mtp_cmd_t mtp_proto_supported_commands[] = {
        {
                MTP_OPERATION_GET_DEVICE_INFO,
                mtp_proto_get_device_info_handler
        },
        {
                MTP_OPERATION_OPEN_SESSION,
                mtp_proto_open_session_handler
        },
        {
                MTP_OPERATION_CLOSE_SESSION,
                mtp_proto_close_session_handler
        },
        {
                MTP_OPERATION_CLOSE_SESSION,
                mtp_proto_close_session_handler
        },
        {
                MTP_OPERATION_GET_STORAGE_IDS,
                mtp_proto_get_storage_ids_handler
        },
        {
                MTP_OPERATION_GET_STORAGE_INFO,
                mtp_proto_get_storage_info_handler
        },
        {
                MTP_OPERATION_GET_OBJECT_HANDLES,
                mtp_proto_get_object_handles_handler
        },
        {
                MTP_OPERATION_GET_OBJECT_INFO,
                mtp_proto_get_object_info_handler
        },
        {
                MTP_OPERATION_GET_OBJECT,
                mtp_proto_get_object_handler
        },
        {
                MTP_OPERATION_DELETE_OBJECT,
                mtp_proto_delete_object_handler
        },
        {
                MTP_OPERATION_SEND_OBJECT_INFO,
                mtp_proto_send_object_info_handler
        },
        {
                MTP_OPERATION_SEND_OBJECT,
                mtp_proto_send_object_handler
        },
        {
                MTP_OPERATION_FORMAT_STORE,
                mtp_proto_format_storage_handler
        },
        {
                MTP_OPERATION_GET_DEVICE_PROP_DESC,
                mtp_proto_get_device_prop_desc_handler
        },
        {
                MTP_OPERATION_GET_DEVICE_PROP_VALUE,
                mtp_proto_get_device_prop_value_handler
        },
        {
                MTP_OPERATION_GET_OBJECT_PROPS_SUPPORTED,
                mtp_proto_get_object_prop_supported_handler
        },
        {
                MTP_OPERATION_GET_OBJECT_PROP_VALUE,
                mtp_proto_get_object_prop_value_handler
        },
        {
                MTP_OPERATION_GET_OBJECT_PROP_DESC,
                mtp_proto_get_object_prop_desc_handler
        },
        {
                MTP_OPERATION_SET_OBJECT_PROP_VALUE,
                mtp_proto_set_object_prop_value_handler
        },
        {
                MTP_OPERATION_MOVE_OBJECT,
                mtp_proto_move_object_handler
        },
        {0, NULL}
};


mtp_cmd_t *mtp_proto_find_cmd_by_code(uint16_t code) {
    mtp_cmd_t *pos = mtp_proto_supported_commands;
    while (pos->op != 0) {
        if (pos->op == code) {
            return pos;
        }
        pos++;
    }
    return NULL;
}
