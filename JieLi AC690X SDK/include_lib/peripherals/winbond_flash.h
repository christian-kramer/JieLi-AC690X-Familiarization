#ifndef	_WINBOND_FLASH_H_
#define _WINBOND_FLASH_H_

#include "typedef.h"

#define FLASH_TIMEOUT   1000000

#define FLASH_PAGE_SIZE 256

#define DUMMY_BYTE  0xff

enum{
    SET_READ_CMD_MODE = 0,
};

enum 
{
    //1bit mode
    WINBOND_READ_DATA_MODE = (1<<0),
    WINBOND_FAST_READ_MODE = (1<<1),
    //2bit mode
    WINBOND_FAST_READ_DUAL_IO_NORMAL_READ_MODE  = (1<<2),
    WINBOND_FAST_READ_DUAL_IO_CONTINUOUS_READ_MODE = (1<<3),
    WINBOND_FAST_READ_DUAL_OUTPUT_MODE     = (1<<4),
    //4bit mode
    WINBOND_FAST_READ_QUAD_IO_NORMAL_READ_MODE   = (1<<5),
    WINBOND_FAST_READ_QUAD_IO_CONTINUOUS_READ_MODE = (1<<6),
    WINBOND_FAST_READ_QUAD_OUTPUT_MODE     = (1<<7),

};

typedef enum _FLASH_ERASER
{
	CHIP_ERASER,
	BLOCK_ERASER,
	SECTOR_ERASER,
}FLASH_ERASER;
/*************************************************/
/*
		COMMAND LIST - WinBond FLASH WX25X
*/
/***************************************************************/
#define WINBOND_WRITE_ENABLE		                        0x06
#define WINBOND_WRITE_DISABLE	                            0x04
#define WINBOND_HIGH_PERFORMANCE_MODE			            0xA3
#define WINBOND_RELEASE_POWER_DOWN_OR_HIGH_PERFORMANCE_MODE 0xAB
#define WINBOND_READ_SR1			                        0x05
#define WINBOND_READ_SR2			                        0x35
#define WINBOND_WRITE_SR			                        0x01
#define WINBOND_READ_DATA		                            0x03
#define WINBOND_FAST_READ			                        0x0B
#define WINBOND_FAST_READ_DUAL_OUTPUT	                    0x3B
#define WINBOND_FAST_READ_DUAL_IO                           0xBB
#define WINBOND_FAST_READ_QUAD_OUTPUT                       0x6B
#define WINBOND_FAST_READ_QUAD_IO                           0xEB
#define WINBOND_WORD_READ_QUAD_IO                           0xE7
#define WINBOND_OCTAL_WORD_READ_QUAD_IO                     0xE3
#define WINBOND_CONTINUOUS_READ_ENHANCE_MODE                0xa0
#define WINBOND_CONTINUOUS_READ_NORMAL_MODE                 0x00
#define WINBOND_PAGE_PROGRAM	                            0x02
#define WINBOND_PAGE_PROGRAM_QUAD	                        0x32
#define WINBOND_SECTOR_ERASE		                        0x20
#define WINBOND_BLOCK_ERASE		                            0xD8
#define WINBOND_CHIP_ERASE		                            0xC7
#define WINBOND_POWER_DOWN		                            0xB9
#define WINBOND_RELEASE			                            0xAB
#define WINBOND_MANUFACTURE		                            0x90
#define WINBOND_JEDEC_ID			                        0x9F
#define WINBOND_ERASE_SUSPEND		                        0x75
#define WINBOND_ERASE_RESUME								0x7A


void spiflash_wait_ok(void);

void spiflash_send_write_enable(void);

void spiflash_continuous_read_mode_reset(void);

void spiflash_send_addr(u32 addr);

void spiflash_recover_continuous_read_mode(void);

s32 spiflash_write(u8 *buf, u32 addr, u32 len);

void spiflash_eraser(FLASH_ERASER eraser, u32 address, u8 wait_ok);

void spiflash_eraser_suspend(void);

void spiflash_eraser_resume(void);


s32 spiflash_read(u8 *buf, u32 addr ,u16 len);

void flash_mode_init(void);

void flash_ioctrl(int ctrl, ...);
#endif
