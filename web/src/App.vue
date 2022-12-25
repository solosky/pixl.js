<template>
  <div id="app">
    <h1 class="header">Pixl.js</h1>
    <el-row>
      <el-col :span="18">
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
            <el-button size="mini" icon="el-icon-top">上一级</el-button>
            <el-button size="mini" icon="el-icon-refresh">刷新</el-button>
          </el-button-group>
        </div>
      </el-col>
      <el-col :span="6">
        <div class="action-right">
          <el-button-group>
            <el-button type="info" size="mini" icon="el-icon-cpu">DFU</el-button>
            <el-button :type="connBtnType" size="mini" icon="el-icon-connection" @click="onConnectionBtnClick">{{
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
            <el-breadcrumb-item>amiibo</el-breadcrumb-item>
            <el-breadcrumb-item>塞尔达</el-breadcrumb-item>
          </el-breadcrumb>
        </div>
      </el-col>
    </el-row>
    <div>
      <el-table ref="multipleTable" :data="tableData" tooltip-effect="dark" style="width: 100%"
        cell-class-name="file-cell">
        <el-table-column type="selection" width="55">
        </el-table-column>
        <el-table-column label="文件">
          <template slot-scope="scope">
            <i class="el-icon-folder"></i>
            <el-link :underline="false">{{ scope.row.name }}</el-link>
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
import { pixlBleConnect } from "./lib/pixl.ble"
import { sharedEventDispatcher } from "./lib/event"

export default {
  data() {
    return {
      tableData: [{
        name: "..",
        size: "",
        type: ""
      }, {
        name: 'Midna & Wolf Link.bin',
        size: "540 B",
        type: "bin"
      },
      {
        name: 'Ganondorf.bin',
        size: "540 B",
        type: "bin"
      },
      {
        name: 'Mipha.bin',
        size: "540 B",
        type: "bin"
      }],
      multipleSelection: [],
      connBtnType: "",
      connBtnText: "连接.."
    }
  },
  methods: {
    onConnectionBtnClick() {
      this.connBtnText = "连接中..";
      pixlBleConnect();
    },
    onBleConnected() {
      this.connBtnText = "已连接";
      this.connBtnType = "success";
      this.$notify({
        title: 'Pixl.js',
        type: 'success',
        message: '已成功连接到Pixl.js!',
        duration: 5000
      });
    },
    onBleDisconnected() {
      this.connBtnType = "";
      this.connBtnText = "连接..";
      this.$notify({
        title: 'Pixl.js',
        type: 'error',
        message: 'Pixl.js已经断开连接!',
        duration: 5000
      });
    },
    onBleDisconnectError() {
      this.connBtnType = "";
      this.connBtnText = "连接..";
      this.$notify({
        title: 'Pixl.js',
        type: 'error',
        message: 'Pixl.js连接失败!',
        duration: 5000
      });
    },

  }, mounted() {
    var dispatcher = sharedEventDispatcher();
    dispatcher.addListener("ble_connected", this.onBleConnected);
    dispatcher.addListener("ble_disconnected", this.onBleDisconnected);
    dispatcher.addListener("ble_connect_error", this.onBleDisconnectError);
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
