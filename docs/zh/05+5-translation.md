# 翻译

## 如何更新现有的翻译

使用您喜欢的文本编辑器编辑 `fw/data/i18n.csv`。对于VSCode，推荐使用扩展程序 "Edit csv"。
运行 `fw/scripts/i18n_gen.py` 以生成新的语言文件。
可选步骤：如果您在 i18n.csv 中添加了新字符，则运行 `fw/scripts/font_data_gen.sh` 生成新的字体数据。
运行 `make full` 重新构建固件。

注意: <br />
`font_data_gen.sh` 脚本只能在Windows的Git Bash中运行。

## 如何添加新的语言翻译

1. 在 `fw/data/i18n.csv` 中添加新的列，例如 "ja_JP"。
2. 运行 `fw/scripts/i18n_gen.py` 生成新的语言文件。
3. （可选）如果在 i18n.csv 中添加了新字符，则运行 `fw/scripts/font_data_gen.sh` 生成新的字体数据。
4. 编辑 `fw/application/src/i18n/language.h` 和 `fw/application/src/i18n/language.c` 以适应新语言。
5. 编辑Makefile以将 `$(PROJ_DIR)/i18n/ja_JP.c` 包含为C源文件。
6. 运行 `make full` 重新构建固件。

## 字体说明

对于发布版本（RELEASE=1），固件使用 wenquanyi_9pt_u8g2.bdf 来显示Unicode字符。<br />
请检查新语言字符的代码点是否包含在 wenquanyi_9pt_u8g2.bdf 中。<br />
如果没有，由于MCU内部闪存的限制，我不建议添加新语言支持。
