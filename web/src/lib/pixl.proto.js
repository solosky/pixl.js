import { sharedEventDispatcher } from "./event"
import * as pixl from "./pixl.ble"
import * as ByteBuffer from "bytebuffer"

const MTU_SIZE = 250
const MTU_MAX_DATA_SIZE = 247
const DF_HEADER_SIZE = 4
const DF_MAX_DATA_SIZE = MTU_MAX_DATA_SIZE - DF_HEADER_SIZE

var op_queue = []
var op_ongoing = false

export function op_queue_push(cmd, tx_data_cb, rx_data_cb) {
    return new Promise((resolve, reject) => {
        var op = {
            cmd: cmd,
            tx_data_cb: tx_data_cb,
            rx_data_cb: rx_data_cb,
            p_resolve: resolve,
            p_reject: reject
        }
        op_queue.push(op);
        process_op_queue();
    });

}

function process_op_queue() {
    if (!op_ongoing && op_queue.length > 0) {
        var op = op_queue.shift();
        proocess_op(op);
    }
}

function proocess_op(op) {
    new_rx_promise().then(data => {
        try {
            var bb = ByteBuffer.wrap(data);
            var h = read_header(bb);
            h.data = op.rx_data_cb(bb);
            op_ongoing = false;
            op.p_resolve(h);
            process_op_queue();
            return h;
        } catch (e) {
            op.p_reject(e);
        }
    }).catch(e => {
        op_ongoing = false;
        op.p_reject(e);
        process_op_queue();
    });

    var bb = new ByteBuffer();
    op.tx_data_cb(bb);
    op_ongoing = true;
    tx_data_frame(op.cmd, 0, 0, bb).catch(e => {
        op.p_reject(e);
    });
}


var m_api_resolve;
var m_api_reject;

export function init() {
    sharedEventDispatcher().addListener("ble_rx_data", on_rx_data);
    sharedEventDispatcher().addListener("ble_disconnected", on_ble_disconnected);
    ByteBuffer.DEFAULT_ENDIAN = ByteBuffer.LITTLE_ENDIAN;
}

export function get_version() {
    console.log("get_version");
    return op_queue_push(0x01,
        b => { },
        b => {
            var ver = read_string(b);
            var ble_addr = "";
            if (b.remaining()) {
                ble_addr = read_string(b);
            }
            return {
                ver: ver,
                ble_addr: ble_addr
            }
        });
}

export function enter_dfu() {
    console.log("enter_dfu");
    return op_queue_push(0x02,
        b => { },
        b => { });
}

export function vfs_get_drive_list() {
    console.log("vfs_get_drive_list");
    return op_queue_push(0x10,
        b => { },
        b => {
            var drives = [];
            var d_cnt = b.readUint8();
            if (d_cnt > 0) {
                var drive = {};
                drive.status = b.readUint8();
                drive.label = String.fromCharCode(b.readByte());
                drive.name = read_string(b);
                drive.total_size = b.readUint32();
                drive.used_size = b.readUint32();

                drives.push(drive);
            }
            return drives;
        });
}

export function vfs_drive_format(path) {
    console.log("vfs_drive_format", path);
    return op_queue_push(0x11,
        b => { b.writeByte(path.charCodeAt(0)) },
        b => { });
}

export function vfs_read_folder(dir) {
    console.log("vfs_read_dir", dir);
    return op_queue_push(0x16,
        b => { write_string(b, dir); },
        bb => {
            var files = [];
            while (bb.remaining() > 0) {
                var file = {};
                file.name = read_string(bb);
                file.size = bb.readUint32();
                file.type = bb.readUint8();
                file.meta = read_meta(bb);
                files.push(file);
            }
            return files;
        });

}

export function vfs_create_folder(dir) {
    console.log("vfs_create_folder", dir);

    return op_queue_push(0x17,
        b => {
            path_validation(dir);
            write_string(b, dir);
        },
        b => { });
}


export function vfs_remove(path) {
    console.log("vfs_remove", path);

    return op_queue_push(0x18,
        b => { write_string(b, path); },
        b => { });
}

export function vfs_open_file(path, mode) {
    console.log("vfs_open_file", path, mode);

    return op_queue_push(0x12,
        b => {
            path_validation(path);
            write_string(b, path);
            if (mode == 'r') {
                b.writeUint8(0x8); //readonly
            } else if (mode == 'w') {
                b.writeUint8(0x16); //truc, create, write
            }

        },
        b => {
            return {
                file_id: b.readUint8()
            }
        });
}

export function vfs_close_file(file_id) {
    console.log("vfs_close_file", file_id);

    return op_queue_push(0x13,
        b => { b.writeUint8(file_id) },
        b => { });
}

export function vfs_read_file(file_id) {
    console.log("vfs_read_file", file_id);
    return op_queue_push(0x14,
        b => { b.writeUint8(file_id) },
        b => { return b.readBytes(b.remaining()).toArrayBuffer() });
}

