/*******************************************************************************************
  File Name: vmalloc.h


Version: 1.00

Discription:


Author:yulin deng

Email :flowingfeeze@163.com

Date:2014-04-22   08:05:06ÏÂÎç

Copyright:(c)JIELI  2014  @ , All Rights Reserved.
 *******************************************************************************************/
#ifndef __malloc_h
#define __malloc_h

#include "typedef.h"
#include "cpu.h"
//#include "mmu_tag.h"
//#include "pmalloc.h"

#define ufat_t   u8

u32 vmm_init() ;
ufat_t * vmalloc(u32 size) ;
u32 vrealloc(ufat_t *fat, u32 size) ;
void vfree(ufat_t *fat) ;
u32 get_phy_addr(u32 vaddr) ;
u32 get_phy_addr_by_mmu(ufat_t *fat, u32 vddr_start, u32 vaddr) ;
void vfill_tag(ufat_t *fat ,u32 vaddr,u8 mod);
extern void *sbrk(void *addr, int incr);
extern void *malloc(unsigned int size);
extern void *calloc(unsigned int count, unsigned int size);
extern void *realloc(void *rmem, unsigned int newsize);
extern void  free(void *mem);
extern void mem_heap_stats(void);
extern void malloc_stats(void);
extern void get_malloc_stats(void * arg);
extern void * pmalloc(u32 nsize, u32 mode);

#endif
