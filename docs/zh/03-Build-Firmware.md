# 构建

## 使用Github Actions进行构建

您可以从Github Actions下载最新的构建开发版本。

https://github.com/solosky/pixl.js/actions


## 使用定制的Docker镜像进行构建

您可以使用定制的Docker镜像构建固件。

```
# 创建容器
docker run -it --rm solosky/nrf52-sdk:latest

# 下载代码
root@b10d54636088:/builds# git clone https://github.com/solosky/pixl.js
root@b10d54636088:/builds# cd pixl.js
root@b10d54636088:/builds/pixl.js# git submodule update --init --recursive

# 构建LCD固件
root@b10d54636088:/builds/pixl.js# cd fw && make all BOARD=LCD RELEASE=1

# 构建OLED固件
root@b10d54636088:/builds/pixl.js# cd fw && make all BOARD=OLED RELEASE=1
```

构建好的固件在 fw/_build/pixjs_all.hex，ota package is fw/_build/pixjs_ota_vXXXX.zip