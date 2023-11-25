
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

## Credits

* [FlipperZero Firmware](https://github.com/flipperdevices/flipperzero-firmware)
* [mlib](https://github.com/P-p-H-d/mlib)
* [TLSF](https://github.com/mattconte/tlsf)
* [cwalk](https://github.com/likle/cwalk)
* [SPIFFS](https://github.com/pellepl/spiffs)
* [ChameleonUltra](https://github.com/RfidResearchGroup/ChameleonUltra)

## 感谢

* 特别感谢 @Caleeeeeeeeeeeee 完善的Bootloader。
* 特别感谢 @白橙 制作的外壳。 

## 讨论群

国内用户可以加入QQ群 109761876 进行交流。

## Office Channel

Where do you find the community?
* [Pixl.js community discord server](https://discord.gg/t8naCqy4)


## 声明

本项目为开源项目，仅为学习研究用途，请勿用于商业用途。 <br />
Amiibo是任天堂的注册商标，NTAG21X为NXP的注册商标。

内置的Amiibo数据库来源分别如下：

* [amiiloop](https://download.amiloop.app/)
* [AmiiboAPI](https://www.amiiboapi.com/)

源代码没有包含任何有任天堂版权的资源（比如相关密钥，Amiibo原始数据等）。

# License

本项目基于GPL 2.0 License发布，使用请遵循License的约定。

* 如果对项目做了修改，需要把改后的源码发布出来
* 发布出来的源码必须要使用相同的License发布


## 提示 

Amiibo无限刷需要网上搜索key_retail.bin文件然后上传到到磁盘根目录后才能使用。