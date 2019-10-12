/******************************************************************************************* 
 *
 *   File Name: .h 
 *
 *   Version: 1.00 
 *
 *   Discription: 
 *
 *   Author:Bingquan Cai 
 *
 *   Email :bingquan_cai@zh-jieli.com 
 *
 *   Date:
 *
 *   Copyright:(c)JIELI  2016  @ , All Rights Reserved.
 *
 *   *******************************************************************************************/

#ifndef __CRC_API_H__
#define __CRC_API_H__

#include "typedef.h"


struct crc_driver {
    u16 (*crc16)(const void *, u32);

    u32 (*crc32)(const void *, u32);
};

// #define REGISTER_CRC_DRIVER(drv) \
	// const struct crc_driver *__crc_drv \
			// __attribute__((section(".text"))) = &drv
#define REGISTER_CRC_DRIVER(drv) \
	const struct crc_driver *__crc_drv \
			sec(.text) = &drv

extern const struct crc_driver *__crc_drv;

/********************************************************************************/
/*
 *      API
 */
u16 crc16(const void *ptr, u32 len);


#define CRC16(ptr, len) \
    crc16(ptr, len)

#endif
