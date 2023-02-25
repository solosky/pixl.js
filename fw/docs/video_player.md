# 动画播放器

动画播放器可以播放转换后的自定义格式的动画文件。需要固件版本为2.3.0或更高版本。


## 动画文件格式 

文件的基本格式为 header + frame1 + frame2 +... + frameN

    # magic + frame_count + fps + width + height + reserved
    # 4b + 2b + 2b + 2b + 2b + 4b = 16 bytes

其中Header格式如下 

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| magic  | bytes | 4  |  固定的前缀，用于快速判断是否是动画文件，固定为 0x56 0x58 0x49 0x50 |
| frame count | uint16 | 2 | 总共有多少帧动画 |
| fps | uint16 | 2 | 每秒有多少帧动画 | 
| width   | uint16 | 2 | 动画宽度 |
| height   | uint16 | 2 | 动画高度 |
| reserved   | uint32 | 4 | 保留字节 |

其中frame N的数据格式为xbm格式的动画每帧数据，大小为 width * height / 8。
其中如果width的长度不是8的倍数，向上去整。

## 如何生成动画文件


首先准备好需要转换的视频文件，通常是mp4结尾，其他格式也可以。

以下操作需要在linux下面运行，否则会报错。
如果没有linux环境，可以使用WSL2来快速创建一个ubuntu的linux环境。

首先执行如下命令安装必须的工具 

```
sudo apt install ffmpeg python3-pip
sudo pip3 install imageio -i https://pypi.tuna.tsinghua.edu.cn/simple
```

然后执行ffmpeg命令，转换动画为独立的帧图片。
```
ffmpeg -y -i badapple.mp4 -ss 0:0 -t 60 -vf "scale=128:64:decrease:flags=lanczos,hue=s=0" -r 10 "frames/%04d.bmp"
```
参数说明如下：

* -i badapple.mp4 : 这是输入视频文件
* -ss 0:0 : 从视频的0:0开始截取，如果不截取部分视频，可以不提供这个参数
* -t 60 : 截取60秒视频，如果转换整个视频，可以不提供这个参数 
* -vf "scale=128:64:decrease:flags=lanczos,hue=s=0" : 转换视频为128x64的图片，保留视频的宽高比
* -r 10 : 转换后的fps为10
* "frames/%04d.bmp" : 转换的视频文件存放在 frames目录下，格式为bmp

如果视频较大，可以通过如下方式减小视频大小：

* 给定 -ss -t 参数截取部分视频
* 减少输出的分辨率，比如80x64等

最后执行如下命令转换为 自定义的动画文件 

```
python3 fw/scripts/video_clip_gen.py 10 frames badapple.bin 
```
参数说明如下：

* 10 : 转换后的fps为10，比如和ffmpeg转换的fps一样，不然播放速度不正确
* frames : ffmpeg转换的图片目录
* badapple.bin : 转换后的动画文件

## 播放动画文件

把转换后的视频文件通过网页传输到Pixl.js的Flash存储的 player 目录下，player目录没有可以手动创建。

打开Pixl.js，选择【动画播放器】，列出player目录下的所有文件，然后选择要播放的视频文件，按中键播放。

注意：

* 虽然Flash存储是1.8MB的空间，但是由于SPIFFS文件系统的特性，当可用的空间低于30%的时候写入就会很慢，甚至写入失败。 <br />
  如果遇到写入很慢的情况，可以通过格式化磁盘来恢复上传速度。
* 当删除大文件的时候比较慢，请耐心等待。

