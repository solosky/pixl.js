# Transalation 

## How to update exists translation

1. Edit fw/data/i18n.csv with your favorite text editor. For vscode, extension "Edit csv" is recommended.
2. Run `fw/scripts/i18n_gen.py` to generate new language files.
3. Optional: Run `fw/scripts/font_data_gen.sh` to generate new font data if you add new charaters in i18n.csv

note: <br />
The script "font_data_gen.sh" only can be run in Git Bash windows in Windows. please cd fw and run "scripts/font_data_gen.sh". 

## How to add new language translation

1. Add new column in fw/data/i18n.csv, for example "ja_JP"
2. Run `fw/scripts/i18n_gen.py` to generate new language files.
3. Optional: Run `fw/scripts/font_data_gen.sh` to generate new font data if you add new charaters in i18n.csv
4. Edit `fw/application/src/i18n/language.h` and `fw/application/src/i18n/language.c` to adopt new language
5. Edit Makefile to include `$(PROJ_DIR)/i18n/ja_JP.c` as C source files
6. Run `make -j8` rebuild firmware

## Font notes 

For release build (RELEASE=1), the firmware use wenquanyi_9pt_u8g2.bdf to display unicode characters.  <br />
Please check new language charater codepoint is included in wenquanyi_9pt_u8g2.bdf or not.<br />
If not, I am not recommended to support the new language due to the MCU internal flash restrction. 