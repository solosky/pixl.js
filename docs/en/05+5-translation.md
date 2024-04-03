# Transalation 

## Firmware

## How to update existing translation

When editing CSV files with VSCode, the extension [Edit CSV](https://marketplace.visualstudio.com/items?itemName=janisdd.vscode-edit-csv) is recommended.


### Windows

Windows requires you to have [Python](https://www.python.org/downloads/) and [Git Client](https://git-scm.com/downloads) installed and within your `$PATH`


1. Clone this repository
 `git clone https://github.com/solosky/pixl.js.git`
2. Edit `fw/data/i18n.csv`
3. Run `py.exe fw/scripts/i18n_gen.py` to generate new language files.
4. Optional: Run `bash fw/scripts/font_data_gen.sh` to generate new font data if you add new charaters in `i18n.csv`
5. [Build the Firmware](03-Build-Firmware.md)


### Linux
You will need to build [bdfconv](https://github.com/olikraus/u8g2/tree/master/tools/font/bdfconv) yourself and place it within `$PATH`.

`git`, `grep`, and `sed` should be included with your distribtion.

1. Clone this repository
 `git clone https://github.com/solosky/pixl.js.git`
2. Edit `fw/data/i18n.csv`
3. Run `python3 fw/scripts/i18n_gen.py` to generate new language files.
4. Optional: Run `bash fw/scripts/font_data_gen.sh` to generate new font data if you add new charaters in `i18n.csv`
5. [Build the Firmware](03-Build-Firmware.md)


### MacOS
You will need to build [bdfconv](https://github.com/olikraus/u8g2/tree/master/tools/font/bdfconv) yourself and place it within `$PATH`.
This requires to have XCode or the Command Line Utilities to be installed.

You can either manually install them via
`xcode-select –-install` or they will automatically installed when installing Homebrew (see next step) 

In addition you will need the GNU version of `grep` and `sed` as the buil-in versions are not compatible.
If you have [Homebrew](https://brew.sh) installed, you can install those via:
`brew install grep`
`brew install gnu-sed`

1. Clone this repository
 `git clone https://github.com/solosky/pixl.js.git`
2. Edit `fw/data/i18n.csv`
3. Run `python3 fw/scripts/i18n_gen.py` to generate new language files.
4. Optional: Run `bash fw/scripts/font_data_gen.sh` to generate new font data if you add new charaters in `i18n.csv`
5. [Build the Firmware](03-Build-Firmware.md)


## How to add new language translation
The process is similar to updating an existing translation, and has the same requirements dependin on your Operating System.

1. Add new column in `fw/data/i18n.csv`, for example "ja_JP"
2. Run `fw/scripts/i18n_gen.py` to generate new language files.
4. Optional: Run `bash fw/scripts/font_data_gen.sh` to generate new font data if you add new charaters in `i18n.csv`
4. Edit `fw/application/src/i18n/language.h` and `fw/application/src/i18n/language.c` to adopt new language
5. Edit Makefile to include `$(PROJ_DIR)/i18n/ja_JP.c` as C source files
5. [Build the Firmware](03-Build-Firmware.md)

## Font notes 

For release builds (RELEASE=1), the firmware uses wenquanyi_9pt_u8g2.bdf to display unicode characters. <br />
Please check if the new language charater codepoint is included in wenquanyi_9pt_u8g2.bdf or not.<br />
If not, it's not recommended to support the new language due to the MCU internal flash restrction. 
