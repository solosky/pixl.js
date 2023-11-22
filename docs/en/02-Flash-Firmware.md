After you build the hardware you need to flash the firmware for the first time, that can only be achieved via wired method

The firmware can be flashed or upgraded using one of the follow methods:

## Method 1: Wired
This method requires a CMASS-DAP compatible JLink or DAPLink flasher.  We recommend PWLINK2 Lite Emulator STM32 Programmer, you can buy one for about 9.9 yuan on [Taobao](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.4b942e8deXyaQO&id=675067753017&_u=d2p75qfn774a "Taobao")

Download the latest version of the firmware zip package. It should contain next files:
- fw_update.bat
- bootloader.hex
- pixljs.hex
- pixljs_all.hex
- fw_readme.txt
- pixjs_ota_v237.zip

You need to connect the 3.3 Volt, GND, SWDIO and SWDCLK leads.  you can use the file  `fw_update.bat` to flash the `pixjs_all.hex` firmware.

You can also use OpenOCD to flash the device, this is an example of the command to execute:
```
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg -d2 -c init -c "reset init" -c halt -c "nrf5 mass_erase" -c "program pixjs_all. hex verify" -c "program nrf52832_xxaa.hex verify" -c exit
```
After first flash is completed, subsequent firmware updates can be done via OTA.

## Method 2: OTA update
This method is only applicable to Pixl.js devices that have been successfully programed via wired method.

### nRF Connect APP
Install the nRF Connect application (you can find it on the both iOS and Android app stores).

On the Devices List, select pixl.js (or pixl dfu) and tap the button `CONNECT`

Put your pixl.js device on "Firmware Update" mode, then the device will enter DFU mode.  To do so, select the `Settings` app and select the item `Firmware Update`.

Open "nRF Connect" app on your phone and connect to the device named `pixl dfu` to update the firmware.

On iOS, The firmware is `pixjs_ota_vxxx.zip` in the compressed package and needs to be shared with the nrfconnect application through WeChat or QQ.

On Android you can use the DFU icon on the upper right of the screen, Select the `Distribution packet (ZIP)` option and browse your storage for the `pixjs_ota_vxxx.zip` file.

### Web page method
Download the latest version of the firmware zip package correspondent to your device version, and extract it to a directory.

The project provide two ways to achive a DFU update:

#### File Transfer Web Page.
First you can connect the device to [official web page](https://pixl.amiibo.xyz/ "official web page") then on the web page, after the device is connected, press the `DFU` grey button, the device will enter on the DFU mode and the page ask you "Do you want to open the DFU upgrade page?" if you accept the Firmware Update Page will be open.

#### Directly to the Firmware Update Page.
Also you can go directly to the Firmware Update Page.

First you need to put your pixl.js device on "Firmware Update" mode.  To do so, select the `Settings` app and select the item `Firmware Update`.

Open the [firmware update page](https://thegecko.github.io/web-bluetooth-dfu).  Drag and drop or select `pixljs_ota_xxx.zip` file from the folder where you extract the firmware package.

Then press the `SELECT DEVICE` button on the page you should see a device called `pixl dfu` connect to start the firmware upgrade process.


# Repair wrong firmware version.
If by mistake you program the wrong version (LCD/OLED) on your device, the device will work but will no show information on screen, the backlight of the LCD version could turn on.

You can use the [Wired Method](Flash-Firmware#Method 1: Wired "Wired Method") to flash the correct firmware version.

Also you can blindly navigate trought menu and put your device on BLE or DUF mode then use the correspondant metho to upgrade the firmware.

**To Enter on the DFU mode** and then use the [nRF Connect APP method](Flash-Firmware#nRF Connect APP)  or the [Directly to the Firmware Update Page method](Flash-Firmware#Directly to the Firmware Update Page).

*You can remove and put back the batter to be sure you are on the "**Main Menu**".*

*If needed press any button to wake up the device*

From the **Main Menu** you can put the device in DFU mode pressing:
- LEFT
- MIDDLE
- LEFT X 3
- MIDDLE

Now you device is on DFU mode, use any of the [nRF Connect APP](#nRF-Connect-APP)  or [Directly to the Firmware Update Page](#directly-to-the-firmware-update-page) methods to upgrade the fimware.

------------

From the "**Main Menu**" you also can **Enter the BLE File Transfer** Mode by pressing:
- LEFT x 2
- MIDDLE

As you are BLE File Transfer mode you can use the [File Transfer Web Page.](#file-transfer-web-page) method to upgrade the firmware.