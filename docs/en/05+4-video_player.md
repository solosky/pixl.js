# animation player

The animation player can play converted animation files in custom formats. Requires firmware version 2.3.0 or higher.


## Animation file format

The basic format of the file is header + frame1 + frame2 +... + frameN

     # magic + frame_count + fps + width + height + reserved
     # 4b + 2b + 2b + 2b + 2b + 4b = 16 bytes

The header format is as follows

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| magic | bytes | 4 | Fixed prefix, used to quickly determine whether it is an animation file, fixed to 0x56 0x58 0x49 0x50 |
| frame count | uint16 | 2 | How many frames of animation are there in total |
| fps | uint16 | 2 | How many frames of animation per second |
| width | uint16 | 2 | animation width |
| height | uint16 | 2 | animation height |
| reserved | uint32 | 4 | reserved bytes |

The data format of frame N is the animation data of each frame in xbm format, and the size is width * height / 8.
If the length of width is not a multiple of 8, round it up.

## How to generate animation files


First prepare the video file that needs to be converted, usually ending in mp4, but other formats are also acceptable.

The following operations need to be run under Linux.
Also you can use WSL2 to quickly create an Ubuntu Linux environment under Windows.

First execute the following command to install the necessary tools

```
sudo apt install ffmpeg python3-pip
sudo pip3 install imageio -i https://pypi.tuna.tsinghua.edu.cn/simple
```

For Ubuntu you will need to install packages running the next command:

```
sudo apt install ffmpeg python3 python3-imageio
```

If you preffer the MSYS MINGW64 environment you can install the packages using this commands:

```
pacman -S mingw-w64-x86_64-ffmpeg mingw-w64-x86_64-python mingw-w64-x86_64-python-imageio
```


Then execute the ffmpeg command to convert the animation into independent frame pictures.
```
mkdir frames
ffmpeg -y -i badapple.mp4 -ss 0:0 -t 60 -vf "scale=128:64:decrease:flags=lanczos,hue=s=0" -r 10 "frames/%04d.bmp"
```
Parameter description is as follows:

* -i badapple.mp4: This is the input video file
* -ss 0:0: intercept from 0:0 of the video. If you do not want to intercept part of the video, you do not need to provide this parameter.
* -t 60: intercept 60 seconds of video. If you convert the entire video, you do not need to provide this parameter.
* -vf "scale=128:64:decrease:flags=lanczos,hue=s=0" : Convert the video to a 128x64 image, retaining the aspect ratio of the video
* -r 10: The converted fps is 10
* "frames/%04d.bmp": The converted video files are stored in the frames directory and the format is bmp

If the video is larger, you can reduce the video size as follows:

* Given the -ss -t parameters, intercept part of the video
* Reduce the output resolution, such as 80x64, etc.

Finally, execute the following command to convert it to a customized animation file

```
python3 fw/scripts/video_clip_gen.py 10 frames badapple.bin
```
Parameter description is as follows:

* 10: The converted fps is 10, for example, it is the same as the fps converted by ffmpeg, otherwise the playback speed will be incorrect.
* frames: directory of pictures converted by ffmpeg
* badapple.bin: converted animation file

## Play animation files

Transfer the converted video files to the player directory of Flash storage in Pixl.js through the web page. The player directory cannot be created manually.

Open Pixl.js, select [Animation Player], list all the files in the player directory, then select the video file you want to play, and press the middle button to play.

Notice:

* Although the Flash storage is 1.8MB of space, due to the characteristics of the SPIFFS file system, when the available space is less than 30%, writing will be very slow or even fail.
* If you encounter slow writing, you can restore upload speed by formatting the disk.
* Deleting large files it is slow, please be patient.