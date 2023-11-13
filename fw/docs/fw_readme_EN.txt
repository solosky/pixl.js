How to flash firmware:

Method 1: Wired
This method requires the additional purchase of a CMASS-DAP compatible downloader. We recommend pwlink2 lite for 9.9 yuan.
Purchase address: https://item.taobao.com/item.htm?spm=a1z09.2.0.0.4b942e8deXyaQO&id=675067753017&_u=d2p75qfn774a

After connecting the cable, double-click fw_update.bat to program the firmware.

Method 2: OTA update
This method is only applicable to Pixl.js devices that have been successfully programed via wired method.
Install the nrfconnect application (supported by both iOS and Android). After the device selects the "Firmware Update" menu in the Settings menu, the device will enter DFU mode, and then use nrfconnect to connect to the device named pixl dfu to update the firmware.
The firmware is pixjs_ota_vxxx.zip in the compressed package and needs to be shared with the nrfconnect application through WeChat or QQ.
Also you can use the OTA update Method
Download the latest version of the firmware zip package and extract it to a directory.
After entering DFU mode in the setting interface, enter the [firmware update page](https://thegecko.github.io/web-bluetooth-dfu/examples/web.html). After connecting to Pixl.js, select pixljs_ota_xxx in the compressed package. .zip for OTA firmware update.