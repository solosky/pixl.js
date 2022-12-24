<template>
  <div id="app">


    <el-tabs v-model="activeName" @tab-click="handleClick">
      <el-tab-pane label="文件管理" name="first">
        <el-tree :data="data" :props="defaultProps" @node-click="handleNodeClick"  :render-content="renderContent"></el-tree>
      </el-tab-pane>
      <el-tab-pane label="DFU" name="second">DFU1</el-tab-pane>
      <el-tab-pane label="设置" name="third">设置</el-tab-pane>
    </el-tabs>


  </div>

</template>



<script>
export default {
  data() {
    return {
      data: [{
        label: '一级 1',
        children: [{
          label: '二级 1-1',
          children: [{
            label: '三级 1-1-1'
          }]
        }]
      }, {
        label: '一级 2',
        children: [{
          label: '二级 2-1',
          children: [{
            label: '三级 2-1-1'
          }]
        }, {
          label: '二级 2-2',
          children: [{
            label: '三级 2-2-1'
          }]
        }]
      }, {
        label: '一级 3',
        children: [{
          label: '二级 3-1',
          children: [{
            label: '三级 3-1-1'
          }]
        }, {
          label: '二级 3-2',
          children: [{
            label: '三级 3-2-1'
          }]
        }]
      }],
      defaultProps: {
        children: 'children',
        label: 'label'
      },
      activeIndex: '1',
      activeName: 'second'
    };
  },
np
  font-family: Helvetica, sans-serif;
  text-align: center;
  width: 960px;
  margin: 0 auto;
}

.custom-tree-node {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: space-between;
    font-size: 14px;
    padding-right: 8px;
  }
</style>
