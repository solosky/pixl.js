#!/usr/bin/env python3

import os
import re
import subprocess
import platform


# Absolute path to the directories containing the .c files
current_dir = os.path.dirname(os.path.abspath(__file__))
source_dirs = [
    os.path.join(current_dir, "../application/src/i18n"),
    os.path.join(current_dir, "../application/src/amiidb")
]

# Absolute path to the directory where the output file will be saved
data_dir = os.path.join(current_dir, "../data")
mui_dir = os.path.join(data_dir, "../application/src/mui")

# Name of the output file
output_file = "pixjs.txt"


def write_to_file(file_path, content):
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)


def extract_non_printable_chars():
    non_printable_chars = set()
    for source_dir in source_dirs:
        for root, _, files in os.walk(source_dir):
            for file in files:
                if file.endswith(".c"):
                    with open(os.path.join(root, file), "r", encoding="utf-8") as f:
                        content = f.read()
                        non_printable_chars.update(re.findall(r'[^\x20-\x7E]', content))

    write_to_file(os.path.join(data_dir, output_file), '\n'.join(sorted(non_printable_chars)))


def convert_and_sort(unicode_strings):
    return sorted(set(f'${ord(char):04X},' for string in unicode_strings for char in string))


def run_bdfconv(map_path, output_path, bdf_path):
    # Construct the path to the bdfconv executable relative to the script directory
    system = platform.system()
    if system == 'Windows':
        bdfconv_path = os.path.join(current_dir, 'bdfconv.exe')
    elif system == 'Darwin':
        bdfconv_path = os.path.join(current_dir, 'bdfconv_macos_universal')
    elif system == 'Linux':
        bdfconv_path = os.path.join(current_dir, 'bdfconv_linux')
    else:
        raise OSError("Unsupported operating system")

    # Convert paths to absolute paths
    abs_map_path = os.path.abspath(map_path)
    abs_output_path = os.path.abspath(output_path)
    abs_bdf_path = os.path.abspath(bdf_path)

    cmd = [
        bdfconv_path, '-b', '0', '-f', '1', '-M', abs_map_path, 
        '-n', 'u8g2_font_wqy12_t_gb2312a', '-o', abs_output_path, abs_bdf_path
    ]
    subprocess.run(cmd, check=True)


def main():
    extract_non_printable_chars()

    combined_content = ''
    for file_name in ['chinese3.txt', 'pixjs.txt']:
        with open(os.path.join(data_dir, file_name), 'r', encoding='utf-8') as file:
            combined_content += file.read()

    sorted_converted = convert_and_sort(combined_content.splitlines())
    write_to_file(os.path.join(data_dir, 'gb2312a.map'), '32-128,\n' + '\n'.join(sorted_converted))

    run_bdfconv(
        map_path=os.path.join(data_dir, 'gb2312a.map'),
        output_path=os.path.join(mui_dir, 'u8g2_font_wqy12_t_gb2312a_t.c'),
        bdf_path=os.path.join(data_dir, 'wenquanyi_9pt_u8g2.bdf')
    )

    # Write to u8g2_font_wqy12_t_gb2312a.c
    with open(os.path.join(mui_dir, 'u8g2_font_wqy12_t_gb2312a.c'), 'w+', encoding='utf-8') as final_file:
        final_file.write('''
#include "mui_u8g2.h"

#include "u8x8.h"

''')

        # Read the content of the temporary file and append it to the final file
        temp_file_path = os.path.join(mui_dir, 'u8g2_font_wqy12_t_gb2312a_t.c')
        with open(temp_file_path, 'r', encoding='utf-8') as temp_file:
            final_file.write(temp_file.read())

    # Remove the temporary files
    os.remove(temp_file_path)
    os.remove(os.path.join(data_dir, output_file))
    os.remove(os.path.join(data_dir, 'gb2312a.map'))


if __name__ == "__main__":
    main()
