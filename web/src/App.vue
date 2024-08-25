<template>

  <div id="app">
    <div class="action-right">
      <el-dropdown trigger="click" class="international" @command="handle_set_language">
        <div>
          <el-button size="mini" type="primary" icon="el-icon-setting" @click="dialogVisible = true" >{{ $t('lang.choose') }}</el-button>
        </div>
        <el-dropdown-menu slot="dropdown">
          <el-dropdown-item Enabled="language==='zh_CN'" command="zh_CN" divided>
            {{ $t('lang.zhcn') }}
          </el-dropdown-item>
          <el-dropdown-item Enabled="language==='zh_TW'" command="zh_TW" divided>
            {{ $t('lang.zhtw') }}
          </el-dropdown-item>
          <el-dropdown-item Enabled="language==='en'" command="en" divided>
            {{ $t('lang.en') }}
          </el-dropdown-item>
          <el-dropdown-item Enabled="language==='es'" command="es" divided>
            {{ $t('lang.es') }}
          </el-dropdown-item>
          <el-dropdown-item Enabled="language==='ru_RU'" command="ru_RU" divided>
            {{ $t('lang.ru') }}
          </el-dropdown-item>
          <el-dropdown-item Enabled="language==='de'" command="de" divided>
            {{ $t('lang.de') }}
          </el-dropdown-item>
        </el-dropdown-menu>
      </el-dropdown>
    </div>
    <h1 class="header">Pixl.js</h1>
    <el-row>
      <el-col :span="14">
        <div class="action-left">
          <el-button-group>
            <el-button size="mini" type="primary" icon="el-icon-upload" @click="on_btn_upload"
              :disabled="btn_disabled()">{{ $t('menu.upload')  }}</el-button>
          </el-button-group>
          <el-button-group>
            <el-button size="mini" icon="el-icon-plus" @click="on_btn_new_folder"
              :disabled="btn_disabled()">{{ $t('menu.newfolder')  }}</el-button>
            <el-button size="mini" type="danger" icon="el-icon-delete" @click="on_btn_remove"
              :disabled="btn_disabled()">{{ $t('menu.del')  }}</el-button>
          </el-button-group>
          <el-button-group>
            <el-button size="mini" icon="el-icon-top" @click="on_btn_up" :disabled="btn_disabled()">{{ $t('menu.up')  }}</el-button>
            <el-button size="mini" icon="el-icon-refresh" @click="on_btn_refresh" :disabled="!connected">{{ $t('menu.refresh') }}</el-button>
          </el-button-group>
        </div>
      </el-col>
      <el-col :span="10">
        <div class="action-right">
          <el-button type="success" size="mini" v-if="version" icon="el-icon-warning">{{ version }}</el-button>
          <el-button-group>
            <el-button type="info" size="mini" icon="el-icon-cpu" @click="on_btn_enter_dfu"
              :disabled="!connected">{{ $t('menu.dfu')  }}</el-button>
            <el-button :type="connBtnType" size="mini" icon="el-icon-connection" @click="on_btn_ble_connect">{{
              connBtnText
            }}</el-button>
          </el-button-group>
        </div>
      </el-col>
    </el-row>
    <el-row>
      <el-col :span="24">
        <div class="folder-path">
          <el-breadcrumb separator="/">
            <el-breadcrumb-item>{{ current_dir }}</el-breadcrumb-item>
          </el-breadcrumb>
        </div>
      </el-col>
    </el-row>
    <div>
      <el-table ref="multipleTable" :data="tableData" tooltip-effect="dark" style="width: 100%" v-loading="table_loading"
        :element-loading-text="$t('status.loading')" element-loading-spinner="el-icon-loading" cell-class-name="file-cell"
        @selection-change="on_table_selection_change" @sort-change="on_table_sort_change"
        :default-sort="{ prop: 'name', order: 'ascending' }">
        <el-table-column type="selection" width="42">
        </el-table-column>
        <el-table-column prop="name" :label="$t('labels.name')" sortable @sort-method="sort_table_row_name" width="320">
          <template slot-scope="scope">
            <i :class="scope.row.icon"></i>
            <el-link :underline="false" @click="handle_name_click(scope.$index, scope.row)">
              {{ scope.row.name }}</el-link>
          </template>
        </el-table-column>
        <el-table-column prop="size" :label="$t('labels.size')" sortable width="150">
        </el-table-column>
        <el-table-column prop="type" :label="$t('labels.type')" sortable width="80">
        </el-table-column>
        <el-table-column prop="notes" :label="$t('labels.remark')" sortable>
        </el-table-column>
        <el-table-column label="" fixed="right" width="40">
          <template slot-scope="scope">
            <el-dropdown>
              <span class="el-dropdown-link">
                <i class="el-icon-arrow-down el-icon--right"></i>
              </span>
              <el-dropdown-menu slot="dropdown">
                <el-dropdown-item @click.native="on_row_btn_remove(scope.$index, scope.row)"
                  v-if="scope.row.type != 'DRIVE'">{{ $t('contxmenu.del') }}</el-dropdown-item>
                <el-dropdown-item @click.native="on_row_btn_rename(scope.$index, scope.row)"
                  v-if="scope.row.type != 'DRIVE'">{{ $t('contxmenu.rename') }}</el-dropdown-item>
                <!-- <el-dropdown-item @click.native="on_row_btn_notes(scope.$index, scope.row)"
                  v-if="scope.row.type != 'DRIVE'">{{ $t('contxmenu.prop') }}</el-dropdown-item> -->
                <el-dropdown-item @click.native="on_row_btn_meta(scope.$index, scope.row)"
                  v-if="scope.row.type != 'DRIVE'">{{ $t('contxmenu.prop') }}</el-dropdown-item>
                <el-dropdown-item @click.native="on_row_btn_format(scope.$index, scope.row)"
                  v-if="scope.row.type == 'DRIVE'">{{ $t('contxmenu.format') }}</el-dropdown-item>
              </el-dropdown-menu>
            </el-dropdown>
          </template>
        </el-table-column>
      </el-table>
    </div>

    <el-dialog :title="$t('properties.title')" :visible.sync="meta_diag_visible" width="30%">
      <el-form ref="form" :model="meta_form" label-width="80px">
        <el-form-item :label="$t('properties.remark')">
          <el-input v-model="meta_form.notes"></el-input>
        </el-form-item>
        <el-form-item :label="$t('properties.attrib')">
          <el-checkbox :label="$t('properties.hide')" v-model="meta_form.flags.hide"></el-checkbox>
          <el-checkbox :label="$t('properties.readonly')" v-model="meta_form.flags.readonly"></el-checkbox>
        </el-form-item>
      </el-form>
      <span slot="footer" class="dialog-footer">
        <el-button @click="meta_diag_visible = false">{{ $t('btn.cancel') }}</el-button>
        <el-button type="primary" @click="on_diag_meta_close">{{ $t('btn.ok') }}</el-button>
      </span>
    </el-dialog>

    <el-dialog :title="$t('upload.title')" :visible.sync="upload_diag_visible" width="30%" :before-close="on_upload_diag_close">
      <div>
        <el-upload ref="upload" class="upload-demo" drag action="https://jsonplaceholder.typicode.com/posts/" multiple
          :http-request="on_upload_request" :on-error="on_upload_error">
          <i class="el-icon-upload"></i>
          <div class="el-upload__text">{{ $t('upload.drag') }} <em>{{ $t('upload.click') }}</em></div>
          <div class="el-upload__tip" slot="tip">
            <ul>
              <li>{{ $t('upload.maxsize') }}</li>
              <li>{{ $t('upload.maxname') }}</li>
            </ul>
          </div>
        </el-upload>
      </div>
      <span slot="footer" class="dialog-footer">
      </span>
    </el-dialog>

  </div>
