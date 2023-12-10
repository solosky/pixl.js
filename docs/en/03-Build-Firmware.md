# Build

## Build with Github Actions

You could download the latest develop build from Github Actions

https://github.com/impeeza/pixl.js/actions/workflows/pixl.js-fw.yml


## Build with customized Docker image

You could build the firmware using customized Docker image. 

```
docker run -it --rm solosky/nrf52-sdk:latest
root@b10d54636088:/builds# git clone https://github.com/solosky/pixl.js
root@b10d54636088:/builds# cd pixl.js
root@b10d54636088:/builds/pixl.js# git submodule update --init --recursive
root@b10d54636088:/builds/pixl.js# cd fw && make all
root@b10d54636088:/builds/pixl.js/fw# cd application && make full ota
```

The firmware is fw/_build/pixjs_all.hex，ota package is fw/_build/pixjs_ota_vXXXX.zip