制造硬件后，您需要通过有线的方法第一次刷写固件（如daplink、jlink等），之后无需再次进行有线刷写。

固件可以使用以下方法之一刷写或更新：

## 方法 1: Wired
此方法需要一个兼容CMASS-DAP的JLink或DAPLink刷写器。我们推荐使用PWLINK2 Lite，您可以在 [淘宝](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.4b942e8deXyaQO&id=675067753017&_u=d2p75qfn774a "Taobao")购买一个约9.9元。

下载最新版本的固件压缩包。它应该包含以下文件：
- fw_update.bat
- bootloader.hex
- pixljs.hex
- pixljs_all.hex
- fw_readme.txt
- pixjs_ota_v237.zip

你需要确保连接好 3.3V、GND、SWDIO、SWDCLK这四根线。  使用文件中的  `fw_update.bat` 下载 `pixjs_all.hex` 文件.

您也可以使用OpenOCD来刷写设备，以下是执行命令的示例:
```
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg -d2 -c init -c "reset init" -c halt -c "nrf5 mass_erase" -c "program pixjs_all.hex verify" -c "program nrf52832_xxaa.hex verify" -c exit
```

完成首次刷写后，后续的固件更新可以通过OTA进行。

## 方法 2: OTA 升级
这种方法仅适用于已成功通过有线方法烧录完成的Pixl.js设备。

### nRF Connect APP
安装nRF Connect应用程序（您可以在iOS和Android应用商店中找到）。

在设备列表中，选择pixl.js（或pixl dfu），然后点击 `CONNECT` 按钮

将您的pixl.js设备调整到“固件更新”模式，然后设备将进入DFU模式。要这样做，请在设备上选择 `设置` 然后选择 `固件更新`即可.

打开手机上的“nRF Connect”应用，并连接到名为 `pixl dfu` 的设备以更新固件。

在iOS上，压缩包中的固件为 `pixjs_ota_vxxx.zip` ，需要通过微信或QQ与nrfconnect应用共享。

在Android上，您可以使用屏幕右上角的DFU图标，选择 `Distribution packet (ZIP)` ，然后浏览存储中的 `pixjs_ota_vxxx.zip` 文件。

### Web方法
下载与您设备版本相对应的最新固件zip包，并将其解压到一个目录中。

该项目提供两种实现DFU更新的方法：

#### 文件传输网页
首先，您可以连接设备到 [official web page](https://pixl.amiibo.xyz/ "official web page") ，然后在网页上，设备连接后，按下DFU灰色按钮，设备将进入DFU模式，并且页面会询问您“您是否要打开DFU升级页面？”如果您接受，固件更新页面将被打开。

#### 直接进入固件更新页面
你也可以直接进入固件更新页面。

首先，您需要将您的pixl.js设备放置在“固件更新”模式下。为此，请选择`设置`，然后选择`固件更新`。 

打开 [固件更新页面](https://thegecko.github.io/web-bluetooth-dfu).  从您提取固件包的文件夹中拖放或选择`pixljs_ota_xxx.zip`文件。
然后在页面上点击 `SELECT DEVICE` 按钮，您应该看到一个名为`pixl dfu` 的设备，连接以开始固件升级过程。

# 修复错误的固件版本


如果操作不当在设备上烧录了错误的固件版本（LCD/OLED），设备将工作但屏幕上不会显示信息，LCD版本的背光可能会打开。

您可以使用以下方法恢复或刷写正确的固件版本。

## 方法1：通过有线连接刷写固件

如果您手头有任何兼容CMASS-DAP的JLink或DAPLink编程器，您可以使用有线方法手动刷写正确的固件版本。


## 方法2：按照特殊的按键序列再次进入DFU模式，以刷写正确的固件版本。

首先确保您的设备处于关闭状态，然后按照下面的按键序列进入 `DFU 模式`

任意键唤醒设备
左
中
左 x 4
中

现在您的设备已经进入了DFU模式，请使用任何 [nRF Connect APP](#nRF-Connect-APP)  或 [直接进入固件页面更新](#directly-to-the-firmware-update-page) 的方法来升级固件。





