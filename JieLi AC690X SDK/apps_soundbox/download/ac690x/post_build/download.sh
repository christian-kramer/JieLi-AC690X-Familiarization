#!bin/bash

if [ -f $1.bin ]; then rm $1.bin; fi
if [ -f $1.lst ]; then rm $1.lst; fi

${OBJDUMP} -disassemble $1.or32 > $1.lst
${OBJCOPY} -O binary -j .text  $1.or32  $1.bin
${OBJCOPY} -O binary -j .data  $1.or32  data.bin
${OBJCOPY} -O binary -j .data1 $1.or32  data1.bin
${OBJDUMP} -section-headers  $1.or32
${OBJDUMP} -t $1.or32 > $1.symbol.txt

#cat uboot.bin ver.bin> uboot.boot 

cat $1.bin data.bin data1.bin > sdram.app 

rm -f $1.bin data.bin data1.bin

# rm $1.or32

host-client -project ${APP_TYPE} -f uboot.bin ver.bin sdram.app fast_run.bin br17loader.bin

# host-client -project ${SDK_TYPE}  -directory ../download  ../post_build 
