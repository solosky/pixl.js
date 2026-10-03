# **Pixl.js文档**

本项目是原版 [Pixl.js](http://www.espruino.com/Pixl.js) 的 fork 版本，主要目标是模拟 Amiibo。

文档分为硬件和固件两大板块。

# [硬件](01-Hardware.md)

- [PCB](01-Hardware.md#PCB)
- [BOM](01-Hardware.md#BOM)
- [图片](01-Hardware.md#Pictures)
- [演示](01-Hardware.md#Demo)
- [外壳](01-Hardware.md#Shell)
- [OLED版本](01-Hardware.md#OLED-version) 


# 固件

- [烧录固件](02-Flash-Firmware.md "Flash the Firmware")
- [固件国际化](05+5-translation.md)
- [构建固件](03-Build-Firmware.md)
- [使用固件](04-Using-Firmware.md)


# [使用手册](04-Using-Firmware.md)

- [概念](04-Using-Firmware.md/#概念)
- [Amiibo模拟器](04-Using-Firmware.md/#Amiibo模拟器)
- [Amiibo数据库](04-Using-Firmware.md/#Amiibo数据库)
- [AmiiboLink](04-Using-Firmware.md/#AmiiboLink)
- [卡模拟器](04-Using-Firmware.md/#卡模拟器)
- [蓝牙传输](04-Using-Firmware.md/#蓝牙传输)
- [设置](04-Using-Firmware.md/#设置)

# 外壳

TODO

# 技术文档

- [蓝牙文件传输协议](05+1-ble_protocol.md)
- [AmiiboLink蓝牙协议](05+2-amiibolink_ble.md)
- [视频播放器](05+4-video_player.md) (由于固件大小限制，从固件版本2.5.2起，此功能已从固件中移除)

# 教程

## iNFC

- [Pixl 固件升级教程](https://www.youtube.com/watch?v=vldNVaoqJg0)

## MTools Lite

- [如何在 Pixl.js OLED/LCD 上使用卡模拟器](https://www.youtube.com/watch?v=KiuyfBKalhI)

# key_retail.bin

要使用固件的部分功能，必须提供 `key_retail.bin` 文件，上传到设备存储根目录后才能使用。<br/>
请提供合法获取的文件，可以使用工具从你的主机（3DS 或 Switch）中提取。

>**key_retail.bin checksums:** <br/>
>MD5:	45fd53569f5765eef9c337bd5172f937 <br/>
>SHA1:	bbdbb49a917d14f7a997d327ba40d40c39e606ce 