#Pixl.js

This is a forked version of the original [Pixl.js](http://www.espruino.com/Pixl.js). Its main function is to simulate Amiibo.

## picture

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-3.jpg)
![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-4.jpg)

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-5.jpg)


## Demo

[The new version of the Amiibo simulator now allows unlimited amiibo readings on game!, trought random UUID](https://www.bilibili.com/video/BV1TD4y1t76A/)

## Shell

A shell for this project, made by Mr. Baicheng can be downloaded [here](https://www.thingiverse.com/thing:5877482)
![image](https://github.com/solosky/pixl.js/blob/main/assets/pixjs-case1.png)

##PCB

hw/RevC is the latest PCB version, use [Kicad 6](https://www.kicad.org/download/) to open and edit.

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-pcb-revc.png)

## BOM

* [RevC InteractiveHtmlBom](docs/RevC-ibom.html)
* [RevC Bill of Materials Buying Guide](docs/RevC-bom.md)

## Alternative versions

[docs/boards-thirdparty.md](docs/boards-thirdparty.md)

## OLED version

The OLED version was contributed by netizen @xiaohail. You can click [here](https://gitlab.com/xiaohai/pixl.js) to download the PCB.

![image](https://github.com/solosky/pixl.js/blob/main/assets/pixljs-oled1.png)


## Compile firmware

The project has CI set up, and you can download the Nightly build version in [Actions](https://github.com/solosky/pixl.js/actions/workflows/pixl.js-fw.yml) without building it yourself.

If you need to compile it yourself, it is recommended to use the provided docker image to build.
```
docker run -it --rm solosky/nrf52-sdk:latest
root@b10d54636088:/builds# git clone https://github.com/solosky/pixl.js
root@b10d54636088:/builds# cd pixl.js
root@b10d54636088:/builds/pixl.js# git submodule update --init --recursive
root@b10d54636088:/builds/pixl.js# cd fw && make all
root@b10d54636088:/builds/pixl.js/fw# cd application && make full ota
```
The built firmware is in fw/_build/pixjs_all.hex, and ota (wireless update package) is in fw/_build/pixjs_ota_vXXXX.zip

## Device firmware programming

Download the latest version of the firmware zip package and use JLink or DAPLink to program pixjs_all.hex.
After programming is completed, subsequent firmware updates can be updated via OTA.
You can also use OpenOCD to program.
```
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg -d2 -c init -c "reset init" -c halt -c "nrf5 mass_erase" -c "program pixjs_all. hex verify" -c "program nrf52832_xxaa.hex verify" -c exit
```

## OTA updates
Download the latest version of the firmware zip package and extract it to a directory.
After entering DFU mode in the setting interface, enter the [firmware update page](https://thegecko.github.io/web-bluetooth-dfu/examples/web.html). After connecting to Pixl.js, select pixljs_ota_xxx in the compressed package. .zip for OTA firmware update.


## File Upload

Currently, web page transmission and APP transmission are supported.
For web page transfer, you can click on the next web page to upload files to device.

* Main site [https://pixl.amiibo.xyz/](https://pixl.amiibo.xyz/)
* Mirror [https://solosky.github.io/pixl.js/](https://solosky.github.io/pixl.js/)

The App Store iOS application iNFC developed by third-party authors is supported.
You can search for iNFC in the App Store to download the app. Or directly scan the QR code below to jump to the App Store download:

![image](https://github.com/solosky/pixl.js/blob/main/assets/iNFC.jpg)

At the same time, iNFC for Mac and iNFC for Android are under development, so stay tuned!

[Video tutorial on uploading Amiibo to Pixl.js using iNFC](https://www.bilibili.com/video/BV1RV4y1f7bn/)

Thanks to the iNFC application author for supporting Pixl.js!


## Bluetooth protocol

[fw/docs/ble_protocol.md](fw/docs/ble_protocol.md)

## Animation production

[fw/docs/video_player.md](fw/docs/video_player.md)


## Credits

* [FlipperZero Firmware](https://github.com/flipperdevices/flipperzero-firmware)
* [mlib](https://github.com/P-p-H-d/mlib)
* [TLSF](https://github.com/mattconte/tlsf)
* [cwalk](https://github.com/likle/cwalk)
* [SPIFFS](https://github.com/pellepl/spiffs)

## Contribution

* Special thanks to @Caleeeeeeeeeeeee for the perfect Bootloader.
* Special thanks to @白成 (Mr. Baicheng) for making the shell.


## Disclaimer

This project is an open source project and is for learning and research purposes only. Please do not use it for commercial purposes.
Amiibo is a registered trademark of Nintendo, and NTAG21X is a registered trademark of NXP.

The sources of the built-in Amiibo database are:

* [amiiloop](https://download.amiloop.app/)
* [AmiiboAPI](https://www.amiiboapi.com/)

The source code does not contain any resources copyrighted by Nintendo (such as related keys, Amiibo original data, etc.).

# License

This project is released under the GPL 2.0 License. Please follow the terms of the License when using it.

* If modifications are made to the project, the modified source code needs to be released.
* The source code released must be released using the same License.


## Tip

The Amiibo unlimited readings on game requires the «key_retail.bin» file to be uploaded to the root directory of the disk before it can be used.  You need to provide your legally adquired one, you can use a tool to extract it from your console (3DS or Switch).

>**key_retail.bin checksums:**
>MD5:	45fd53569f5765eef9c337bd5172f937
>SHA1:	bbdbb49a917d14f7a997d327ba40d40c39e606ce