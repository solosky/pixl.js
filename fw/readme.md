### 编译步骤

1. 检查fw/components/littlefs目录确保已经初始化子模块
2. 执行key_data_gen.py文件生成src/amiibo_private.c文件（注意修改代码中key文件路径）
3. 执行make生成固件