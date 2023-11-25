# Firmware Concepts
## Amiibo

Amiibo是任天堂销售的游戏周边，通常呈现为一个游戏角色的手办，但也包括卡片、手腕带等其他形式。

每个Amiibo都在手办底座上嵌入了一个NFC标签。NFC标签在工厂预先记录了一些数据，用于识别手办的类型，使每个Amiibo与其他不同。
在一些游戏中（特别是任天堂发布的一方游戏）扫描NFC标签可以获得游戏中的道具等。

## Model Info
这是一个数字，用于描述Amiibo系列、游戏系列、角色和角色变体，此信息在首次编程后无法更改。

## Amiibo ID
用于识别Amiibo手办，手办序列号也是在工厂硬编码的，使得每一种手办都有自己的编号，且能通过Amiibo ID识别是哪一种手办。

## Amiibo Data
标签上的数据的这一部分存储了用户可修改的信息，如所有者和昵称的注册信息或游戏存档数据。

## NTAG215
NTAG215是NXP公司出售的一种NFC卡片，可以用来存储数据在卡片上。使用NFC读卡器可以读取或者写入NTAG215里面的数据。

Amiibo就是使用NTAG215卡片存储数据的。

## UID
为NTG215卡片的卡号。每个NTAG215都有一个7个字节的唯一的卡号。

在修改场景中，术语UID用于引用由Amiibo模拟器发送给游戏的UID，因为游戏使用UID来判断是否已经使用了Amiibo。如果模拟器更改了UID，模拟的Amiibo在游戏中看起来就像是完全不同的Amiibo，从而允许再次使用该Amiibo。

## key_retail.bin
这是一个包含任天堂用于加密Amiibo的私钥的二进制文件，是受版权保护的材料，因此您需要提供一个合法获取的文件，您可以从3DS或Switch主机中导出它。

## .BIN file
Amiibo的数据可以被导出到一个文件，该文件以.BIN扩展名存储了Amiibo原始内容的二进制转储。这些转储文件被上传到设备上供Amiibo模拟器应用程序或AmiiboLink模拟器应用程序使用。您需要提供一个合法获取的二进制转储文件。

通常大小是540字节，但是有些文件可能是不太常见的532字节或者572字节，但是`Amiibo模拟器`应用都能很好的使用。

## 多功能按钮
这是设备顶部的微型开关，是一个具有三个按钮的设备，一个“**中键**“和两个“**侧边**”按钮。将微型开关推向两侧会按下“侧边”按钮。按下微型开关时，会按下“中间”按钮。

在本文档中，按下多功能按钮的两侧被称为“**左键**”和”**右键**“，按下多功能按钮被称为“**中键**”；在菜单中导航项目时，使用侧面按钮，要选择一个项目，请按下”中间“按钮。

# 使用固件

Pixl.js固件的主屏幕分为几个应用程序，每个应用程序都可以运行以执行一些操作，主屏幕上的应用程序包括：


|   |
| ------------ |
|  Amiibo模拟器|
|  Amiibo数据库|
| ֍ AmiiboLink|
| ᛡᛒ 文件传输 |
|  Settings |
|   |

----
# Amiibo模拟器

这个应用程序允许您浏览本地存储中的 .BIN 文件，并将它们用作当前的Amiibo。

您可以使用拇指滑轮滑动开关浏览文件夹和文件，推动到两侧可以更改元素，按下中间按钮可以选择突出显示的元素。

一旦选择了一个 .BIN 文件，其数据就会被用作当前的Amiibo，屏幕显示当前Amiibo的详细信息，如Amiibo当前UUID、文件名和Amiibo名称。

## Amiibo details screen
You can use side buttons to change the current amiibo with the next or previous one on the current folder.  Pressing the middle button you can change the behavior of the current amiibo, on the sub menu:

