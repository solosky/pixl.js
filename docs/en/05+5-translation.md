# Translation

## Firmware

## How to update existing translation

When editing CSV files with VSCode, the extension [Edit CSV](https://marketplace.visualstudio.com/items?itemName=janisdd.vscode-edit-csv) is recommended.

**Windows** requires you to have [Python](https://www.python.org/downloads/) and [Git](https://git-scm.com/download/windows) installed and within your `$PATH`  
For **Linux**, most distributions come with `python` and `git` by default and **macOS** comes bundled with `python`, but to use `git` you either need to install the Command Line tools via `xcode-select –-install`, or download [Git](https://git-scm.com/download/mac) separately.

### Windows

1. Clone this repository
   `git clone https://github.com/solosky/pixl.js.git; cd pixl.js`
2. Edit `fw/data/i18n.csv`
3. Run `py.exe fw/scripts/i18n_gen.py` to generate new language files.
4. _Optional:_ Run `py.exe fw/scripts/font_data_gen.py` to generate new font data if you add new characters in `i18n.csv`
5. [Build the Firmware](03-Build-Firmware.md)

### Linux and macOS

1. Clone this repository
   `git clone https://github.com/solosky/pixl.js.git; cd pixl.js`
2. Edit `fw/data/i18n.csv`
3. Run `python3 fw/scripts/i18n_gen.py` to generate new language files.
4. Optional: Run `python fw/scripts/font_data_gen.py` to generate new font data if you add new characters in `i18n.csv`
5. [Build the Firmware](03-Build-Firmware.md)

## How to add new language translation

The process is similar to updating an existing translation, and has the same requirements depending on your Operating System.

1. Add new column in `fw/data/i18n.csv`, for example "ja_JP"
2. Run `fw/scripts/i18n_gen.py` to generate new language files.
3. Optional: Run `fw/scripts/font_data_gen.py` to generate new font data if you add new characters in `i18n.csv`
4. Edit `fw/application/src/i18n/language.h` and `fw/application/src/i18n/language.c` to adopt new language
5. Edit Makefile to include `$(PROJ_DIR)/i18n/ja_JP.c` as C source files
6. [Build the Firmware](03-Build-Firmware.md)

## Font notes

For release builds (RELEASE=1), the firmware uses wenquanyi_9pt_u8g2.bdf to display unicode characters.  
Please check if the new language character codepoint is included in wenquanyi_9pt_u8g2.bdf or not.  
If not, it's not recommended to support the new language due to the MCU internal flash restrictions.

## Web App

### How to update existing translation

The language file can be found under `web/src/i18n'

## How to add new language translation

In this example, we will add a Japanese (jp_JP) translation:

1. Copy the `en_US.js` file and name it `jp_JP.js`
2. Translate the string within `jp_JP.js` including the `changeok` message.

   Don't translate the other language names.

   Add your language at the bottom of the `lang {` section:

   `jp: '日本語',`

3. Edit `index.js` while keeping it's current structure, adding:

   ```js
   import elementJpLocale from 'element-ui/lib/locale/lang/jp' // element-ui
   lang import jpLocale from './jp_JP' elementJpLocale
   ```

     And extending `messages`:

     ```js
       jp_JP: {
    ...jpLocale,
    ...elementJpLocale,
     },
     ```

4. Add your language to the other `.js` files (within the `lang: {` section):

   `vuejp: '日本語',`

5. Add your language to the `web/src/App.vue` file:

```js
<el-dropdown-item Enabled="language==='jp'" command="jp" divided>
 {{ $t('lang.jp') }}
 </el-dropdown-item>
```
