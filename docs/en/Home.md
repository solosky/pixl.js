
# **Welcome to the pixl.js project documentation !**

The pixl.jx project is a forked version of the original [Pixl.js](http://www.espruino.com/Pixl.js). This fork have Its main focus to simulate Amiibo.

This fork is divided in two main sections, Hardware and Firmware


## **[Hardware](01-Hardware.md)**
[![Example](https://github.com/solosky/pixl.js/raw/main/assets/pixljs-3.jpg "Example")](https://github.com/solosky/pixl.js/raw/main/assets/pixljs-3.jpg "Example")
- ### [PCB](01-Hardware.md#PCB)
- ### [BOM](01-Hardware.md#BOM)
- ### [Pictures](01-Hardware.md#Pictures)
- ### [Demo](01-Hardware.md#Demo)
- ### [Shell](01-Hardware.md#Shell)
- ### [OLED version](01-Hardware.md#OLED-version) 
- ### [Alternative Boards](01-Hardware.md#alternative-boards)

## **Firmware**
In order to use some features of the firmware you must to provide the «key_retail.bin» file, should be uploaded to the root directory of the device storage before it can be used.  You need to provide a legally adquired one, you can use a tool to extract it from your console (3DS or Switch).
>**key_retail.bin checksums:**
>MD5:	45fd53569f5765eef9c337bd5172f937
>SHA1:	bbdbb49a917d14f7a997d327ba40d40c39e606ce

- ### A guide to [Flash the Firmware](02-Flash-Firmware "Flash the Firmware")
- ### To help on the internationalization of the Firmware you can follow the [Guide](https://github.com/solosky/pixl.js/blob/develop/fw/docs/translation.md).
- ### [Building the firmware](03-Build-Firmware)
- ### [Using the firmware](04-Using-Firmware.md)
    - #### [Concepts](04-Using-Firmware.md/#Firmware-Concepts)
    - #### [Amiibo Emulator App](04-Using-Firmware.md/#Amiibo-Emulator)
    - #### [Amiibo Database App](04-Using-Firmware.md/#Amiibo-Database)
    - #### [AmiiboLink Emulator APP](04-Using-Firmware.md/#AmiiboLink)
    - #### [BLE File Transfer APP](04-Using-Firmware.md/#BLE-File-Transfer)
    - #### [Settings APP](04-Using-Firmware.md/#Settings-1)

## Techinical Documentation
- ### [Bluetooth BLE File Transfer protocol](05+1-ble_protocol.md)
- ### [AmiiboLink BLE Protocol](05+2-amiibolink_ble.md)
- ### [Amiibo Database Doc.](05+3-amiidb.md)
- ### [video player](05+4-video_player.md) (Retired on Firmware 2.5.2 due to flash size restrictions.

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

## License

This project is released under the GPL 2.0 License. Please follow the terms of the License when using it.

* If modifications are made to the project, the modified source code needs to be released.
* The source code released must be released using the same License.
