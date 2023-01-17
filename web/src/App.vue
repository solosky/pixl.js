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
            <el-button size="mini" icon="el-icon-plus">新建文件夹</el-button>
            <el-button size="mini" type="danger" icon="el-icon-delete">删除</el-button>
          </el-button-group>
          <el-button-group>
            <el-button size="mini" icon="el-icon-top" @click="on_btn_up">上一级</el-button>
            <el-button size="mini" icon="el-icon-refresh">刷新</el-button>
          </el-button-group>
        </div>
      </el-col>
      <el-col :span="8">
        <div class="action-right">
          <el-button type="success" size="mini" v-if="version" icon="el-icon-warning">{{ version }}</el-button>
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
            <el-breadcrumb-item :to="{ path: '/' }"></el-breadcrumb-item>
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
            <i class="el-icon-folder"></i>
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
            <div class="folder-action">
              <el-button size="mini" icon="el-icon-edit-outline"
                @click="handleDelete(scope.$index, scope.row)"></el-button>
              <el-button size="mini" type="danger" icon="el-icon-delete"
                @click="handleDelete(scope.$index, scope.row)"></el-button>
            </div>
          </template>
        </el-table-column>
      </el-table>
    </div>

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
      current_dir: ""
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

      this.table_loading = true;

      proto.get_version().then(version => {
        this.version = "已连接, ver: " + version.ver;

        //get drive list 
        var thiz = this;

        proto.vfs_get_drive_list().then(data => {
          console.log(data);
          var _table_data = [];
          for (var i in data) {
            var drive = data[i];
            var row = {
              name: drive.label + ":/ [" + drive.name + "]",
              size: thiz.format_size(drive.used_size) + "/" + thiz.format_size(drive.total_size),
              type: "DRIVE"
            };
            _table_data.push(row)
          }
          thiz.tableData = _table_data;
          thiz.table_loading = false;
        });
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

    on_btn_up(){

        var idx = this.current_dir.lastIndexOf('/');
        if(idx > 2){ // E:/
          this.current_dir = this.current_dir.substring(0, idx);
          this.reload_folder();
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
              type: file.type == 0 ? "REG" : "DIR"
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
