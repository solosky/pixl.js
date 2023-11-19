
# Build

## Build with Github Actions

TODO

## Build with customized Docker image



# Flash

## Flash firmware wired mode

TODO

## Flash firmware with DFU 

TODO 

# FAQ


## How to fix if flashing wrong firmware

you could try to move (blind) by the menus and put the device on the DFU mode, or use the wired mode to reprogram the device.

From the Main Menu you can put the device in DFU mode pressing:

_press any button to wake up the device, then press:_

- LEFT
- MIDDLE
- LEFT X 3
- MIDDLE

(Also to be sure you are on the MAIN MENU, you can remove and put the battery, then start the process above)

Open the page https://thegecko.github.io/web-bluetooth-dfu/examples/web.html load the `pixjs_ota_vXXX.zip` file for the LCD and open it, then press the `SELECT DEVICE` button and if your device is on DFU mode you should see a device called `pixl dfu` connect to it and the process should be finish well.


Also, from main menu you can press:

- LEFT x 2
- MIDDLE
then you are en BLE uplad mode then in https://pixl.amiibo.xyz/ page, you can connect and use the button `DFU` to put the device in DFU mode and programming again.




**For wire programming:**
This method requires the additional purchase of a CMASS-DAP compatible downloader. We recommend pwlink2 lite for 9.9 yuan.
Purchase address: https://item.taobao.com/item.htm?spm=a1z09.2.0.0.4b942e8deXyaQO&id=675067753017&_u=d2p75qfn774a

- Download the latest version of the firmware zip package and use JLink or DAPLink to program pixjs_all.hex.
- After connecting the cable, double-click `fw_update.bat` to program the firmware.
- After programming is completed, subsequent firmware updates can be updated via OTA.
- You can also use OpenOCD to program.
```
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg -d2 -c init -c "reset init" -c halt -c "nrf5 mass_erase" -c "program pixjs_all. hex verify" -c "program nrf52832_xxaa.hex verify" -c exit
```

