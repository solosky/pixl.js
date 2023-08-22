# Pixl.js

这是一个基于原版[Pixl.js](http://www.espruino.com/Pixl.js)的复刻版本，主要的功能是用来模拟Amiibo。

## 图片

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-3.jpg)
![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-4.jpg)

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-5.jpg)


## 演示

[新版Amiibo模拟器，目前可以做到无限刷！](https://www.bilibili.com/video/BV1TD4y1t76A/)

## 外壳

项目由 白橙 大佬制作的外壳，可以在[这里下载](https://www.thingiverse.com/thing:5877482) 
![image](https://github.com/solosky/pixl.js/blob/main/assets/pixjs-case1.png)

## PCB

hw/RevC为最新PCB版本，使用[Kicad 6](https://www.kicad.org/download/)打开编辑。

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-pcb-revc.png)

## BOM 

* [RevC InteractiveHtmlBom](docs/RevC-ibom.html)
* [RevC 物料清单购买指南](docs/RevC-bom.md)

## 衍生版本

[docs/boards-thirdparty.md](docs/boards-thirdparty.md)

## OLED版本

OLED版本由网友 @xiaohail 贡献，可以点[这里](https://gitlab.com/xiaohai/pixl.js)下载PCB。

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-oled1.png)


## 编译

项目设置了CI，可以在[Actions](https://github.com/solosky/pixl.js/actions/workflows/pixl.js-fw.yml)里面下载Nightly构建版本的，无需自行构建。

如需自行编译，这里推荐使用提供的docker镜像构建。
```
docker run -it --rm solosky/nrf52-sdk:latest
root@b10d54636088:/builds# git clone https://github.com/solosky/pixl.js
root@b10d54636088:/builds# cd pixl.js/
root@b10d54636088:/builds/pixl.js# git submodule update --init --recursive
root@b10d54636088:/builds/pixl.js# cd fw/bootloader && make
root@b10d54636088:/builds/pixl.js# cd fw && make && make full
```
构建出来的固件在 fw/_build/pixjs_all.hex。

## 固件烧写

下载最新版本的固件zip包，使用JLink或者DAPLink烧写pixjs_all.hex即可。
烧写完毕后，后续固件更新可以使用OTA的方式更新。
也可以用OpenOCD烧录。
```
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg -d2 -c init -c "reset init" -c halt -c "nrf51 mass_erase" -c "program pixjs_all.hex verify" -c "program nrf52832_xxaa.hex verify" -c exit
```

## OTA更新
下载最新版本固件zip包，解压到一个目录。
在设置界面进入DFU模式后，进入 [固件更新页面](https://thegecko.github.io/web-bluetooth-dfu/examples/web.html)，连接Pixl.js后，选择压缩包里面的 pixljs_ota_xxx.zip 进行OTA固件更新。


## 文件上传

目前支持网页传输和APP传输。 <br />
网页传输可以点开下面的网页进行上传。

* 主站 [https://pixl.amiibo.xyz/](https://pixl.amiibo.xyz/)
* 镜像 [https://solosky.github.io/pixl.js/](https://solosky.github.io/pixl.js/)

App Store上第三方作者开发的iOS应用 iNFC。
可以在App Store搜索 iNFC 下载应用。或直接扫描下方二维码跳转App Store下载：

![image](https://github.com/solosky/pixl.js/blob/main/assets/iNFC.jpg)

同时，Mac版iNFC、安卓版iNFC正在开发中，敬请期待！

[使用iNFC上传Amiibo至Pixl.js的视频教程](https://www.bilibili.com/video/BV1RV4y1f7bn/)

感谢iNFC应用作者对Pixl.js的支持！


## 蓝牙协议

[fw/docs/ble_protocol.md](fw/docs/ble_protocol.md)

## 动画制作

[fw/docs/video_player.md](fw/docs/video_player.md)


## Credits

* [FlipperZero Firmware](https://github.com/flipperdevices/flipperzero-firmware)
* [mlib](https://github.com/P-p-H-d/mlib)
* [TLSF](https://github.com/mattconte/tlsf)
* [cwalk](https://github.com/likle/cwalk)
* [SPIFFS](https://github.com/pellepl/spiffs)

## Contribution 

* 特别感谢 @Caleeeeeeeeeeeee 完善的Bootloader。
* 特别感谢 @白橙 制作的外壳。 


## 声明

本项目为开源项目，仅为学习研究用途，请勿用于商业用途。 <br />
Amiibo是任天堂的注册商标，NTAG21X为NXP的注册商标。

内置的Amiibo数据库来源分别如下：

* [amiiloop](https://download.amiloop.app/)
* [AmiiboAPI] (https://www.amiiboapi.com/)

源代码没有包含任何有任天堂版权的资源（比如相关密钥，Amiibo原始数据等）。

# License

本项目基于GPL 2.0 License发布，使用请遵循License的约定。

* 如果对项目做了修改，需要把改后的源码发布出来
* 发布出来的源码必须要使用相同的License发布


## 提示 

Amiibo无限刷需要网上搜索key_retail.bin文件然后上传到到磁盘根目录后才能使用。
