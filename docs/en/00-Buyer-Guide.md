# Buyer Guide and Hardware Variants

The pixl.js firmware in this repository targets the open pixl.js hardware variants documented here. Before updating a device bought from a marketplace, identify the exact hardware variant and use the matching firmware package.

## Official release packages

Release assets are split by hardware version:

| Device hardware | Release package to use |
| --- | --- |
| LCD pixl.js / allmiibo-compatible hardware with the three-way thumbwheel control | `*_LCD.zip` |
| OLED pixl.js hardware with the same three-way thumbwheel control | `*_OLED.zip` |
| OLED pixl.js hardware with a 4th dedicated back button | `*_OLED_4BTN.zip` |

After downloading the correct outer release package, extract it and use the inner `pixjs_ota_vxxx.zip` file for OTA updates.

## Modified or rebranded devices

Some devices are sold with pixl.js-like names, iNFC/AmiiboTool firmware, extra buttons, a different button layout, or a different screen orientation. Those devices may not be compatible with the LCD or OLED release packages from this repository.

One such case, a 4-button OLED board with an added back button, now has an official `OLED_4BTN` package (see table above). Its button map and screen orientation were set based on one specific unit; other batches or vendors of a similar 4-button board may be assembled differently and could still need a different board definition.

Common signs of an incompatible package include:

- the screen is upside down;
- the back button or side buttons do not match the firmware;
- the screen shows stripes, a garbled image, or only the backlight;
- the device works with a vendor firmware package but not with the standard pixl.js LCD/OLED package.

This does not necessarily mean the device is broken. It usually means the hardware needs a different board definition, display orientation, button map, or vendor firmware package. Support for a new variant requires hardware details and test feedback; it cannot be safely selected from the screen type alone.

## Before updating

- Keep a copy of the firmware package that came with the device or was provided by the seller.
- Check whether the device is the documented LCD/OLED pixl.js hardware or a modified AmiiboTool/iNFC-style variant.
- If the device has extra buttons or a different case layout, do not assume the standard LCD/OLED release package is compatible.
- If you are unsure, ask for the exact device variant before flashing.

If the wrong or incompatible firmware was already installed, see the firmware flashing and recovery guide in [Flash the Firmware](02-Flash-Firmware.md).
