@rem This script can be used to flash firmware with one click

@echo off

echo ------------------------------------------------------------------------------
echo Pixl.js Firmware automatic flash script @solosky
echo ------------------------------------------------------------------------------

set openocd_win64_download_url="https://ghproxy.com/https://github.com/openocd-org/openocd/releases/download/v0.12.0/openocd-v0.12.0-i686-w64-mingw32.tar.gz"
set openocd_win64_file="openocd-v0.12.0-i686-w64-mingw32.tar.gz"
set openocd_bin="./bin/openocd.exe"

if not exist %openocd_bin% (
    echo Download openocd…
    curl -Lo %openocd_win64_file% %openocd_win64_download_url%
    if not  %errorlevel%==0 (
        echo "openocd download failed, please try again!"
        exit -1
    )

    echo Unzipping openocd…
    tar -xzf %openocd_win64_file%
) else (
    echo openocd already installed, skip download
)

echo Checking openocd version…
%openocd_bin% -v

echo Flashing the firmware, please wait…
%openocd_bin%  -f interface/cmsis-dap.cfg -f target/nrf52.cfg -c init -c "reset init" -c halt -c "nrf52_recover" -c halt -c "nrf5 mass_erase" -c "program pixljs_all.hex verify"  -c reset -c exit
if %errorlevel%==0 (
    echo -----------------------------------------------------------------------------
    echo Firmware flash is successful!
    echo -----------------------------------------------------------------------------
) else (
    echo -----------------------------------------------------------------------------
    echo Failed to flash the firmware. Please check the output to determine the
    echo possible cause of the error. Common error causes are as follows
    echo -----------------------------------------------------------------------------
    echo 1. If the last line outputs "Error connecting DP: cannot read IDR"
    echo     The chip cannot be detected
    echo ----a. Check if the nrf52832 chip is solder correctly
    echo ----b. Check if flasher wiring is done correctly
    echo 2. If the last line outputs "Error: unable to find a matching CMSIS-DAP device"
    echo    The programmer could not be detected
    echo ----a. Check that the programmer is correctly inserted and recognized
    echo     as a CMISS-DAP device
    echo 3. If the last line outputs "Error: writing to flash at address 0x00000000
    echo    at offset 0x00000000"
    echo    Flashing has failed
    echo ----a. Just retry flashing
    echo For other reasons, please join the QQ group 109761876 to ask for solutions
    
)

pause