
cd  %~dp0

@echo off

if "%1" == "pi32" (
    echo "download pi32" %1 %2
    set OBJDUMP=C:\JL\pi32\bin\llvm-objdump
    set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy
    )

if "%1" =="pi32_lto" (
    echo "download pi32_lto" %1  %2
    set OBJDUMP=C:\JL\pi32\bin\llvm-objdump
    set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy
    )

::cd tools\%2
::call download.bat %2 %OBJDUMP% %OBJCOPY% 

call download.bat %2 %OBJDUMP% %OBJCOPY% 
