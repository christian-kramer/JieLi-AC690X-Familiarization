cd  %~dp0

set OR32_NAME=ac690x
set OBJDUMP=C:\JL\pi32\bin\llvm-objdump
   
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy

cd ../post_build

call download.bat %OR32_NAME% %OBJDUMP% %OBJCOPY% 



pause
