# Pixl.js

这是一个基于原版[Pixl.js](hhttp://www.espruino.com/Pixl.js)的复刻版本，主要的功能是用来模拟Amiibo。

## 图片

![image](https://raw.githubusercontent.com/solosky/pixl.js/main/assets/pixljs-1.png)
![image](https://raw.githubusercontent.com/solosky/pixl.js/main/assets/pixljs-2.png)


## 演示

[新版Amiibo模拟器，目前可以做到无限刷！](https://www.bilibili.com/video/BV1TD4y1t76A/)

## 固件烧写

下载最新版本的固件zip包，使用JLink或者DAPLink烧写pixjs_all.hex即可。
烧写完毕后，后续固件更新可以使用OTA的方式更新。

## OTA更新
下载最新版本固件zip包，解压到一个目录。
在设置界面进入DFU模式后，进入 [固件更新页面](https://thegecko.github.io/web-bluetooth-dfu/examples/web.html)，连接Pixl.js后，选择压缩包里面的 pixljs_ota_xxx.zip 进行OTA固件更新。


## 文件上传

[https://solosky.github.io/pixl.js/](https://solosky.github.io/pixl.js/)


## 提示 

Amiibo无限刷需要网上搜素key_retail.bin文件然后上传到到磁盘根目录后才能使用。