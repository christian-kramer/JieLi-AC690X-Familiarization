/*********************************************************************************************
    *   Filename        : icache_interface.h

    *   Description     : 

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-18 10:31

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _ICACHE_INTERFACE_H
#define _ICACHE_INTERFACE_H

#include "typedef.h"

#define CACHE_LINE_COUNT   32 
#define CACHE_ADDR_ALIGN(addr)	 ((void *)(((unsigned int)(addr) + CACHE_LINE_COUNT - 1) & ~(unsigned int)(CACHE_LINE_COUNT-1)))

struct icache_driver {
    void (*resume)();
    void (*suspend)();

    void (*flush)();

    void (*ioctrl)(int, ...);
};

// #define REGISTER_ICACHE_DRIVER(drv) \
	// const struct icache_driver *__icache_drv \
			// __attribute__((section(".icache_drv"))) = &drv
#define REGISTER_ICACHE_DRIVER(drv) \
	const struct icache_driver *__icache_drv \
			sec(.icache_drv) = &drv

extern const struct icache_driver *__icache_drv;

#endif 