|   |
| ------------ |
| Rand. Tag<BR>Auto Rand.<BR>Delete Tag<BR>Back to Tag Details<BR>Back to File List<BR>Back to Main Menu<BR>Rand. Tag |
|   |

### Randomize Tag.
Changes the current UUID presented to game to current amiibo. The new UUID stays in place until you change the current amiibo or select this option again.
### Auto Rand. (Automatic Random)
Turning ON this feature, will generate a new random UUID for the current amiibo each time a game read it.  Allowing to use the same amiibo multiple times on games with restrictions.
### Delete Tag
Delete the file associated to current amiibo.
### Back to Tag Details
Returns to the details of the current amiibo.
### Back to File List
Returns to the file list of current folder.
### Back to Main Menu
Exit the amiibo emulator application.

_If you enable UUID random generation, manually or automatic the original UUID of the .BIN file is used the next time that file is loaded._

## File explorer.
If you have **highlighted a folder or a file** and you press and hold the middle button for 1 second a properties sub menu is open:

|   |
| ------------ |
| Create New Folder…<BR>Create New Tag…<BR>Rename…<BR>Delete...<BR>Back to File List<BR>Back to Main Menu |
|   |

### Create New Folder…
Dialog to create a new sub folder on current one.
### Create New Tag…
Creates a blank amiibo .BIN file on the current folder.  That .BIN file is now a simil of a New Ntag215, ready to be programmed, you can assign that .BIN file as current amiibo and using a NFC programmer you can write the amiibo data, by example you can use iNFC or Tagmo for phones, Thenaya for the 3DS.  If the programmed amiibo is on the database the .BIN file is even renamed to the name of the amiibo.
### Rename…
Dialog to rename highlighted .BIN file or folder
### Delete…
Remove highlighted object, asking for confirmation.  If is a folder, all subfolders and files are deleted too.
### Back to File List
Returns to the file list of current folder.
### Back to Main Menu
Exit the amiibo emulator application.

If you have **highlighted a storage** the properties sub menu is different:

|   |
| ------------ |
| Storage Status<BR>Total Space<BR>Free Space<BR>Format…<BR>Back List<BR>Back to Main Menu |
|   |

### Storage Status
Shows the current status and type of the storage, by example:
```
=====Not Mounted=====
===Mounted[LFS]===
===Mounted[FFS]===
```

### Total Space
Shows total space of the mounted storage on KB.
### Free Space
Shows the actual free space of the mounted storage on KB.
### Format…
Ask confirmation for formatting the current storage, WARNING ALL DATA WILL BE LOST!
### Back to File List
Returns to the file list of current folder.
### Back to Main Menu
Exit the amiibo emulator application.

----
# Amiibo Database
This application allows the emulation of amiibo from the list of well know ones, using legally available information like the model info, then when you select one, a new virtual amiibo is created on memory using a random UUID.  In order to use this application you must to provide your «key_retail.bin» file.

_Each time one an amiibo is used the initial UUID is random generated._

Once you open the application the main menu have the options

|   |
| ------------ |
| Browser…<BR>Search…<BR>My Favorites…<BR>My Amiibo…<BR>Settings… |
|   |

## Browser… 
Let you browse the database, first by category, then all amiibo on that category.
After you select an amiibo, the details are shown, the first line contains the actual UUID and second line the amiibo name.

On the amiibo details page you can use the side buttons to navigate through all the amiibo on the category, if you press the middle button you can change the behavior of the current amiibo, with the sub menu:

|   |
| ------------ |
| Rand. Tag<BR>Auto Rand.<BR>Favorite…<BR>Save As…<BR>[Back to Details]<BR>[Back to List]<BR>[Exit] |
|   |

### Rand. Tag
Randomize Tag, changes the current UUID presented to game to current amiibo. The new UUID stays in place until you change the current amiibo or select this option again.
### Auto Rand.
Automatic Random, Turning ON this feature, will generate a new random UUID for the current amiibo each time a game reads it.  Allowing to use the same amiibo multiple times on games with restrictions.
### Favorite…
Open a new submenu to manage Favorite folders.  You have the options:

