# OLED 4-Button Version (PC_V2.0)

A larger OLED board with a dedicated 4th "back" button, silkscreened **PC_V2.0**. It is a re-layout of the OLED board contributed by @xiaohail ([gitlab.com/xiaohai/pixl.js](https://gitlab.com/xiaohai/pixl.js)), built with `BOARD=OLED_4BTN`.

No design files were released for PC_V2.0. This page documents it from photos, chip markings, firmware testing, and xiaohai's KiCad netlist, which the circuit closely follows. Reference designators below are the ones printed on the PC_V2.0 silkscreen; they do **not** match xiaohai's.

| Front | Back |
| --- | --- |
| ![front](images/front.jpg) | ![back](images/back.jpg) |

## Confidence legend

| Mark | Meaning |
| --- | --- |
| ✅ | Verified on PC_V2.0: chip marking read, or confirmed working with the `OLED_4BTN` firmware |
| 🔶 | Inferred: matches xiaohai's netlist and the part placement, but not continuity-tested on PC_V2.0 |
| ❓ | Unknown |

## Changes from xiaohai's OLED board

| | xiaohai OLED | PC_V2.0 |
| --- | --- | --- |
| Input | 3-way thumbwheel (left / push / right) | 4 tactile buttons: UP, DOWN, OK, RETURN |
| Back button | — | SW4 on **P0.20** (unused GPIO on xiaohai's board) |
| Display mounting | Normal orientation | Rotated 180° (`OLED_ROTATION_180`) |
| NFC antenna | 0.2 mm enameled wire, 5.5 turns around a screw post | Copper loop on the PCB around the display |
| Power path | CJL2623 dual MOSFET (power path + Hall reset) | AO3401A single P-MOSFET (Q3) + Schottky (D1) |
| Hall sensor | MH254ESQ, resets the MCU through a MOSFET | CC6201ST (U5), resets the MCU when a magnet is near |
| Battery | 501525 | 402030, 3.7 V 200 mAh |
| Status LEDs | Status LED + charge LED | None |
| SWD access | 4 pads (J2), also routed to USB-C D+/D− | No pads found; ships with a bootloader, updated over OTA |
| Screen options | 1.3" / 1.54" OLED or LCD footprints | 1.3" SH1106 OLED only |

## nRF52832 pin map

U1 is an nRF52832-QFAA in QFN-48 (marking `N52832 QFAAE1`) ✅. Pin numbers are from the QFN-48 package.

| Pin | Signal | Connects to | |
| --- | --- | --- | --- |
| 1 | DEC1 | Decoupling cap | 🔶 |
| 2 | P0.00 / XL1 | X1 32.768 kHz + load cap | 🔶 |
| 3 | P0.01 / XL2 | X1 32.768 kHz + load cap | 🔶 |
| 4 | P0.02 / AIN0 | Battery voltage divider (`ADC_PIN`) | 🔶 |
| 5 | P0.03 / AIN1 | Charger U3 CHRG output (`CHRG_PIN`) | 🔶 |
| 7 | P0.05 | **SW2 DOWN** | ✅ |
| 8 | P0.06 | **SW1 OK** | ✅ |
| 9 | P0.07 | **SW3 UP** | ✅ |
| 11 | NFC1 / P0.09 | NFC antenna loop + tuning cap to GND | 🔶 |
| 12 | NFC2 / P0.10 | NFC antenna loop + tuning cap to GND | 🔶 |
| 13, 36, 48 | VDD | +3V3 | 🔶 |
| 21 | P0.18 | U2 flash CS# | ✅ |
| 22 | P0.19 | U2 flash SO (MISO) | ✅ |
| 23 | P0.20 | **SW4 RETURN (back)** | ✅ |
| 24 | P0.21 / nRESET | U5 Hall sensor output: a magnet resets the chip or wakes it from sleep (exact path not traced) | ✅ |
| 25 | SWDCLK | Not found on PC_V2.0 (xiaohai: J2 + USB-C D−) | ❓ |
| 26 | SWDIO | Not found on PC_V2.0 (xiaohai: J2 + USB-C D+) | ❓ |
| 30 | ANT | L1 / C9 matching network → PCB meander antenna | 🔶 |
| 31, 45, EP | VSS | GND | 🔶 |
| 32 | DEC2 | Decoupling cap | 🔶 |
| 33 | DEC3 | Decoupling cap | 🔶 |
| 34 | XC1 | X2 32 MHz + load cap | 🔶 |
| 35 | XC2 | X2 32 MHz + load cap | 🔶 |
| 37 | P0.25 | SPI MOSI: OLED D1 (SDIN) + flash SI | ✅ |
| 38 | P0.26 | SPI SCK: OLED D0 (SCLK) + flash SCLK | ✅ |
| 39 | P0.27 | OLED CS# | ✅ |
| 40 | P0.28 | OLED D/C# | ✅ |
| 41 | P0.29 | OLED RES# | ✅ |
| 42 | P0.30 | Backlight on the LCD build; not used by the OLED | ❓ |
| 43 | P0.31 | Status LED in firmware; not used, PC_V2.0 has no LED | ✅ |
| 46 | DEC4 | Decoupling cap | 🔶 |
| 47 | DCC | Not connected (LDO mode, no DC/DC inductor) | 🔶 |

All other GPIOs are unused. All four buttons connect the GPIO to GND when pressed and use the nRF52832's internal pull-ups (`NRF_GPIO_PIN_PULLUP`), so there are no external button resistors ✅.

SW4 is printed "RST / RETURN", but it is an ordinary GPIO button on P0.20 and does not reset the chip in hardware ✅.

### Firmware key mapping

| Physical | Silkscreen | GPIO | `input_key_t` |
| --- | --- | --- | --- |
| Far right | SW1 OK | P0.06 | `INPUT_KEY_CENTER` |
| Middle-right | SW2 DOWN | P0.05 | `INPUT_KEY_RIGHT` |
| Middle-left | SW3 UP | P0.07 | `INPUT_KEY_LEFT` |
| Far left | SW4 RST/RETURN | P0.20 | `INPUT_KEY_BACK` |

## Power

```
VBUS ─┬─► U3 VCC        U3 BAT ─┬─► B+ ── LiPo 402030
      │                         ├─► R14 ─┬─► P0.02 (ADC)
      │                         │        └─► R15 ─► GND
      │                         │
      │                         └─► Q3 drain
      ├─► Q3 gate                   Q3 source ─┐
      ├─► R16 ─► GND                           ├─► VSYS ─► U4 VIN ─► +3V3
      └─► D1 anode       D1 cathode ───────────┘

USB-C CC1 / CC2 ─► R17 / R18 ─► GND
U3 PROG ─► R13 ─► GND
```

A standard load-sharing power path, confirmed by voltage measurements ✅:

- **On USB:** VBUS (5.26 V) holds Q3's gate high, so Q3 is off. The board runs from USB through D1 (VSYS = 4.99 V, a 0.28 V Schottky drop), and the charger only charges the battery.
- **On battery:** R16 pulls Q3's gate to GND, Q3 turns on, and the battery feeds the LDO.

Measured on USB power with the battery disconnected, relative to B−:

| Net | Voltage | Points |
| --- | --- | --- |
| VBUS | 5.26 V | D1 anode, Q3 gate, U3 VCC, R16 |
| VSYS (LDO input) | 4.99 V | D1 cathode, Q3 source, U4 VIN |
| BAT | 4.25–4.38 V (no battery, charger output drifts) | U3 BAT, Q3 drain, R14 |
| +3V3 | 3.34 V | U4 VOUT |
| Battery divider | 2.90 V | R14/R15 junction |
| PROG | ~0.05 V (not charging) | U3 PROG, R13 |
| CC (cable side in use) | 1.64 V | R18 |

The 2.90 V divider reading matches xiaohai's 806 kΩ / 2 MΩ divider once the meter's 10 MΩ input load is included, so R14 / R15 most likely have those values 🔶.

### U3: Li-ion charger, SOT-23-5, marking `55b2`

Probably a TP4055 (xiaohai uses a TP4055) 🔶. The measured pins match the TP4054/TP4055 pinout ✅:

| Pin | Name | Net | Measured |
| --- | --- | --- | --- |
| 1 | CHRG# | P0.03 (open-drain, low while charging) | 3.3 V (pulled up, not charging) |
| 2 | GND | GND | 0 V |
| 3 | BAT | B+ | 4.25–4.38 V |
| 4 | VCC | VBUS | 5.26 V |
| 5 | PROG | R13 to GND; sets the charge current (I = 1000 V / R<sub>PROG</sub>; xiaohai uses 5 kΩ ≈ 200 mA) | 0.05 V |

### Q3: power-path P-MOSFET, SOT-23, marking `A19T` ✅

AO3401A. Gate on VBUS, source on VSYS, drain on BAT ✅ (measured).

### D1: Schottky diode, marking `S4`

Anode on VBUS, cathode on VSYS ✅ (measured). Probably a 1N5819W 🔶.

### U4: 3.3 V LDO, SOT-23, marking `662K` ✅

XC6206P332MR. Pinout confirmed by measurement ✅.

| Pin | Name | Net | Measured |
| --- | --- | --- | --- |
| 1 | VSS | GND | 0 V |
| 2 | VOUT | +3V3 | 3.34 V |
| 3 | VIN | VSYS | 4.98 V |

### U5: Hall sensor, SOT-23, marking `6201` ✅

CrossChip CC6201ST, a micropower omnipolar Hall switch (reacts to either magnetic pole). It works like the reset on xiaohai's board ✅:

- Device on: hold a magnet against the back, over U5. The screen goes off and comes back about a second later (the chip resets).
- Device asleep: a magnet wakes it up.

PC_V2.0 has no second MOSFET for this, so U5's output probably drives nRESET directly 🔶.

## Display (PC1)

A 1.3" 128×64 OLED with an SH1106 driver, on an FPC marked `ZFP1106-34AX` ✅, driven over 4-wire SPI ✅.

PC1 is **not** xiaohai's connector. On xiaohai's board, the panel's 30-pin tail is soldered straight to the PCB. On PC_V2.0 the panel has a shorter tail that plugs into a **16-pin** ZIF connector ✅. On the panel's ribbon, 15 traces run to the contacts and 3 more end early at vias. The full PC1 pinout is not confirmed yet.

Voltages measured on each PC1 leg (USB power, display on, relative to B−). Legs are numbered 1–16 from the left, viewing the back with `PC_V2.0` at the top right:

| Leg | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| V | 0.2 | 3.4 | 7.98 | 3.12 | 3.34 | 0 | 3.34 | 3.34 | 3.34 | 3.34 | 0 | 0 | 5.6 | 8.0 | 8.03 | 0 |

What the voltages suggest 🔶:
- **Legs 3, 14, 15 (~8 V):** the SH1106's internal high-voltage supply (VCC / VPP).
- **Leg 13 (5.6 V):** VCOMH (0.7 × VCC).
- **Legs 5, 7–10 (3.34 V):** VDD and CS#, RES#, D/C# held high.
- **Legs 6, 11, 12, 16 (0 V):** GND, plus SCK and MOSI sitting idle.

The SH1106 still needs the same external parts, so R3 and C21–C27 next to PC1 are most likely the IREF resistor and the capacitors for the charge pump, VCOMH, VCC and VDD 🔶. For reference, this is xiaohai's 30-pin layout (connector J3):

| Pin | SH1106 function | Net (xiaohai) |
| --- | --- | --- |
| 1 | NC / GND | GND |
| 2, 3 | C2P, C2N | 1 µF charge-pump capacitor between pins 2 and 3 |
| 4, 5 | C1P, C1N | 1 µF charge-pump capacitor between pins 4 and 5 |
| 6 | VBAT (charge-pump supply) | +3V3 |
| 7 | NC | — |
| 8 | VSS | GND |
| 9 | VDD | +3V3 |
| 10, 11, 12 | BS0, BS1, BS2 | GND (pins 10–11 through 4.7 kΩ / 0 Ω): selects 4-wire SPI |
| 13 | CS# | P0.27 |
| 14 | RES# | P0.29 |
| 15 | D/C# | P0.28 |
| 16, 17 | R/W#, E/RD# | GND |
| 18 | D0 (SCLK) | P0.26 |
| 19 | D1 (SDIN) | P0.25 |
| 20 | D2 | NC |
| 21–25 | D3–D7 | GND |
| 26 | IREF | 560 kΩ to GND |
| 27 | VCOMH | 2.2 µF to GND |
| 28 | VCC (charge-pump output) | 2.2 µF + 1 µF to GND |
| 29, 30 | VLSS, NC | GND |

## SPI flash (U2)

U2 is a Zbit ZD25WQ16, a 16 Mbit SPI NOR flash in USON-8 (3 × 2 mm) ✅. It is populated and required: the firmware stores its files on it (SPIFFS).

| Pin | Name | Net |
| --- | --- | --- |
| 1 | CS# | P0.18 ✅ |
| 2 | SO | P0.19 ✅ |
| 3 | WP# | Pull-up to +3V3 (R1 or R2 🔶) |
| 4 | GND | GND |
| 5 | SI | P0.25 ✅ |
| 6 | SCLK | P0.26 ✅ |
| 7 | HOLD# | Pull-up to +3V3 (R1 or R2 🔶) |
| 8 | VCC | +3V3, decoupled by C12 🔶 |

## Bill of Materials

Values come from xiaohai's design wherever the role matches. Ceramic capacitors carry no markings, so their values on PC_V2.0 cannot be read from photos. Designators hidden under the battery are not listed.

| Designator | Description | Package | Suggested part | |
| --- | --- | --- | --- | --- |
| U1 | BLE + NFC-A SoC, 512 KB flash / 64 KB RAM | QFN-48 6×6 mm | Nordic nRF52832-QFAA | ✅ |
| U2 | 16 Mbit SPI NOR flash | USON-8 3×2 mm | Zbit ZD25WQ16 (alt. GD25Q16) | ✅ |
| U3 | Li-ion linear charger | SOT-23-5 | TP4055 (marking `55b2`) | 🔶 |
| U4 | 3.3 V LDO | SOT-23 | Torex XC6206P332MR (marking `662K`) | ✅ |
| U5 | Omnipolar Hall switch, magnet reset | SOT-23 | CrossChip CC6201ST (marking `6201`) | ✅ |
| Q3 | P-channel MOSFET, power path | SOT-23 | AO3401A (marking `A19T`) | ✅ |
| D1 | Schottky diode, power path | SOD-123 | 1N5819W / B5819W (marking `S4`) | 🔶 |
| X1 | 32.768 kHz crystal | SMD 3.2×1.5 mm | marking `YG604A` | 🔶 |
| X2 | 32 MHz crystal | SMD 4-pad | YXC 32.000 MHz | ✅ |
| C1, C2 | X1 load capacitors | 0402 | 12 pF C0G | 🔶 |
| C3, C4 | X2 load capacitors | 0402 | 12 pF C0G | 🔶 |
| C5–C8, C10, C11 | MCU DEC / VDD decoupling | 0402 | 100 nF, 1 µF, 4.7 µF (exact mapping ❓) | ❓ |
| L1, C9 | 2.4 GHz antenna matching | 0603 / 0402 | 3.9 nH + 0.8 pF | 🔶 |
| C12 | Flash decoupling | 0402 | 100 nF | 🔶 |
| R1, R2 | Flash WP# / HOLD# pull-ups | 0402 | 4.7 kΩ | 🔶 |
| R3 | OLED IREF | 0402 | 560 kΩ | 🔶 |
| C21–C27 | OLED charge-pump, VCOMH, VCC, VDD capacitors | 0402 / 0603 | 1 µF, 2.2 µF | 🔶 |
| C16–C19 | Power-section capacitors (charger, LDO in/out) | 0402 / 0603 / 0805 | 1–10 µF | 🔶 |
| R13 | Charger PROG (sets charge current) | 0402 | ❓ (xiaohai: 5 kΩ ≈ 200 mA) | ✅ role |
| R14, R15 | Battery voltage divider to P0.02 | 0402 | 806 kΩ (top), 2 MΩ (bottom) | ✅ role, 🔶 values |
| R16 | Q3 gate pull-down | 0402 | ❓ (typically 10–100 kΩ) | ✅ role |
| R17, R18 | USB-C CC1 / CC2 pull-downs | 0402 | 5.1 kΩ | ✅ role, 🔶 values |
| SW1–SW4 | Tactile switch, 4-pad SMD | ~5.2×5.2 mm | generic | ✅ |
| PC1 | FPC/ZIF connector for the OLED, 16 pins | SMD, pitch ❓ | generic | ✅ count |
| — | USB-C receptacle (power only) | SMD | generic 16-pin | 🔶 |
| — | 1.3" 128×64 OLED, SH1106, 30-pin FPC `ZFP1106-34AX` | — | generic | ✅ |
| BT | LiPo cell with protection board | 402030 | 3.7 V 200 mAh | ✅ |
| — | NFC antenna | PCB copper loop | — | ✅ |

## Open items

### PC1 signal pinout: logic analyzer

A logic analyzer (for example a Saleae) can identify all five display signals at once, which a multimeter can't do for CS#, D/C# and RES#:

1. Connect the analyzer's ground to the B− pad.
2. Put the channels on the PC1 legs that read 0–3.4 V: legs 1, 2, 4–12 and 16. Use fine test hooks, or solder thin wires to the legs.
3. **Leave legs 3, 13, 14 and 15 alone.** They carry 5.6–8 V, which is more than many analyzers (especially the cheap clones) can tolerate.
4. Start capturing, then plug in USB so the device boots, and press a few buttons so the screen redraws.

How to read the capture:

| Signal | What it looks like |
| --- | --- |
| RES# | One low pulse at boot, then stays high |
| CS# | Goes low for each burst of data, high in between |
| SCK | Clock bursts while CS# is low |
| MOSI | Changing data while SCK runs |
| D/C# | Low during command bytes (mostly at boot), high during screen data |

To check the result, add the SPI decoder on SCK / MOSI / CS#. Right after the RES# pulse it should show the SH1106 start-up commands (for example `0xAE`, display off). The remaining 0 V and 3.3 V legs are GND and VDD.

### Multimeter

- **PC1 support parts:** which legs connect to R3 and C21–C27 (continuity), plus R3's value.
- **R13, R16:** values (resistance, with all power removed).
- **PC1:** connector pitch (measure with calipers).
- **NFC tuning caps:** which capacitors connect to MCU pins 11 and 12.
- **SWD:** whether USB-C D+ / D− connect to MCU pins 26 / 25, as on xiaohai's board.

## License

This documentation is derived from xiaohai's OLED hardware and is released under the same license, **GPL-2.0-or-later**. xiaohai's README also asks that the design not be used for commercial purposes.