</template>



<script>
import * as pixl from "./lib/pixl.ble"
import { sharedEventDispatcher } from "./lib/event"
import * as proto from "./lib/pixl.proto"



export default {
  data() {
    return {
      tableData: [],
      connBtnType: "",
      connBtnText: `${this.$t("conn.connect")}`,
      version: "",
      connected: false,
      table_loading: false,
      current_dir: "",
      upload_diag_visible: false,
      table_selection: [],
      language: "zh_CN",
      meta_diag_visible: false,
      meta_form: {
        notes: "",
        flags: {
          hide: false
        },
        amiibo: {
          head: 0,
          tail: 0
        },
        name: "",
        row: null
      }
    }
  },

  methods: {
    on_btn_ble_connect() {
      if (this.connected) {
        pixl.disconnect();
        this.connBtnText = `${this.$t("conn.connect")}`;
      } else {
        this.connBtnText = `${this.$t("conn.connecting")}`;
        pixl.connect();
      }
    },
    on_ble_connected() {
      this.connBtnText = `${this.$t("conn.disconnect")}`;
      this.connBtnType = "success";
      this.connected = true;
      this.$notify({
        title: 'Pixl.js',
        type: 'success',
        message: `${this.$t("conn.consuccess")}`,
        duration: 5000
      });

      proto.get_version().then(res => {
        console.log("get version result", res);
        this.version = `${this.$t("status.connected")}` + res.data.ver;

        if (res.data.ver.startsWith("2.0.0")) {
          this.$alert(`${this.$t("oldfirm.message")}`, `${this.$t("oldfirm.title")}`, {
            confirmButtonText: `${this.$t("btn.ok")}`,
            callback: action => {
            }
          });
        }

        var v = res.data;

        LA.track("pixl_device_connect", {"version": v.ver, "mac": v.ble_addr});

        this.reload_drive();
      });
    },
    on_ble_disconnected() {
      this.connBtnType = "";
      this.connected = false;
      this.connBtnText = `${this.$t("conn.connect")}`;
      this.version = "";
      this.table_loading = false;
      this.tableData = [];
      this.current_dir = "";
      this.$notify({
        title: 'Pixl.js',
        type: 'error',
        message: `${this.$t("conn.disconnected")}`,
        duration: 5000
      });


    },
    on_ble_connect_error() {
      this.connBtnType = "";
      this.connBtnText = `${this.$t("conn.connect")}`;
      this.version = "";
      this.table_loading = false;
      this.tableData = [];
      this.current_dir = "";
      this.$notify({
        title: 'Pixl.js',
        type: 'error',
        message: `${this.$t("conn.connfailed")}`,
        duration: 5000
      });
    },

    on_btn_enter_dfu() {
      this.$confirm(`${this.$t("dfumode.startconfirm")}`, `${this.$t("dfumode.title")}`, {
        confirmButtonText: `${this.$t("btn.ok")}`,
        cancelButtonText: `${this.$t("btn.cancel")}` ,
        type: 'warning'
      }).then(() => {
        proto.enter_dfu().then(data => {

          this.$confirm(`${this.$t("dfumode.updateconfirm")}`, `${this.$t("dfumode.updatetitle")}`, {
            confirmButtonText: `${this.$t("btn.ok")}`,
            cancelButtonText: `${this.$t("btn.cancel")}`,
            type: 'success'
          }).then(_ => {
            document.location.href = "https://thegecko.github.io/web-bluetooth-dfu/examples/web.html";
          });
        });
      })
    },

    on_btn_up() {

      var drive = this.current_dir.substring(0, 2); //E:
      var path = this.current_dir.substring(2);

      if (path == '/') { // root
        this.current_dir = "";
        this.reload_drive();
      } else {
        var idx = path.lastIndexOf('/');
        if (idx == 0) {
          this.current_dir = drive + "/";
        } else {
          this.current_dir = drive + path.substring(0, idx);
        }

        this.reload_folder();
      }
    },

    on_btn_refresh() {
      if (this.current_dir == '') {
        this.reload_drive();
      } else {
        this.reload_folder();
      }
    },

    on_btn_new_folder() {
      var thiz = this;
      this.$prompt(`${this.$t("newfolder.message")}`, `${this.$t("newfolder.title")}`, {
        confirmButtonText: `${this.$t("btn.ok")}`,
        cancelButtonText: `${this.$t("btn.cancel")}`,
        inputValue: ""
      }).then(({ value }) => {
        if (value == "") {
          return;
        }
        thiz.table_loading = true;
        var path = this.append_segment(this.current_dir, value);
        proto.vfs_create_folder(path).then(res => {
          thiz.table_loading = false;
          if (res.status == 0) {
            this.reload_folder();
          } else {
            this.$message({
              type: 'error',
              message: `${this.$t("newfolder.newfoldererr")}` + " [" + res.status + "]"
            });
          }
        }).catch(e => {
          thiz.table_loading = false;
          this.$message({
            type: 'error',
            message: `${this.$t("newfolder.newfoldererr")}` + " [" + e.message + "]"
          });
        });

      }).catch(() => {
        //ignore     
      });
    },

    on_btn_upload() {
      this.upload_diag_visible = true;
    },

    on_btn_remove() {
      if (this.table_selection.length == 0) {
        return;
      }
      var thiz = this;
      const dir = this.current_dir;
      var proceed_count = 0;
      var total_count = this.table_selection.length;
      thiz.table_loading = true;
      this.table_selection.forEach(v => {
        proto.vfs_remove(this.append_segment(dir, v.name)).then(_ => {
          this.delete_table_row_by_name(v.name);
          proceed_count++;
          if (proceed_count == total_count) {
            thiz.table_loading = false;
          }
        }).catch(e => {
          this.$message({
            type: 'error',
            message: v.name + `${this.$t("del.error")}` + e
          });
          proceed_count++;
          if (proceed_count == total_count) {
            thiz.table_loading = false;
          }
        });
      }, this);
    },


    on_upload_diag_close(done) {
      this.$confirm(`${this.$t("upload.closemessage")}`, `${this.$t("upload.closetitle")}`,{
        confirmButtonText: `${this.$t("btn.ok")}`,
        cancelButtonText: `${this.$t("btn.cancel")}`,
      })
      .then(_ => {
        this.$refs.upload.clearFiles();
        this.reload_folder();
        done();
      })
      .catch(_ => { });
    },

    on_upload_request(options) {
      console.log(options);
      proto.vfs_helper_write_file(this.append_segment(this.current_dir, options.file.name), options.file, p => {
        options.onProgress({ percent: p.written_bytes / p.total_bytes * 100 });
      }, _ => {
        options.onSuccess()
      }, e => {
        options.onError(e)
      });
    },

    on_upload_error(err, file, filelist) {
      this.$message({
        type: 'error',
        message: file.name + `${this.$t("upload.errupload")}` + err
      });
    },

    on_row_btn_format(index, row) {
      var thiz = this;
      this.$confirm(`${this.$t("format.messrow1a")}` + row.name + `${this.$t("format.messrow1b")}` + "\n" + `${this.$t("format.messrow2")}` + '\n' +`${this.$t("format.messrow3")}`, `${this.$t("format.title")}`, {
        confirmButtonText: `${this.$t("btn.ok")}`,
        cancelButtonText: `${this.$t("btn.cancel")}`,
        type: 'warning'
      }).then(() => {
        thiz.table_loading = true;
        var path = row.name.substr(0, 1);
        proto.vfs_drive_format(path).then(data => {
          this.$message({
            type: 'success',
            message: row.name + `${this.$t("format.formatok")}`
          });
          thiz.table_loading = false;
          this.reload_drive();
        }).catch(e => {
          this.$message({
            type: 'error',
            message: row.name + `${this.$t("format.formaterr")}` + err
          });
          thiz.table_loading = false;
        });
      });
    },

    on_row_btn_remove(index, row) {
      var thiz = this;
      this.$confirm(`${this.$t("del.message")}` + row.name + `${this.$t("del.messageend")}`, `${this.$t("del.title")}`, {
        confirmButtonText: `${this.$t("btn.ok")}`,
        cancelButtonText: `${this.$t("btn.cancel")}`,
        type: 'warning'
      }).then(() => {
        thiz.table_loading = true;
        var path = this.append_segment(this.current_dir, row.name);
        proto.vfs_remove(path).then(data => {
          thiz.table_loading = false;
          if (data.status == 0) {
            this.$message({
              type: 'success',
              message: `${this.$t("del.deleteok")}`
            });

            this.reload_folder();
          } else {
            this.$message({
              type: 'error',
              message: row.name + `${this.$t("del.error")}` + "[" + data.status + "]"
            });
          }
        }).catch(e => {
          this.$message({
            type: 'error',
            message: row.name + `${this.$t("del.error")}` + "[" + err + "]"
          });
          thiz.table_loading = false;
        });
      });
    },

    on_row_btn_notes(index, row) {
      var thiz = this;
      this.$prompt(`${this.$t("properties.entermsg")}`, `${this.$t("properties.title")}`, {
        confirmButtonText: `${this.$t("btn.ok")}`,
        cancelButtonText: `${this.$t("btn.cancel")}`,
        inputValue: row.notes
      }).then(({ value }) => {
        var meta = {
          notes: value
        };


        var path = this.append_segment(this.current_dir, row.name);
        proto.vfs_update_meta(path, meta).then(res => {
          if (res.status == 0) {
            row.notes = value;
          } else {
            this.$message({
              type: 'error',
              message: `${this.$t("properties.errupdate")}`
            });
          }
        }).catch(e => {
          this.$message({
            type: 'error',
            message: e.message
          });
        });

      }).catch(() => {
        //ignore     
      });
    },

    on_row_btn_rename(index, row) {
      var thiz = this;
      this.$prompt(`${this.$t("rename.message")}`, `${this.$t("rename.title")}`, {
        confirmButtonText: `${this.$t("btn.ok")}`,
        cancelButtonText: `${this.$t("btn.cancel")}`,
        inputValue: row.name
      }).then(({ value }) => {
        if (value == row.name) {
          return;
        }
        thiz.table_loading = true;
        var path_old = this.append_segment(this.current_dir, row.name);
        var path_new = this.append_segment(this.current_dir, value);
        proto.vfs_rename(path_old, path_new).then(res => {
          thiz.table_loading = false;
          if (res.status == 0) {
            row.name = value;
          } else {
            this.$message({
              type: 'error',
              message: `${this.$t("rename.errrename")}` + " [" + res.status + "]"
            });
          }
        }).catch(e => {
          thiz.table_loading = false;
          this.$message({
            type: 'error',
            message: `${this.$t("rename.errrename")}` + " [" + e.message + "]"
          });
        });

      }).catch(() => {
        //ignore     
      });
    },


    on_row_btn_meta(index, row) {
      this.meta_form.name = row.name;
      this.meta_form.notes = row.notes;
      this.meta_form.flags = row.flags;
      this.meta_form.amiibo = row.amiibo;
      this.meta_form.row = row;
      this.meta_diag_visible = true;
    },

    on_diag_meta_close() {
      var meta = {
        notes: this.meta_form.notes,
        flags: this.meta_form.flags,
        amiibo: this.meta_form.amiibo
      }
      this.meta_diag_visible = false;
      var path = this.append_segment(this.current_dir, this.meta_form.name);
      var meta_form = this.meta_form;
      proto.vfs_update_meta(path, meta).then(res => {
        if (res.status == 0) {
          meta_form.row.notes = meta_form.notes;
          meta_form.row.flags = meta_form.flags;
          meta_form.row.amiibo = meta_form.amiibo;
       } else {
          this.$message({
            type: 'error',
            message: `${this.$t("properties.errupdate")}`
          });
        }
      }).catch(e => {
        this.$message({
          type: 'error',
          message: e.message
        });
      });
    },

    on_table_selection_change(selected) {
      this.table_selection = selected;
    },

    on_table_sort_change(column, prop, order) {
      console.log("sort change: ", column, prop, order);
    },

    sort_table_row_name(a, b) {
      console.log(a, b); //not working
      return a < b ? 1 : -1;
    },

    handle_name_click(index, row) {
      if (row.type == "DRIVE") {
        this.current_dir = row.name.substr(0, 3);
        this.reload_folder();
      } else if (row.type == "DIR") {
        if (this.current_dir.charAt(this.current_dir.length - 1) != '/') {
          this.current_dir = this.current_dir + "/";
        }
        this.current_dir = this.current_dir + row.name;
        this.reload_folder();
      } else {
        proto.vfs_helper_read_file(this.append_segment(this.current_dir, row.name), _ => {
          var url = window.URL.createObjectURL(new Blob([_], { type: 'application/octet-stream' }));
          // window.open(url);
          var downloadLink = document.createElement("a");
          downloadLink.style.display = 'none';
          downloadLink.href = url;
          downloadLink.download = row.name;

          document.body.appendChild(downloadLink);
          downloadLink.click();
          document.body.removeChild(downloadLink);
          window.URL.revokeObjectURL(url);
        }, _ => { }, _ => { });
      }
    },

    reload_drive() {
      this.table_loading = true;
      var thiz = this;
      proto.vfs_get_drive_list().then(res => {
        console.log(res);
        const data = res.data
        var _table_data = [];
        for (var i in data) {
          var drive = data[i];
          var row = {
            name: drive.label + ":/ [" + drive.name + "]",
            size: drive.status == 0 ? thiz.format_size(drive.used_size) + "/" + thiz.format_size(drive.total_size) : `${this.$t("drive.message")}` + drive.status + `${this.$t("drive.messageend")}`,
            type: "DRIVE",
            icon: "el-icon-box",
            notes: ""
          };
          _table_data.push(row)
        }
        thiz.tableData = _table_data;
        thiz.table_loading = false;
      });
    },

    reload_folder() {
      this.table_loading = true;
      var thiz = this;
      proto.vfs_read_folder(this.current_dir).then(h => {
        thiz.table_loading = false;
        console.log(h);

        if (h.status == 0) {
          var _table_data = [];
          for (var i in h.data) {
            var file = h.data[i];

            var row = {
              name: file.name,
              size: thiz.format_size(file.size),
              type: file.type == 0 ? "REG" : "DIR",
              icon: file.type == 0 ? "el-icon-document" : "el-icon-folder",
              notes: file.meta.notes,
              flags: file.meta.flags,
              amiibo: file.meta.amiibo
            };

            _table_data.push(row);
          }
          thiz.tableData = _table_data;
        }


      });
    },

    format_size(size) {
      if (typeof size == 'number') {
        if (size < 1024) {
          return size + " B"
        } else if (size < 1024 * 1024) {
          return (size / 1024).toFixed(2) + " KB";
        } else {
          return (size / 1024 / 1024).toFixed(2) + " MB";
        }
      } else {
        return size;
      }
    },

    btn_disabled() {
      return !this.connected || this.current_dir == '';
    },

    append_segment(dir, seg) {
      var drive = dir.substring(0, 2); //E:
      var path = dir.substring(2);
      if (path == '/') {
        return dir + seg;
      } else {
        return dir + '/' + seg;
      }
    },

    delete_table_row_by_name(name) {
      for (var i = 0; i < this.tableData.length; i++) {
        if (this.tableData[i].name == name) {
          this.tableData.splice(i, 1);
          return;
        }
      }
    },

    handle_set_language(lang) {
      this.$i18n.locale = lang
      this.$cookie.set("lang", lang, 60 * 60 * 24 * 365)
      if (this.connected) {
        this.connBtnText = `${this.$t("conn.disconnect")}`;
        proto.get_version().then(res => {
          console.log("get version result", res);
          this.version = `${this.$t("status.connected")}` + res.data.ver;
        });
      } else {
        this.connBtnText = `${this.$t("conn.connect")}`;
      }
      this.$message({
        message: `${this.$t("lang.changeok")}`,
        type: 'success'
      })
    }


  }, mounted() {
    var dispatcher = sharedEventDispatcher();
    dispatcher.addListener("ble_connected", this.on_ble_connected);
    dispatcher.addListener("ble_disconnected", this.on_ble_disconnected);
    dispatcher.addListener("ble_connect_error", this.on_ble_connect_error);

    proto.init();

    var lang = this.$cookie.get("lang")
    if (!lang) {
      lang = "zh_CN"
    }

    this.language = lang
    this.$i18n.locale = lang
    this.connBtnText = `${this.$t("conn.connect")}`;
  },
}
</script>

<style>
#app {
  font-family: Helvetica, sans-serif;
  width: 960px;
  margin: 0 auto;
}

.action-left {
  float: left;
}

.action-right {
  float: right;
}

.folder-path {
  margin: 20px auto;
}

.folder-action {
  float: right;
}

.file-cell {
  padding: 4px 0 !important;
}

.header {
  text-align: center;
  margin: 100px auto;
}
</style>
