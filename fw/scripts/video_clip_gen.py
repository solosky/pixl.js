#!/usr/bin/python3

import sys
import io
import os
from PIL import Image, ImageOps
from struct import *

def get_im_size(file):
    with Image.open(file) as im:
        return im.size


def write_xbm(data, fp):
    byte_count = 0
    lines = str(data).split("\\n")
    for line in lines:
        if line.startswith("0x"):
            hex_list = line.split(",")
            for h in hex_list:
                if h.startswith("0x"):
                    fp.write(bytes.fromhex(h[2:]))
                    byte_count = byte_count + 1
    return byte_count


def get_xbm(file):
    with Image.open(file) as im:
        with io.BytesIO() as output:
            bw = im.convert("1")
            bw = ImageOps.invert(bw)
            bw.save(output, format="xbm")
            return output.getvalue()

def main():
    if len(sys.argv) < 4:
        print("usage: video_clip.py [fps] [input_dir] [output_file]")
        exit(1)
   
    fps = sys.argv[1]
    input_dir = sys.argv[2]
    output_file = sys.argv[3]

    files = os.listdir(input_dir)
    files.sort()

    fp = open(output_file, "wb")

    im_size = get_im_size(input_dir + "/" + files[0])

    # magic + frame_count + fps + width + height + reserved
    # 4b + 2b + 2b + 2b + 2b + 4b = 16 bytes
    head = pack("<IHHHHI", 0x50495856,  len(files), int(fps), im_size[0], im_size[1], 0)
    fp.write(head)

    out_dirname, out_filename = os.path.split(output_file)

    total_bytes_count = 16
    for file in files:
        byte_count = write_xbm(get_xbm(input_dir + "/" + file), fp)
        total_bytes_count = total_bytes_count + byte_count
        print("process %s .. %d bytes written." % (file, byte_count ))

    fp.close()

    print("successfully created %s with %dx%d@%dfps, %d frames, %d bytes" % (out_filename, im_size[0], im_size[1], int(fps), len(files), total_bytes_count))


######
main()


#ffmpeg -y -i cxk.mp4 -vf "scale=128:64:flags=lanczos,hue=s=0" -r 10 "out/%04d.bmp"
#ffmpeg -y -i badapple.mp4 -ss 0:0 -t 60 -vf "scale=80:64:increase:flags=lanczos,hue=s=0" -r 10 "out2/%04d.bmp"
