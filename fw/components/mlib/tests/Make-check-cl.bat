REM Copyright (c) 2017-2022, Patrick Pelissier
REM All rights reserved.
REM 
REM Redistribution and use in source and binary forms, with or without
REM modification, are permitted provided that the following conditions are met:
REM + Redistributions of source code must retain the above copyright
REM   notice, this list of conditions and the following disclaimer.
REM + Redistributions in binary form must reproduce the above copyright
REM   notice, this list of conditions and the following disclaimer in the
REM   documentation and/or other materials provided with the distribution.
REM 
REM THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
REM EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
REM WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
REM DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
REM DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
REM (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
REM LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
REM ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
REM (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
REM SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

REM Execute the test suite for Microsoft Visual Studio
REM
REM Load Micosoft Visual Studio environement file (2019 or 2022)
IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)
IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
)

IF EXIST "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)
IF EXIST "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
)

REM Add clang-cl in the PATH. It is the default install of clang for MSVC:
REM %VCINSTALLDIR%\Tools\Llvm\bin;%VCINSTALLDIR%\Tools\Llvm\x64\bin
REM Adding x64 version
REM See https://docs.microsoft.com/de-de/cpp/build/clang-support-msbuild?view=vs-2019
SET PATH=%PATH%;%VCINSTALLDIR%\Tools\Llvm\x64\bin

REM Remove previous results
DEL *.log *.dat

REM Select compiler to use (either cl or clang-cl)
set "compiler=%1"

echo "Compiler full version (%compiler%):"
%compiler% /Bv
%compiler% -v

REM Perform for each test
for %%f in (test-*.c) do (
    echo =================================================
    echo Testing %%f
    REM Copy the test file as a C++ file as CL.EXE only support C++ file
    del test.exe
    copy %%f test.cpp || EXIT /B 1
    REM Compile the test suite
    REM /Zc:preprocessor is mandatory to have a compliant preprocessor
    REM /Zc:__cplusplus is needed to report the real value of __cplusplus, so that M*LIB uses the C++ atomic, and not its emulation.
    REM Enable warnings and basic optimization
    REM Inform M*LIB to use Annex K by defining __STDC_WANT_LIB_EXT1__
    echo Compiling %%f with %compiler%
    %compiler% /I.. /O2 /W3 /std:c++14 /Zc:__cplusplus /Zc:preprocessor /D__STDC_WANT_LIB_EXT1__ test.cpp > %%f.log 2>&1  || ( 
        echo *** BUILD ERROR for %%f *** >> %%f.log
        type %%f.log 
        EXIT /B 1
        )
    REM Execute it
    echo Running %%f
    test.exe >> %%f.log 2>&1 && echo Test OK for %%f  >> %%f.log || (
        echo *** RUNTIME ERROR for %%f ***  >> %%f.log
        type %%f.log 
        EXIT /B 1
    )
    type %%f.log
)
echo "All tests passed"
exit /B 0
