After you build the hardware you need to flash the firmware for the first time, that can only be achieved via wired method

The firmware can be flashed or upgraded using one of the follow methods:

## Method 1: Wired
This method requires a CMSIS-DAP-compatible JLink or DAPLink flasher.  We recommend PWLINK2 Lite Emulator STM32 Programmer, you can buy one for about 9.9 yuan on [Taobao](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.4b942e8deXyaQO&id=675067753017&_u=d2p75qfn774a "Taobao")

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
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg -d2 -c init -c "reset init" -c halt -c "nrf5 mass_erase" -c "program pixjs_all.hex verify" -c "program nrf52832_xxaa.hex verify" -c exit
```
After first flash is completed, subsequent firmware updates can be done via OTA.

### Note for the 4-button OLED variant (`OLED_4BTN`)
OTA updates only replace the application, never the bootloader. On this variant the DFU/"Firmware Update" screen shown by the bootloader can stay upside down even after an OTA update, since that screen belongs to the bootloader. If it bothers you, the fix requires reflashing `pixljs_all.hex` over the wired connection above; there is no OTA-only way to correct it.

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
Download the latest version of the firmware zip package corresponding to your device version, and extract it to a directory.
The release package is an outer zip file for your hardware version. Do not upload that outer zip file to the DFU updater. After extracting it, use the inner OTA package named `pixjs_ota_vxxx.zip`. Uploading the outer release zip can cause errors such as `Unable to find manifest` or `init not found`.

The project provide two ways to achive a DFU update:

#### File Transfer Web Page.
First you can connect the device to [official web page](https://pixl.amiibo.xyz/ "official web page") then on the web page, after the device is connected, press the `DFU` grey button, the device will enter on the DFU mode and the page ask you "Do you want to open the DFU upgrade page?" if you accept the Firmware Update Page will be open.

#### Directly to the Firmware Update Page.
Also you can go directly to the Firmware Update Page.

First you need to put your pixl.js device on "Firmware Update" mode.  To do so, select the `Settings` app and select the item `Firmware Update`.

Open the [firmware update page](https://thegecko.github.io/web-bluetooth-dfu).  Drag and drop or select the inner `pixjs_ota_vxxx.zip` file from the folder where you extracted the firmware package.

Then press the `SELECT DEVICE` button on the page you should see a device called `pixl dfu` connect to start the firmware upgrade process.


# Recovery after a wrong or incompatible firmware update

If an update completes but the device appears unusable, it may still be recoverable. The most common cases are a display contrast setting that makes the screen unreadable, or a firmware package that does not match the hardware.

## Blank or very faint LCD after update

If the LCD backlight turns on but the text is invisible or extremely faint, the contrast setting may be too low. The device can still be running normally.

Try this blind button sequence to reach the contrast setting and raise it:

1. Make sure the device is awake. If the screen is asleep, press MIDDLE once.
2. Press LEFT once.
3. Press MIDDLE once.
4. Press RIGHT 3 times.
5. Press MIDDLE once.
6. Press RIGHT repeatedly until the text becomes visible.
7. Set the contrast to a visible value, commonly around 50 to 80.

If this works, no firmware reflash is required.

## Wrong LCD/OLED package or unsupported hardware variant

If you accidentally flash the wrong hardware version (LCD/OLED), or flash this firmware onto an unsupported modified device, the device can still boot, but the display driver, screen orientation, or button map may not match the hardware. You may see stripes, a garbled screen, an upside-down screen, broken buttons, or only the LCD backlight. The display will stay this way until you flash firmware that matches your hardware.

You can use either method below to recover.

## Option 1: Flash the firmware via wired connection

If you have any CMSIS-DAP-compatible JLink or DAPLink programmer on hand, you can use the [Wired Method](#method-1-wired) to flash the correct firmware version manually.


## Option 2: Enter DFU mode with the key sequence.

Because the screen is unreadable, enter `DFU Mode` by pressing the buttons blindly:

1. Make sure your device is off.
2. Press any key once to wake up the device.
3. Press LEFT once.
4. Press MIDDLE once.
5. Press LEFT 4 times if the wrong firmware is older than 2.11.x, or LEFT 5 times if it is 2.11.x or newer.
6. Press MIDDLE once.

If you do not know which version was flashed, try LEFT 5 times first because current releases use that menu position. If the device does not advertise as `pixl dfu`, power it off and try again with LEFT 4 times.

The screen may still show stripes or a garbled image while the device is in DFU mode. This is expected when the wrong display firmware is installed.

If blind menu navigation fails, use the bootloader DFU button instead: power off the device, then hold RIGHT and power on the device while continuing to hold RIGHT for about 3 seconds until the device advertises as `pixl dfu`.

Now the device is in DFU mode. Use either the [nRF Connect APP](#nrf-connect-app) or the [Firmware Update Page](#directly-to-the-firmware-update-page) to flash the correct `pixjs_ota_vxxx.zip` file for your hardware version. If the web update stalls, start the update again; DFU can resume. If the official web page does not find the device or the browser update continues to stall, use nRF Connect or the wired method instead.
