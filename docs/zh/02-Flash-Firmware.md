硬件搭建完成后，需要先通过有线方式（如 DAPLink、JLink 等）首次烧录固件，之后的更新可以不再依赖有线烧录。

固件可以通过以下方法之一烧录或更新：

## 方法 1: 线刷
此方法需要一个兼容 CMSIS-DAP 的 JLink 或 DAPLink 调试器。推荐使用 PWLINK2 Lite，在 [淘宝](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.4b942e8deXyaQO&id=675067753017&_u=d2p75qfn774a "Taobao")上约 9.9 元即可买到。

下载最新版本的固件压缩包，里面应包含以下文件：
- fw_update.bat
- bootloader.hex
- pixljs.hex
- pixljs_all.hex
- fw_readme.txt
- pixjs_ota_v237.zip

你需要连接好 3.3V、GND、SWDIO、SWDCLK 四根线，然后运行压缩包中的 `fw_update.bat` 来烧录 `pixjs_all.hex` 固件。

您也可以使用OpenOCD来刷写设备，以下是执行命令的示例:
```
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg -d2 -c init -c "reset init" -c halt -c "nrf5 mass_erase" -c "program pixjs_all.hex verify" -c "program nrf52832_xxaa.hex verify" -c exit
```

完成首次刷写后，后续的固件更新可以通过OTA进行。

## 方法 2: OTA 升级
此方法仅适用于已经通过有线方式成功烧录过的 Pixl.js 设备。

### nRF Connect APP
安装 nRF Connect 应用（iOS 和 Android 应用商店均可下载）。

在设备列表中选择 pixl.js（或 pixl dfu），然后点击 `CONNECT` 按钮。

将 pixl.js 设备切换到"固件更新"模式，设备即进入 DFU 模式。操作方法：在设备上打开 `设置` 应用，选择 `固件更新` 即可。

打开手机上的“nRF Connect”应用，并连接到名为 `pixl dfu` 的设备以更新固件。

在 iOS 上，固件为压缩包中的 `pixjs_ota_vxxx.zip`，需要通过微信或 QQ 将其共享给 nRF Connect 应用。

在 Android 上，可以点击屏幕右上角的 DFU 图标，选择 `Distribution packet (ZIP)`，然后在存储中找到 `pixjs_ota_vxxx.zip` 文件。

### 网页方式
下载与设备版本相对应的最新固件 zip 包，并解压到某个目录。

项目提供了两种通过网页完成 DFU 更新的方法：

#### 通过官方网页传输
首先将设备连接到 [官方网页](https://pixl.amiibo.xyz/ "official web page")，设备连接成功后，点击网页上的灰色 `DFU` 按钮，设备将进入 DFU 模式，此时页面会询问"是否打开 DFU 升级页面？"，点击确认即可进入固件更新页面。

#### 直接进入固件更新页面
也可以直接打开固件更新页面。

首先需要将 pixl.js 设备切换到"固件更新"模式：打开 `设置` 应用，选择 `固件更新` 即可。

打开[固件更新页面](https://thegecko.github.io/web-bluetooth-dfu)，从解压出的固件目录中拖放或选择 `pixljs_ota_xxx.zip` 文件。
然后点击页面上的 `SELECT DEVICE` 按钮，应该能看到名为 `pixl dfu` 的设备，连接它即可开始固件升级。

# 修复错误的固件版本


如果误烧录了错误的固件版本（LCD/OLED），设备仍能正常工作，但屏幕不会显示信息，LCD 版本的背光可能会常亮。

可以通过以下方法恢复或刷写正确的固件版本。

## 方法1：通过有线连接刷写固件

如果手头有兼容 CMSIS-DAP 的 JLink 或 DAPLink 调试器，可以按上文[线刷方法](#方法-1-线刷 "Wired Method")手动刷写正确的固件版本。


## 方法2：按照特殊的按键序列再次进入DFU模式，以刷写正确的固件版本。

首先确保设备处于关机状态，然后按照下面的按键序列进入 `DFU 模式`：

- 任意键唤醒设备
- 左
- 中
- 左 x N
- 中

N 的具体次数随固件版本而异（具体次数无法从代码中确认，需实际尝试）。

此时设备已进入 DFU 模式，使用 [nRF Connect APP](#nrf-connect-app) 或[直接进入固件更新页面](#直接进入固件更新页面)任一方法升级固件即可。

