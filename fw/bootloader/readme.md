### bootloader

#### build:
```
make privgen # 保存好priv.pem文件
make
```

#### init:

编译好application和bootloader
```
make settingsgen  # app.hex为你的app程序
```

### update zip:
app_new.hex 为新的固件程序
--application-version需要递增
--hw-version固定52，如更新bl请与bl中数值保持一致
--sd-req固定为0x0103，如更新softdevice请查看对应版本发行说明
nrfutil pkg generate --application app_new.hex --application-version 2 --hw-version 52 --sd-req 0x0103 --key-file priv.pem appv2_sd112.zip


#### dfu:
移除电池，按住按钮安装电池，此时搜索蓝牙出现名为pixl dfu的设备，使用nrf connect连接
点击dfu按钮，选择更新压缩包，等待传输完成，led灯会亮
升级完成后设备会自动跳转到程序执行
注意：如果发生莫名其妙的中断请更换新电池保证电压稳定