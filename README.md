# Pixl.js

这是一个基于原版[Pixl.js](http://www.espruino.com/Pixl.js)的复刻版本，主要的功能是用来模拟Amiibo。

## 图片

![image](https://raw.githubusercontent.com/solosky/pixl.js/main/assets/pixljs-1.png)
![image](https://raw.githubusercontent.com/solosky/pixl.js/main/assets/pixljs-2.png)


## 演示

[新版Amiibo模拟器，目前可以做到无限刷！](https://www.bilibili.com/video/BV1TD4y1t76A/)

## PCB

hw/RevB为最新PCB版本，使用[Kicad 6](https://www.kicad.org/download/)打开编辑。

![image](https://raw.githubusercontent.com/solosky/pixl.js/main/assets/pixljs-pcb1.png)

## BOM 

[RevB InteractiveHtmlBom](docs/RevB-ibom.html)

[RevB 物料清单购买指南](docs/RevB-bom.md)

## 衍生版本

[docs/boards-thirdparty.md](docs/boards-thirdparty.md)


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

## OTA更新
下载最新版本固件zip包，解压到一个目录。
在设置界面进入DFU模式后，进入 [固件更新页面](https://thegecko.github.io/web-bluetooth-dfu/examples/web.html)，连接Pixl.js后，选择压缩包里面的 pixljs_ota_xxx.zip 进行OTA固件更新。


## 文件上传

[https://solosky.github.io/pixl.js/](https://solosky.github.io/pixl.js/)


## 提示 

Amiibo无限刷需要网上搜索key_retail.bin文件然后上传到到磁盘根目录后才能使用。