```
New…
Folder1
Folder2
[Back]
```

**New…** allows you to create a new Favorites folder, **Back** go back to current amiibo details, if you select an existing folder, that amiibo definition will be added to selected folder.

### Save As…
Allows you to assign the current amiibo to one of the «My Amiibo» slots.
### [Back to Details]
Returns to the details of the current amiibo.
### [Back to List]
Returns to the amiibo list of current category.
### [Exit]
Exit the Amiibo Database application.


## Search…
Allows to search the database using partial name, the search result is like a category, you can select any amiibo on it and set it like another one.

## My Favorites…
Display the existent favorites folders, you can browse them with side buttons and select one with middle button.  If you press and Hold the middle button a sub menu is open:

|   |
| ------------ |
| New…<BR>Empty…<BR>Delete…<BR>[Back] |
|   |

### New…
Allows you to create a new favorites folder.
### Empty…
After confirmation, removes all amiibo associated to the selected folder.
### Delete…
After confirmation, removes the favorite folder.
### [Back]
Returns to Folder list.

Once you select a folder a list of the associated amiibo to that favorite folder is shown, you can browse using side buttons and select an amiibo with middle button, the selected amiibo becomes the current one, and behave like an amiibo of the browser.

## My Amiibo…
List the configured slots, you can browse the slots using side buttons, select one with middle button and the amiibo associated to that slot becomes the current one.

If you press and hold middle button a sub menu with the option of reset the slot is shown allowing to empty the slot.

## Settings…
The settings menu show you the Keys status and number of slot set up.

|   |
| ------------ |
| Keys []<BR>Slot Num. []<BR>[Back] |
|   |

### Keys []
Show the status of the «key_retail.bin» file, can be «Loaded» or «NOT Loaded»
### Slot Num. []
Allows to select how many slots are available to «My Amiibo» section, values from 10 to 50 on tenths increments.
### [Back]
Exit the Amiibo Database and returns to Main Menu.

----
# AmiiboLink
This application emulates an AmiiboLink (AmiLoop or omllbolink) device.

These devices are generic espruino boards sell on different forms, normally are Keychains drop shaped, but also there are some Puck or Sheika Slate shaped.

Almost all have a button and some LEDs as indicators and runs from CR2032 cell batteries or lipo cells.

All of them are controlled from phone applications via Bluetooth, also you can upload one or several .BIN amiibo files using the phone to the device.

Starting this application the details for current amiibo are shown.  If you are on sequential mode you can use the side buttons to walk through all the uploaded amiibo, in all modes, you can press the middle button to open the AmiiboLink APP config sub menu, the menu have this items:

|   |
| ------------ |
| Mode []<BR>Auto Random. []<BR>Compati. Mode []<BR>Tag Details<BR>[Main Menu] |
|   |

## Mode []
Shows the actual operation mode, when selected you can choose the mode of amiibo working, the available modes are:
- ### Randomize (Manual)
On the actual AmiiboLink device the current amiibo changes the UUID when you press the device button
- ### Randomize (Auto)
A new random UUID for the current amiibo is generated after each time a game read it.  Allowing to use the same amiibo multiple times on games with restrictions. You cannot change the active amiibo on this mode.
- ### Sequential mode
On this mode you can change the actual amiibo using the side buttons, rolling through all amiibo uploaded with the phone app. All uploaded .BIN files are stored on the 01.bin to 26.bin files at the /amiibolink/ folder.
- ### Read-write mode
The device use a "special" amiibo slot where only an amiibo can be used, the amiibo can be replaced by another .BIN file from the phone app. On the local storage this amiibo is saved with at the /amiibolink/99.bin file.

Changing the mode preserve the current selected amiibo, with exception of «Read-write mode».