export function vfs_write_file(file_id, data) {
    console.log("vfs_write_file", file_id);
    return op_queue_push(0x15,
        b => {
            b.writeUint8(file_id);
            write_bytes(b, data);
        },
        b => { });
}


export function vfs_update_meta(path, meta) {
    console.log("vfs_update_meta", path, meta);
    return op_queue_push(0x1a,
        b => {
            write_string(b, path);
            write_meta(b, meta);
        },
        b => { });
}

export function vfs_rename(old_path, new_path) {
    console.log("vfs_rename", old_path, new_path);
    return op_queue_push(0x19,
        b => {
            path_validation(old_path);
            path_validation(new_path);
            write_string(b, old_path);
            write_string(b, new_path);
        },
        b => { });
}

export function get_utf8_byte_size(str) {
    return encode_utf8(str).length;
}

export function vfs_helper_read_file(path, success_cb, error_cb, done_cb) {
    vfs_open_file(path, "r").then(res => {
        console.log(res)
        if (res.status != 0) {
            console.log("vfs_open_file error: status=", res.status);
            error_cb(new Error("create file failed!"));
            done_cb();
            return;
        }
        //读取

        var state = {
            file_id: res.data.file_id,
        }

        vfs_read_file(state.file_id).then(data => {
            console.log(data)
            console.log("vfs read end");
            vfs_close_file(state.file_id).then(data1 => {
                success_cb(data.data);
                done_cb();
            }).catch(e => {
                error_cb(e);
                done_cb();
            })
        }).catch(e => {
            console.log("vfs read error", e);
            vfs_close_file(state.file_id).then(data => {
                error_cb(e);
                done_cb();
            }).catch(e => {
                console.log("vfs close error", e);
                error_cb(e);
                done_cb();
            })
        });
    }).catch(e => {
        console.log("vfs_open_file error", e);
        error_cb(e);
        done_cb();
    });
}

var file_write_queue = []
var file_write_ongoing = false

export function vfs_helper_write_file(path, file, progress_cb, success_cb, error_cb) {
    file_write_queue.push({
        path: path,
        file: file,
        progress_cb: progress_cb,
        success_cb: success_cb,
        error_cb: error_cb
    });
    vfs_process_file_write_queue();
}

function vfs_process_file_write_queue() {
    if (!file_write_ongoing && file_write_queue.length > 0) {
        var e = file_write_queue.shift();
        file_write_ongoing = true;
        vfs_process_file_write(e.path, e.file, e.progress_cb, e.success_cb, e.error_cb, _ => {
            file_write_ongoing = false;
            console.log("vfs process file done", e.path);
            vfs_process_file_write_queue();
        });

    }
}

function vfs_process_file_write(path, file, progress_cb, success_cb, error_cb, done_cb) {
    console.log("vfs_process_file_write", path);
    read_file_as_bytebuffer(file).then(buffer => {
        vfs_open_file(path, "w").then(res => {
            if (res.status != 0) {
                console.log("vfs_open_file error: status=", res.status);
                error_cb(new Error("create file failed!"));
                done_cb();
                return;
            }
            //分批写入

            var state = {
                file: file,
                file_id: res.data.file_id,
                write_offset: 0,
                file_size: buffer.remaining(),
                batch_size: DF_MAX_DATA_SIZE - 1,
                data_buffer: buffer
            }

            function vfs_write_cb() {
                if (state.write_offset < state.file_size) {
                    //vfs write 
                    const batch_size = Math.min(state.batch_size,
                        state.file_size - state.write_offset);
                    const data_buffer = state.data_buffer.slice(state.write_offset, state.write_offset + batch_size);
                    console.log("vfs_write_cb", state.write_offset, state.file_size, batch_size);
                    vfs_write_file(state.file_id, data_buffer).then(data => {
                        state.write_offset += batch_size;
                        progress_cb({ written_bytes: state.write_offset, total_bytes: state.file_size }, state.file);
                        vfs_write_cb();
                    }).catch(e => {
                        console.log("vfs write error", e);
                        vfs_close_file(state.file_id).then(data => {
                            error_cb(e, state.file);
                            done_cb();
                        }).catch(e => {
                            console.log("vfs close error", e);
                            error_cb(e, state.file);
                            done_cb();
                        })
                    });
                } else {
                    console.log("vfs write end");
                    vfs_close_file(state.file_id).then(data => {
                        success_cb(state.file);
                        done_cb();
                    }).catch(e => {
                        error_cb(e, state.file);
                        done_cb();
                    })
                }
            }

            vfs_write_cb();
        }).catch(e => {
            console.log("vfs_open_file error", e);
            error_cb(e);
            done_cb();
        });
    });

}


function path_validation(path) {
    if (get_utf8_byte_size(path) > 63) {
        throw new Error("文件路径最大不能超过63个字节");
    }
    if (path.length > 3) {
        var p = path.lastIndexOf('/');
        var file_name = path.substring(p + 1);
        if (get_utf8_byte_size(file_name) > 47) {
            throw new Error("文件名最大不能超过47个字节");
        }
    }
}


