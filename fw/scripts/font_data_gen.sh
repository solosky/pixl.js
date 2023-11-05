#!/bin/bash

cat application/src/i18n/*.c application/src/amiidb/*.c  |grep -Po '".*?"' | tr -d '[:print:]' |sort|uniq > data/pixjs.txt
echo '32-128,' > data/gb2312a.map
cat data/gb2312a.txt data/pixjs.txt  | iconv -f utf-8 -t c99 | sed 's/\\u\([0-9a-f]\{4\}\)/\$\1,\n/g' | sort | uniq | sed '/^$/d' | tr '/a-f/' '/A-F/' >> data/gb2312a.map
scripts/bdfconv -b 0 -f 1 -M data/gb2312a.map -n u8g2_font_wqy12_t_gb2312a -o application/src/mui/u8g2_font_wqy12_t_gb2312a_t.c data/wenquanyi_9pt.bdf
echo '''
#include "mui_u8g2.h"

#include "u8x8.h"

''' > application/src/mui/u8g2_font_wqy12_t_gb2312a.c
cat application/src/mui/u8g2_font_wqy12_t_gb2312a_t.c >> application/src/mui/u8g2_font_wqy12_t_gb2312a.c
sed -i 's/U8G2_USE_LARGE_FONTS/U8G2_USE_LARGE_GB2312_FONT/g' application/src/mui/u8g2_font_wqy12_t_gb2312a.c
rm application/src/mui/u8g2_font_wqy12_t_gb2312a_t.c
rm data/gb2312a.map data/pixjs.txt 