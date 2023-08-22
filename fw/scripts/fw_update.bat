@rem 这个脚本可以用来一键烧写固件 

@echo off

echo ------------------------------------------------------------------------------
echo Pixl.js 固件自动烧写脚本 @solosky
echo ------------------------------------------------------------------------------

set openocd_win64_download_url="https://ghproxy.com/https://github.com/openocd-org/openocd/releases/download/v0.12.0/openocd-v0.12.0-i686-w64-mingw32.tar.gz"
set openocd_win64_file="openocd-v0.12.0-i686-w64-mingw32.tar.gz"
set wget_win64_download_url="https://eternallybored.org/misc/wget/1.21.4/64/wget.exe"
set openocd_bin="./bin/openocd.exe"

if not exist %openocd_bin% (
    echo 下载wget...
    curl -o wget.exe %wget_win64_download_url%
    if not  %errorlevel%==0 (
        echo "wget 下载失败，请重试！"
        exit -1
    )

    echo 下载openocd.. 
    wget.exe -O %openocd_win64_file% %openocd_win64_download_url%
    if not  %errorlevel%==0 (
        echo "openocd 下载失败，请重试！"
        exit -1
    )

    echo 解压缩openocd..
    tar -xzf %openocd_win64_file%
) else (
    echo openocd已经安装，跳过下载
)

echo 检查openocd版本
%openocd_bin% -v

echo 刷新固件中，请稍后...
%openocd_bin%  -f interface/cmsis-dap.cfg -f target/nrf52.cfg -c init -c "reset init" -c halt -c "nrf52_recover" -c halt -c "nrf51 mass_erase" -c "program pixljs_all.hex verify"  -c reset -c exit
if %errorlevel%==0 (
    echo -----------------------------------------------------------------------------
    echo 恭喜，刷新固件成功！
    echo -----------------------------------------------------------------------------
) else (
    echo -----------------------------------------------------------------------------
    echo 刷新固件失败，请查看输出中的内容判断可能的错误原因，其中常见的错误原因如下：
    echo -----------------------------------------------------------------------------
    echo 1. 如果最后一行输出 "Error: Error connecting DP: cannot read IDR": 未能检测到芯片
    echo ----a. 检查nrf52832芯片是否焊接正常
    echo ----b. 检查下载器接线是否正确
    echo 2. 如果最后一行输出 "Error: unable to find a matching CMSIS-DAP device": 未能检测到下载器
    echo ----a. 检查下载器是否正确插入和识别为CMISS-DAP设备
    echo 3. 如果最后一行输出 "Error: error writing to flash at address 0x00000000 at offset 0x00000000": 烧写失败
    echo ----a. 重试烧写即可
    echo 其他原因请加QQ群 109761876 询问解决
    
)

pause