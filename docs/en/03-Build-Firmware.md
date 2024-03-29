# Build

## Build with Github Actions

You could download the latest develop build from Github Actions

https://github.com/solosky/pixl.js/actions


## Build with customized Docker image

You could build the firmware using customized Docker image. 

```
# create containers
docker run -it --rm solosky/nrf52-sdk:latest

# init repository
root@b10d54636088:/builds# git clone https://github.com/solosky/pixl.js
root@b10d54636088:/builds# cd pixl.js
root@b10d54636088:/builds/pixl.js# git submodule update --init --recursive

# build LCD version
root@b10d54636088:/builds/pixl.js# cd fw && make all BOARD=LCD RELEASE=1

# build OLED version
root@b10d54636088:/builds/pixl.js# cd fw && make all BOARD=OLED RELEASE=1

```

The firmware is fw/_build/pixjs_all.hex，ota package is fw/_build/pixjs_ota_vXXXX.zip