## Auto Random. []
When this option is turn ON a new random UUID for the current amiibo is generated after each time a game read it.  Allowing to use the same amiibo multiple times on games with restrictions. You cannot change the active amiibo on this mode.  Cannot be turned OFF for «Randomize (Auto)» mode, cannot be turned ON for «Randomize (Manual)»mode; 
## Compati. Mode []
Shows the emulation firmware mode, the supported values are V1, V2 and AmiLoop, there are several APPs each one designed for a AmiiboLink firmware version or to the AmiLoop protocol.  Using this setting you can match the emulation with the protocol needed by the APP you use on your phone.
## Tag Details
Return to amiibo details page
## [Main Menu]
Exit the AmiiboLink Emulator application.

## AmiiboLink Compatible Applications
So far the only two AmiiboLink/AmiLoop mode manager phone Apps are translated to English, they are "Umiibo" and "AmiLoop" this applications are compatible only with the AmiLoop Mode.

Other oddity is what the modes on applications do not correspond with the modes set up on the device.

| Umiibo APP | Pixl.js Emulation Mode |
| ------------ |------------ |
| Manual Mode | Random automatic |
| File Mode | Sequential |
| Auto Mode | Read/Write |

| AmiiLoop APP | Pixl.js Emulation Mode |
| ------------ |------------ |
| Manual Mode | Random automatic |
| File Mode | Sequential |
| Auto Mode | Read/Write |

----
# BLE File Transfer
This application allows you to connect the iNFC or the pixl.js web page, to manage files on the device storage or update the firmware.

The official web page is [https://pixl.amiibo.xyz/](https://pixl.amiibo.xyz/ "https://pixl.amiibo.xyz/.").

The **iNFC APP** is a third party APP

Entering this mode the Bluetooth MAC address of device and official URL are shown at the screen.

----
# Settings
This application allows to configure the device working settings, the application have the next items on the menu:

|   |
| ------------ |
| Version []<BR>Language []<BR>Auto Storage []<BR>Backlight / OLED Contrast []<BR>Menu Animation []<BR>LiPO Battery []<BR>Memory Used []<BR>Fast Resume []<BR>Sleep Timer []<BR>Firmware Update<BR>System Reboot |
|   |

## Version
This submenu shows the version details of the firmware.
- Version
- Build date
- Githash
- Branch
- Target
- Dirty
## Language
You can change the language displayed on the device
## Auto Storage
Turning ON this setting the local storage(s) of the device are hidden on the root of the Amiibo Emulator application, if this setting is OFF the «External Flash» object is shown at the root of the Amiibo Emulator application.
## Backlight / OLED Contrast
With this setting you can control the brightness of the LCD screen or the contrast of the OLED screen.
## Menu Animation
Enable / disable the animation of items larger than screen allowing to read them completely, enabling this option increase the battery consumption.
## LiPO Battery
Enable the use of a LiPO battery.  It requires an hardware mod. If you build a LiPO version of Pixl.js device, you can enable this option. For CR2032 version, this option will not work and should not be enabled.

If you enable LiPO option, the device will use LiPO voltage level to display the battery level, and also can display a charge label when the battery is in charge mode.
## Memory Used
Shows the % used storage memory at the bottom of the screen
## Fast Resume
Enables hibernation of device, allowing a quicker response of the reading of the current amiibo by a game, and resume to the application open at the moment of sleep, when disabled the resume after sleep takes 1 second more and the device restore to the Main Menu.
## Sleep Timer
Set the time when the device go to sleep after the last user input or amiibo read.
## Firmware Update
This option put the device in the DFU mode, allowing OTA firmware update, you can update the firmware using the iNFC or the web page.

You can go to the URL [https://thegecko.github.io/web-bluetooth-dfu/](https://thegecko.github.io/web-bluetooth-dfu/) to upload the firmware, this page also can be open through the official site [https://pixl.amiibo.xyz/](https://pixl.amiibo.xyz/)
## System Reboot
Allows you to reboot the device and get back to the state after you remove and put the battery.
