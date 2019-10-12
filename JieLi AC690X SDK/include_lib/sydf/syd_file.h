/*******************************************************************************************
 File Name: devcie.h

 Version: 1.00

 Discription:设备状态，操作函数定义和宏定义

 Author:dengyulin

 Email ：flowingfeeze@163.com

 Date:2010.09.09

 Copyright:(c) 2010 , All Rights Reserved.
*******************************************************************************************/
/*
version history

2010.09.09 v1.0 建立初始版本

*/

#ifndef __SYD_FILE_H_
#define __SYD_FILE_H_

#include "typedef.h"

#ifdef _SYD_CODE_LIB_
typedef enum
{
    FR_OK = 0,
    FR_FIND_DIR = 0x80,
    FR_FIND_FILE,
    FR_DIR_END,         //前面几个位置不能变
    FR_NO_FILE,
    FR_NO_PATH,
    FR_EXIST,
    FR_INVALID_NAME,
    FR_INVALID_DRIVE,
    FR_DENIED,
    FR_RW_ERROR,
    FR_WRITE_PROTECTED,
    FR_NO_FILESYSTEM,
    FR_DEEP_LIMIT,
    FR_END_PATH,
    FR_FILE_LIMIT,
    FR_END_FILE,
    FR_LFN_ERR,
    FR_MKFS_ABORTED,
    FR_DIR_DELETE,
    FR_DISK_ERROR,
    FR_FILE_END,
	FR_FILE_ERR,
    FR_NO_WINBUF,
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NO_SEL_DRIVE,
} FRESULT;
#endif

typedef struct _SYDWIN_BUF
{
    u8  start[512];
	u32  sector;
    struct _SYDFS  *fs;
    u8   flag;
}SYDWIN_BUF;


struct _SYDFS
{
    u32		fsbase;		/* FAT start sector */
    u32     file_total;
    u32     brk_file;
    u32 	(*disk_read)(void *hdev,u8 _xdata * buf, u32 lba);		/* device read function */
    u32		(*disk_write)(void *hdev,u8 _xdata * buf, u32 lba);		/* device write function */
    void 	*hdev;
    SYDWIN_BUF 	win;		        /* Disk access window for Directory/FAT/File */
    u8 			drive_cnt;
};
typedef struct _SYDFS SYDFS;


typedef struct _SDFILE
{
    u32 addr;
    u32 length;
    u32 pointer;
    u32 index;
    u16 crc;
    SYDFS *f_s;
} SDFILE , *PSDFILE ,sdfile_t ,*psdfile_t;

typedef struct _FLASHHEADER
{
    u16 crc16;
    u16 crcfileheaddata;
    u8 info[8];
    u32 filenum;
    u32 version;
    u32 version1;
    u8 chiptype[8];

} FLASHHEADER;


typedef struct _FILEHEAD
{
    u8 filetype;
    u8 reserv;
    u16 crc16;
    u32 addr;
    u32 len;
    u32 index;
    u8 name[16];

} FLASHFILEHEAD , *PFLASHPFILEHEAD ;


typedef struct _BANKCB
{
    u16 bankNum;
    u16 size;
    u32 bankaddr;
    u32 addr;
    u16 crc;
    u16 crc1;
} BANKCB;

typedef struct _SDF_IO
{
    void *param;
    u32 addOffset;
    u32 devOnline;
    tbool(*disk_read)(void *param, void *, u32, u32);
} SDF_IO;

#define  SDDEVICE_NOR_FLASH     0x00
#define  SDDEVICE_SDCARD        0x01
#define  SDDEVICE_NAND_FLASH    0x02

void set_sydf_header_base(u32 base);
u32 check_syd (SYDFS *syd_fs, u32 sec);
void set_sdf_head_addr(u32 addr);
tbool sydf_openbyname(SYDFS *f_s,const u8 *filename ,SDFILE *file);
tbool sydf_openbyindex(SYDFS *f_s, SDFILE *file,u16 index,char *lfn_buf);
tbool syd_api_init(SYDFS *syd_fs,void *p_hdev);
//u32 syd_get_file_total(SYDFS *syd_fs,const char *file_type);
bool syd_get_file_total(SYDFS *syd_fs,u32 brk_info);
u32 get_free_flash_addr(SYDFS *syd_fs);
u16 sydf_read(SDFILE  *fp ,u8  *buff , u16 length);
u32 sydf_seek(SDFILE *fp,u8 type,u32 offsize );



#endif

