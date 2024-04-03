#!/bin/bash

# Check the operating system
if [[ "$OSTYPE" == "darwin"* ]]; then
    # MacOS
    GREP="ggrep"
    SED="gsed"
    BDFCONV="bdfconv"  # Assuming bdfconv is in PATH
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    GREP="grep"
    SED="sed"
    BDFCONV=$(command -v bdfconv)  # Check if bdfconv is in PATH
else
    # Windows
    GREP="grep"
    SED="sed"
    BDFCONV="scripts/bdfconv"  # Assuming bdfconv is in scripts folder
fi

# Change directory to script's directory
cd "$(dirname "$0")" && cd ..

# Generate pixjs.txt file
cat application/src/i18n/*.c application/src/amiidb/*.c | $GREP -Po '".*?"' | tr -d '[:print:]' | sort | uniq > data/pixjs.txt

# Create gb2312a.map file
echo '32-128,' > data/gb2312a.map
cat data/chinese3.txt data/pixjs.txt | iconv -f utf-8 -t c99 | $SED 's/\\u\([0-9a-f]\{4\}\)/\$\1,\n/g' | sort | uniq | $SED '/^$/d' | tr '/a-f/' '/A-F/' >> data/gb2312a.map

# Convert font using bdfconv
$BDFCONV -b 0 -f 1 -M data/gb2312a.map -n u8g2_font_wqy12_t_gb2312a -o application/src/mui/u8g2_font_wqy12_t_gb2312a_t.c data/wenquanyi_9pt_u8g2.bdf

# Generate u8g2_font_wqy12_t_gb2312a.c file
echo '''
#include "mui_u8g2.h"

#include "u8x8.h"

''' > application/src/mui/u8g2_font_wqy12_t_gb2312a.c
cat application/src/mui/u8g2_font_wqy12_t_gb2312a_t.c >> application/src/mui/u8g2_font_wqy12_t_gb2312a.c
$SED -i 's/U8G2_USE_LARGE_FONTS/U8G2_USE_LARGE_GB2312_FONT/g' application/src/mui/u8g2_font_wqy12_t_gb2312a.c

# Clean up temporary files
rm application/src/mui/u8g2_font_wqy12_t_gb2312a_t.c
rm data/gb2312a.map data/pixjs.txt