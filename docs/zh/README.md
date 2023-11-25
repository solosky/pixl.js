
# **Pixl.js文档**

这是一个基于原版[Pixl.js](http://www.espruino.com/Pixl.js)的复刻版本，主要的功能是用来模拟Amiibo。

这个项目主要分成两个部分：硬件和固件。


## **[硬件](01-Hardware.md)**
[![Example](https://github.com/solosky/pixl.js/raw/main/assets/pixljs-3.jpg "Example")](https://github.com/solosky/pixl.js/raw/main/assets/pixljs-3.jpg "Example")
- ### [PCB](01-Hardware.md#PCB)
- ### [BOM](01-Hardware.md#BOM)
- ### [图片](01-Hardware.md#Pictures)
- ### [演示](01-Hardware.md#Demo)
- ### [外壳](01-Hardware.md#Shell)
- ### [OLED版本](01-Hardware.md#OLED-version) 
- ### [其他版本](01-Hardware.md#alternative-boards)

## **固件**

为了使用固件的一些功能，您必须提供`key_retail.bin`文件，应在设备存储的根目录中上传，然后才能使用。您需要提供一个合法获得的文件，您可以使用工具从您的控制台（3DS 或 Switch）中提取它。

>**key_retail.bin checksums:**
>MD5:	45fd53569f5765eef9c337bd5172f937
>SHA1:	bbdbb49a917d14f7a997d327ba40d40c39e606ce

- ### A guide to [Flash the Firmware](02-Flash-Firmware "Flash the Firmware")
- ### 国际化 [Guide](https://github.com/solosky/pixl.js/blob/develop/fw/docs/translation.md).
- ### [构建固件](03-Build-Firmware)
- ### [使用手册](04-Using-Firmware.md)
    - #### [概念](04-Using-Firmware.md/#Firmware-Concepts)
    - #### [Amiibo模拟器](04-Using-Firmware.md/#Amiibo-Emulator)
    - #### [Amiibo数据库](04-Using-Firmware.md/#Amiibo-Database)
    - #### [AmiiboLink](04-Using-Firmware.md/#AmiiboLink)
    - #### [蓝牙传输](04-Using-Firmware.md/#BLE-File-Transfer)
    - #### [设置](04-Using-Firmware.md/#Settings-1)

## 技术文档
- ### [蓝牙协议](05+1-ble_protocol.md)
- ### [AmiiboLink蓝牙协议](05+2-amiibolink_ble.md)
- ### [视频播放器](05+4-video_player.md) (由于固件大小限制，从固件版本2.6.2起，此功能已从固件中删除)