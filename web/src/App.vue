<template>
  <div id="app">
    <h1 class="header">Pixl.js</h1>
    <el-row>
      <el-col :span="16">
        <div class="action-left">
          <el-button-group>
            <el-button size="mini" type="primary" icon="el-icon-upload">上传</el-button>
            <el-button size="mini" icon="el-icon-download">下载</el-button>
          </el-button-group>
          <el-button-group>
            <el-button size="mini" icon="el-icon-plus" @click="on_btn_new_folder">新建文件夹</el-button>
            <el-button size="mini" type="danger" icon="el-icon-delete">删除</el-button>
          </el-button-group>
          <el-button-group>
            <el-button size="mini" icon="el-icon-top" @click="on_btn_up">上级目录</el-button>
            <el-button size="mini" icon="el-icon-refresh" @click="on_btn_refresh">刷新</el-button>
          </el-button-group>
        </div>
      </el-col>
      <el-col :span="8">
        <div class="action-right">
          <el-button type="success" size="mini" v-if="version" icon="el-icon-warning">{{ version }}</el-button>
          <el-popover placement="bottom" title="队列" width="400" trigger="hover">
              <el-table :data="queue_data">
                <el-table-column width="200" property="file" label="文件"></el-table-column>
                <el-table-column width="100" label="进度">
                  <template slot-scope="scope">
                    <el-progress type="circle" :percentage="scope.row.progress" status="success" :width="30"></el-progress>
                  </template>

                </el-table-column>
                <el-table-column width="100" label="操作">
                  <template  slot-scope="scope">
                    <el-button size="mini" type="danger" icon="el-icon-close"></el-button>
                  </template>

                </el-table-column>
              </el-table>
              <el-badge :value="12" class="item" slot="reference">
                <el-button size="mini" icon="el-icon-loading">队列</el-button>
              </el-badge>
            </el-popover>
          <el-button-group>
            <el-button type="info" size="mini" icon="el-icon-cpu" @click="on_btn_enter_dfu">DFU</el-button>
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
      <el-table ref="multipleTable" :data="tableData" tooltip-effect="dark" style="width: 100%"
        v-loading="table_loading" element-loading-text="加载中.." element-loading-spinner="el-icon-loading"
        cell-class-name="file-cell">
        <el-table-column type="selection" width="55">
        </el-table-column>
        <el-table-column label="文件">
          <template slot-scope="scope">
            <i :class="scope.row.icon"></i>
            <el-link :underline="false" @click="handle_name_click(scope.$index, scope.row)">{{
              scope.row.name
            }}</el-link>
          </template>
        </el-table-column>
        <el-table-column prop="size" label="大小">
        </el-table-column>
        <el-table-column prop="type" label="类型">
        </el-table-column>
        <el-table-column label="">
          <template slot-scope="scope">
            <el-dropdown>
              <span class="el-dropdown-link">
                <i class="el-icon-arrow-down el-icon--right"></i>
              </span>
              <el-dropdown-menu slot="dropdown">
                <el-dropdown-item @click.native="on_row_btn_remove(scope.$index, scope.row)">删除</el-dropdown-item>
                <el-dropdown-item>重命名..</el-dropdown-item>
              </el-dropdown-menu>
            </el-dropdown>
          </template>
        </el-table-column>
      </el-table>
    </div>

    <el-dialog title="新建" :visible.sync="name_diag_visible" width="30%">
      <el-input v-model="input_name" placeholder="请输入文件名"></el-input>
      <span slot="footer" class="dialog-footer">
        <el-button @click="name_diag_visible = false">取 消</el-button>
        <el-button type="primary" @click="on_diag_name_close">确 定</el-button>
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
      multipleSelection: [],
      connBtnType: "",
      connBtnText: "连接",
      version: "",
      state: "disconnected",
      table_loading: false,
      current_dir: "",
      name_diag_visible: false,
      input_name: "",
      queue_data: [{
        file: "zelda.bin",
        progress: 50
      },
    {
        file: "mifa.bin",
        progress: 80
      }]
    }
  },
  methods: {
    on_btn_ble_connect() {
      if (this.state == "connected") {
        pixl.disconnect();
        this.connBtnText = "连接";
      } else {
        this.connBtnText = "连接中..";
        pixl.connect();
      }
    },
    on_ble_connected() {
      this.connBtnText = "断开";
      this.connBtnType = "success";
      this.state = "connected";
      this.$notify({
        title: 'Pixl.js',
        type: 'success',
        message: '已成功连接到Pixl.js!',
        duration: 5000
      });

      proto.get_version().then(version => {
        this.version = "已连接, ver: " + version.ver;
        this.reload_drive();
      });
    },
    on_ble_disconnected() {
      this.connBtnType = "";
      this.state = "diconnected";
      this.connBtnText = "连接";
      this.version = "";
      this.$notify({
        title: 'Pixl.js',
        type: 'error',
        message: 'Pixl.js已经断开连接!',
        duration: 5000
      });

    },
    on_ble_connect_error() {
      this.connBtnType = "";
      this.connBtnText = "连接";
      this.version = "";
      this.$notify({
        title: 'Pixl.js',
        type: 'error',
        message: 'Pixl.js连接失败!',
        duration: 5000
      });
    },

    on_btn_enter_dfu() {
      if (this.state == "connected") {

        this.$confirm('是否进入DFU模式?', '提示', {
          confirmButtonText: '确定',
          cancelButtonText: '取消',
          type: 'warning'
        }).then(() => {
          proto.enter_dfu().then(data => {
            this.$message({
              type: 'success',
              message: '进入DFU模式成功!'
            });
          });
        });
      }
    },

    on_btn_up() {

      var drive = this.current_dir.substring(0, 2); //E:
      var path = this.current_dir.substring(2);

      if (path == '') { // root
        this.current_dir = "";
        this.reload_drive();
      } else {
        var idx = path.lastIndexOf('/');
        this.current_dir = drive + path.substring(0, idx);
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
      this.name_diag_visible = true;
    },

    on_row_btn_remove(index, row) {
      this.$confirm('是否删除 ' + row.name + '?', '提示', {
        confirmButtonText: '确定',
        cancelButtonText: '取消',
        type: 'warning'
      }).then(() => {

        var path = this.current_dir + '/' + row.name;

        proto.vfs_remove(path).then(data => {
          this.$message({
            type: 'success',
            message: '删除文件成功!'
          });

          this.reload_folder();
        });
      });
    },

    on_diag_name_close() {
      if (this.input_name.length > 0) {
        var dir = this.current_dir + "/" + this.input_name;
        proto.vfs_create_folder(dir).then(data => {
          if (data.status == 0) {
            this.name_diag_visible = false;
            this.$message({
              message: '创建文件夹成功！',
              type: 'success'
            });
            this.reload_folder();
          } else {
            this.$message({
              message: '创建文件夹失败！',
              type: 'error'
            });
          }
        });
      }
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
      }
    },

    reload_drive() {
      this.table_loading = true;
      var thiz = this;
      proto.vfs_get_drive_list().then(data => {
        console.log(data);
        var _table_data = [];
        for (var i in data) {
          var drive = data[i];
          var row = {
            name: drive.label + ":/ [" + drive.name + "]",
            size: thiz.format_size(drive.used_size) + "/" + thiz.format_size(drive.total_size),
            type: "DRIVE",
            icon: "el-icon-box"
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

        if (h.status == 0) {
          var _table_data = [];
          for (var i in h.data) {
            var file = h.data[i];

            var row = {
              name: file.name,
              size: thiz.format_size(file.size),
              type: file.type == 0 ? "REG" : "DIR",
              icon: file.type == 0 ? "el-icon-document" : "el-icon-folder"
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
    }

  }, mounted() {
    var dispatcher = sharedEventDispatcher();
    dispatcher.addListener("ble_connected", this.on_ble_connected);
    dispatcher.addListener("ble_disconnected", this.on_ble_disconnected);
    dispatcher.addListener("ble_connect_error", this.on_ble_connect_error);

    proto.init();
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
