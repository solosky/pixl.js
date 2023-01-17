import { sharedEventDispatcher } from "./event"
import * as pixl from "./pixl.ble"
import * as ByteBuffer from "bytebuffer"


var m_api_resolve;
var m_api_reject;

export function init() {
    sharedEventDispatcher().addListener("ble_rx_data", one_rx_data);
    ByteBuffer.DEFAULT_ENDIAN = ByteBuffer.LITTLE_ENDIAN;
}

export function get_version() {
    console.log("get_version");
    tx_data_frame(0x01, 0, 0);

    return new_rx_promise().then(data => {
        var bb = ByteBuffer.wrap(data);
        bb.skip(4);
        var ver_size = bb.readUint16();
        var version = {
            ver: bb.readUTF8String(ver_size)
        }

        return version;
    });
}

export function enter_dfu() {
    console.log("enter_dfu");
    tx_data_frame(0x02, 0, 0);
    return new_rx_promise();
}

export function vfs_get_drive_list() {
    console.log("vfs_get_drive_list");



    tx_data_frame(0x10, 0, 0);
    return new_rx_promise().then(data => {
        var bb = ByteBuffer.wrap(data);
        var h = read_header(bb);
        var drives = [];
        var d_cnt = bb.readUint8();
        if (d_cnt > 0) {
            var drive = {};
            drive.status = bb.readUint8();
            drive.label = String.fromCharCode(bb.readByte());
            drive.name = read_string(bb);
            drive.total_size = bb.readUint32();
            drive.used_size = bb.readUint32();

            drives.push(drive);
        }
        return drives;
    });
}

export function vfs_read_folder(dir) {
    console.log("vfs_read_dir", dir);

    var p = new_rx_promise().then(data => {
        var bb = ByteBuffer.wrap(data);
        var h = read_header(bb);

        if (h.status == 0) {
            var files = [];
            while (bb.remaining() > 0) {
                var file = {};
                file.name = read_string(bb);
                file.size = bb.readUint32();
                file.type = bb.readUint8();
                files.push(file);
            }
            h.data = files;
        }
        return h;
    });

    var bb = new ByteBuffer();
    write_string(bb, dir);
    tx_data_frame(0x16, 0, 0, bb);

    return p;
}


function read_header(bb) {
    return {
        cmd: bb.readUint8(),
        status: bb.readUint8(),
        chunk: bb.readUint16()
    }
}

function read_string(bb) {
    var size = bb.readUint16();
    if (size > 0) {
        return bb.readUTF8String(size);
    }
    return null;
}

function write_string(bb, str) {
    bb.writeUint16(str.length);
    for (var i = 0; i < str.length; i++) {
        bb.writeByte(str.charCodeAt(i));
    }
}

function tx_data_frame(cmd, status, chunk, data) {
    var bb = new ByteBuffer();
    bb.writeUint8(cmd);
    bb.writeUint8(status);
    bb.writeUint16(chunk);
    if (data) {
        data.flip();
        var data_remain = data.remaining();
        for (var i = 0; i < data_remain; i++) {
            bb.writeByte(data.readByte());
        }
    }
    bb.flip();
    pixl.tx_data(bb.toArrayBuffer());
}



function new_rx_promise() {
    return new Promise((resolve, reject) => {
        m_api_reject = reject;
        m_api_resolve = resolve;
    });
}



function one_rx_data(data) {
    if (m_api_resolve) {
        m_api_resolve(data);
        m_api_resolve = null;
    }
}
