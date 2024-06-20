#!/usr/bin/python3

import sys
import io
import os
from PIL import Image, ImageOps
from struct import *

RESOURCE_H_HEADER = '''
#ifndef MUI_RESOURCE_H
#define MUI_RESOURCE_H

#include <stdint.h>

typedef struct {
    const uint8_t width;
    const uint8_t height;
    const uint8_t *data;
} xbm_t;
'''

RESOURCE_H_TAILER = '''
#endif
'''

RESOURCE_C_HEADER = '''
#include "mui_resource.h"
'''

RESOURCE_C_TAILER = '''
'''

def get_prorject_directory():
    return os.path.abspath(os.path.dirname(__file__)+"/../")

def get_im_size(file):
    with Image.open(file) as im:
        return im.size


def write_xbm(data, fp):
    lines = str(data).split("\\n")
    for line in lines:
        if line.startswith("0x"):
            fp.write(line)
            fp.write("\n")


def get_xbm(file):
    with Image.open(file) as im:
        with io.BytesIO() as output:
            bw = im.convert("1")
            bw = ImageOps.invert(bw)
            bw.save(output, format="xbm")
            return output.getvalue()
        
def gen_bmp():
    
    project_dir = get_prorject_directory()
    c_file_name = project_dir + "/application/src/mui/mui_resource.c"
    c_file = open(c_file_name, "w+", newline="\n", encoding="utf8")
    c_file.write(RESOURCE_C_HEADER)

    h_file_name = project_dir + "/application/src/mui/mui_resource.h"
    h_file = open(h_file_name, "w+", newline="\n", encoding="utf8")
    h_file.write(RESOURCE_H_HEADER)

    files = os.listdir(project_dir + "/resources/bmp")
    files.sort()

    for file in files:
        print("process %s .." % file)        
        file_name_abs = project_dir + "/resources/bmp/" + file
        if os.path.isdir(file_name_abs):
            continue

        img_size = get_im_size(file_name_abs)
        xbm_data = get_xbm(file_name_abs)

        file_name, file_extension = os.path.splitext(file)

        h_file.write("extern const xbm_t "+file_name+";\n")

        c_file.write("//************************************************************************\n")
        c_file.write("//** /resources/bmp/%s\n" % (file))
        c_file.write("//************************************************************************\n")

        c_file.write("const uint8_t %s_data[] = {\n" % (file_name))
        write_xbm(xbm_data, c_file)
        c_file.write("};\n")
        c_file.write("const xbm_t %s = {.width = %d, .height = %d, .data = %s_data};\n\n\n" % (file_name, img_size[0], img_size[1], file_name))


    c_file.write(RESOURCE_C_TAILER)
    h_file.write(RESOURCE_H_TAILER)

    c_file.close()
    h_file.close()

def main():
    gen_bmp()

######
main()