function read_file_as_bytebuffer(file) {
    return new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.onload = function () {
            resolve(ByteBuffer.wrap(reader.result));
        }
        reader.onerror = function () {
            reject(reader.error);
        }

        reader.readAsArrayBuffer(file);
    });
}

function read_header(bb) {
    return {
        cmd: bb.readUint8(),
        status: bb.readUint8(),
        chunk: bb.readUint16()
    }
}

function read_meta(bb) {
    var size = bb.readUint8();
    var meta = {
        notes: "",
        flags: {
            hide: false
        }
    }
    if (size == 0) {
        return meta;
    }
    var mb = ByteBuffer.wrap(read_bytes_array(bb, size));
    while (mb.remaining() > 0) {
        var type = mb.readUint8(); //1 notes
        if (type == 1) {
            var type_size = mb.readUint8();
            if (type_size > 0) {
                var bytes = read_bytes_array(mb, type_size);
                if (bytes.length > 0) {
                    meta["notes"] = decode_utf8(bytes);
                }
            }
        } else if (type == 2) {
            var flags = mb.readUint8();
            if (flags & 1) {
                meta.flags.hide = true;
            }
        }
    }

    return meta;
}

function write_meta(bb, meta) {
    var notes = meta.notes;
    var bytes = encode_utf8(notes);

    if (bytes.length > 90) {
        throw new Error("备注最大只能是90字节，即90个字符或30个汉字！（当前" + bytes.length + "字节）")
    }

    var tb = new ByteBuffer();
    //notes
    if (notes.length > 0) {
        tb.writeUint8(1);//amiibo notes
        tb.writeUint8(bytes.length);
        for (var i = 0; i < bytes.length; i++) {
            tb.writeUint8(bytes[i]);
        }
    }

    //flags
    tb.writeUint8(2);
    var flags = 0;
    if (meta.flags.hide) {
        flags |= 1;
    }
    tb.writeUint8(flags);
    tb.flip();

    bb.writeUint8(tb.remaining());
    write_bytes(bb, tb);
}


function decode_utf8(bytes) {
    //return ByteBuffer.wrap(bytes).toUTF8();
    return new TextDecoder().decode(new Uint8Array(bytes));
}

function encode_utf8(text) {
    //return Array.from(new Uint8Array(ByteBuffer.wrap(text, 'utf8').toArrayBuffer()));
    return Array.from(new TextEncoder().encode(text));
}

function read_string(bb) {
    var size = bb.readUint16();
    var bytes = []
    for (var i = 0; i < size; i++) {
        bytes.push(bb.readUint8());
    }
    return decode_utf8(bytes);
}

function write_string(bb, str) {
    var bytes = encode_utf8(str);
    bb.writeUint16(bytes.length);
    for (var i = 0; i < bytes.length; i++) {
        bb.writeUint8(bytes[i]);
    }
}

function write_bytes(bb, buffer) {
    var size = buffer.remaining();
    for (var i = 0; i < size; i++) {
        bb.writeUint8(buffer.readUint8());
    }
}

function read_bytes_array(bb, size) {
    var bytes = []
    for (var i = 0; i < size; i++) {
        bytes.push(bb.readUint8());
    }
    return bytes;
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
    return pixl.tx_data(bb.toArrayBuffer());
}



function new_rx_promise() {
    return new Promise((resolve, reject) => {
        m_api_reject = reject;
        m_api_resolve = resolve;
    });
}


var rx_bytebuffer = new ByteBuffer();
var rx_chunk_state = "NONE"; //NONE CHUNK,


function on_rx_data(data) {
    var buff = ByteBuffer.wrap(data);
    var h = read_header(buff);
    if (h.chunk & 0x8000) {
        if (rx_chunk_state == "NONE") {
            write_bytes(rx_bytebuffer, ByteBuffer.wrap(data));
            rx_chunk_state = "CHUNK";
        } else if (rx_chunk_state == "CHUNK") {
            write_bytes(rx_bytebuffer, buff); //next chunk, ignore header
        }
    } else {
        var cb_data = data;
        if (rx_chunk_state == "CHUNK") { //end of chunk
            write_bytes(rx_bytebuffer, buff);
            rx_bytebuffer.flip();
            cb_data = rx_bytebuffer.toArrayBuffer();
        } else if (rx_chunk_state == "NONE") { //single chunk
            cb_data = data;
        }

        //call back 
        if (m_api_resolve) {
            m_api_resolve(cb_data);
            m_api_resolve = null;

        }
        rx_chunk_state = "NONE";
    }
}


function on_ble_disconnected() {
    rx_bytebuffer.clear();
    rx_chunk_state = "NONE";

    m_api_resolve = null;
    m_api_reject = null;

    file_write_queue = [];
    file_write_ongoing = false;

    op_queue = [];
    op_ongoing = false;

}

