@rem ����ű���������һ����д�̼� 

@echo off

echo ------------------------------------------------------------------------------
echo Pixl.js �̼��Զ���д�ű� @solosky
echo ------------------------------------------------------------------------------

set openocd_win64_download_url="https://ghproxy.com/https://github.com/openocd-org/openocd/releases/download/v0.12.0/openocd-v0.12.0-i686-w64-mingw32.tar.gz"
set openocd_win64_file="openocd-v0.12.0-i686-w64-mingw32.tar.gz"
set openocd_bin="./bin/openocd.exe"

if not exist %openocd_bin% (
    echo ����openocd.. 
    curl -Lo %openocd_win64_file% %openocd_win64_download_url%
    if not  %errorlevel%==0 (
        echo "openocd ����ʧ�ܣ������ԣ�"
        exit -1
    )

    echo ��ѹ��openocd..
    tar -xzf %openocd_win64_file%
) else (
    echo openocd�Ѿ���װ����������
)

echo ���openocd�汾
%openocd_bin% -v

echo ˢ�¹̼��У����Ժ�...
%openocd_bin%  -f interface/cmsis-dap.cfg -f target/nrf52.cfg -c init -c "reset init" -c halt -c "nrf52_recover" -c halt -c "nrf5 mass_erase" -c "program pixljs_all.hex verify"  -c reset -c exit
if %errorlevel%==0 (
    echo -----------------------------------------------------------------------------
    echo ��ϲ��ˢ�¹̼��ɹ���
    echo -----------------------------------------------------------------------------
) else (
    echo -----------------------------------------------------------------------------
    echo ˢ�¹̼�ʧ�ܣ���鿴����е������жϿ��ܵĴ���ԭ�����г����Ĵ���ԭ�����£�
    echo -----------------------------------------------------------------------------
    echo 1. ������һ����� "Error: Error connecting DP: cannot read IDR": δ�ܼ�⵽оƬ
    echo ----a. ���nrf52832оƬ�Ƿ񺸽�����
    echo ----b. ��������������Ƿ���ȷ
    echo 2. ������һ����� "Error: unable to find a matching CMSIS-DAP device": δ�ܼ�⵽������
    echo ----a. ����������Ƿ���ȷ�����ʶ��ΪCMISS-DAP�豸
    echo 3. ������һ����� "Error: error writing to flash at address 0x00000000 at offset 0x00000000": ��дʧ��
    echo ----a. ������д����
    echo ����ԭ�����QQȺ 109761876 ѯ�ʽ��
    
)

pause