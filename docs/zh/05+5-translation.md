# 翻译 

## 固件

### 如何更新现有的翻译

使用 VSCode 编辑 CSV 文件时，推荐使用扩展 [Edit csv](https://marketplace.visualstudio.com/items?itemName=janisdd.vscode-edit-csv)。

**Windows** 需要安装 [Python](https://www.python.org/downloads/) 和 [Git](https://git-scm.com/download/windows)，并加入 `$PATH` 环境变量。  
**Linux** 大多数发行版默认自带 `python3` 和 `git`；**macOS** 自带 `python3`，但使用 `git` 需要通过 `xcode-select --install` 安装命令行工具，或单独下载 [Git](https://git-scm.com/download/mac)。

#### Windows

1. 克隆本仓库
   `git clone https://github.com/solosky/pixl.js.git; cd pixl.js`
2. 编辑 `fw/data/i18n.csv`
3. 运行 `py.exe fw/scripts/i18n_gen.py` 生成新的语言文件。
4. 可选：如果在 `i18n.csv` 中添加了新字符，运行 `py.exe fw/scripts/font_data_gen.py` 生成新的字体数据。
5. [构建固件](03-Build-Firmware.md)

#### Linux 和 macOS

1. 克隆本仓库
   `git clone https://github.com/solosky/pixl.js.git; cd pixl.js`
2. 编辑 `fw/data/i18n.csv`
3. 运行 `python3 fw/scripts/i18n_gen.py` 生成新的语言文件。
4. 可选：如果在 `i18n.csv` 中添加了新字符，运行 `python3 fw/scripts/font_data_gen.py` 生成新的字体数据。
5. [构建固件](03-Build-Firmware.md)

### 如何添加新的语言翻译

流程与更新现有翻译类似，环境要求也取决于你的操作系统。

1. 在 `fw/data/i18n.csv` 中添加新的列，例如 "ja_JP"。
2. 运行 `fw/scripts/i18n_gen.py` 生成新的语言文件。
3. 可选：如果在 `i18n.csv` 中添加了新字符，运行 `fw/scripts/font_data_gen.py` 生成新的字体数据。
4. 编辑 `fw/application/src/i18n/language.h` 和 `fw/application/src/i18n/language.c` 以支持新语言。
5. 编辑 Makefile，将 `$(PROJ_DIR)/i18n/ja_JP.c` 加入 C 源文件。
6. [构建固件](03-Build-Firmware.md)

### 字体说明

对于发布版本（RELEASE=1），固件使用 wenquanyi_9pt_u8g2.bdf 来显示 Unicode 字符。  
请检查新语言的字符代码点是否包含在 wenquanyi_9pt_u8g2.bdf 中。  
如果没有，由于 MCU 内部闪存的限制，不建议添加新语言支持。

## 网页端

### 如何更新现有的翻译

语言文件位于 `web/src/i18n` 目录下。

### 如何添加新的语言翻译

以添加日语（ja_JP）翻译为例：

> 注意：项目中已经注册了 `ja_JP`（日语），网页端也已包含日语选项。此示例仅作通用说明；实际添加新语言时，请将示例中的 `ja_JP` 替换为尚未注册的 locale（例如 `ko_KR`），并同步调整各处引用。

1. 复制 `en_US.js` 文件，命名为 `ja_JP.js`
2. 翻译 `ja_JP.js` 中的字符串，包括 `changeok` 消息。

   不要翻译其他语言的名称。

   在 `lang {` 部分的末尾添加你的语言：

   `ja: '日本語',`

3. 编辑 `web/src/i18n/index.js`，保持原有结构，添加：

   ```js
   import elementJaLocale from 'element-ui/lib/locale/lang/ja' // element-ui
   import jaLocale from './ja_JP'
   ```

   并扩展 `messages`：

   ```js
     ja_JP: {
   ...jaLocale,
   ...elementJaLocale,
    },
   ```

4. 在其他 `.js` 文件中（`lang: {` 部分内）添加你的语言：

   `ja: '日本語',`

5. 在 `web/src/App.vue` 文件中添加：

```js
<el-dropdown-item Enabled="language==='ja_JP'" command="ja_JP" divided>
 {{ $t('lang.ja') }}
 </el-dropdown-item>
```
