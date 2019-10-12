#ifndef _FS_IO_H_H_
#define _FS_IO_H_H_

#include "typedef.h"
#define NORM_BRK_LEN  8
#define FLAC_BRK_LEN  564

typedef enum __DEV_FS_TYPE
{
    FAT_FS_T =1,
    SYD_FS_T,
}DEV_FS_TYPE;

typedef struct _FS_BRK_INFO
{
    u32 bk_sclust;
    u32 f_size;
    u8 lg_dev_num;
    u8 phy_dev_num;
    // u16 crc;
    u8 norm_brk_buff[NORM_BRK_LEN];
	u8 flac_brk_buff[FLAC_BRK_LEN];
}FS_BRK_INFO;


typedef struct _FS_BRK_POINT
{
    FS_BRK_INFO inf;
    u32 file_num;
    tbool result;
}FS_BRK_POINT;


#endif
