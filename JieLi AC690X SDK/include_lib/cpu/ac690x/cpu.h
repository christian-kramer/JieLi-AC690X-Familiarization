/*********************************************************************************************
    *   Filename        : cpu.h

    *   Description     : 

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 10:56

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _CPU_H_
#define _CPU_H_



#ifndef __ASSEMBLY__
#include "asm_type.h"
#include "hw_cpu.h"
#endif
/*
*********************************************************************************************************
*                                              Defines
*********************************************************************************************************
*/
#define     CLR_WDT()	(JL_SYSTEM->WDT_CON |= BIT(6))
#define     trig        __asm__ volatile ("trigger")
#define     LITTLE_ENDIAN
#define     OS_CPU_CORE     1
#define     OS_CPU_ID       (OS_CPU_CORE - 1)
#define     OS_STK_GROWTH        1             /* Stack grows from HIGH to LOW memory on 80x86  */

#define OS_CPU_MMU                1
#define SDRAM                     0
#define MEM_MALLOC                1

#if SDRAM > 0
#define PAGE_SHIFT 14
#define SDRAM_BEGIN_ADDR        (0x800000)
#define MAX_SDRAM_SIZE          (4*1024*1024)
#define MMU_MAX_SIZE            (4*1024*1024)  // mmu 管理的最大地址空间
#define MMU_ADDR_BEGIN          (0x400000)
#define MMU_ADDR_END            (0x800000)
#define STACK_TASK_SIZE         (1024*64)
#define STACK_START             MMU_ADDR_BEGIN
#define STACK_END               (MMU_ADDR_BEGIN+STACK_TASK_SIZE)
#else
#define PAGE_SHIFT 9
//#define SDRAM_BEGIN_ADDR        (0x0)
#define MAX_SDRAM_SIZE          (104*1024)
#define MMU_MAX_SIZE            (104*1024)  // mmu 管理的最大地址空间
#define MMU_ADDR_BEGIN          (0x80000)
#define MMU_ADDR_END            (MMU_ADDR_BEGIN+MMU_MAX_SIZE)
#define STACK_TASK_SIZE         (1024*1)
#define STACK_START             MMU_ADDR_BEGIN
#define STACK_TASK_MAX_SIZE     (1024*4)
#endif

#define PAGE_SIZE   (1UL << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))
#define PAGE_ALIGN(addr)    (((addr)+PAGE_SIZE-1)&PAGE_MASK)

#define MMU_TAG_ADDR0   0x49800
#define APP_BEGIN_ADDR  MMU_ADDR_BEGIN

#define MMU_TAG_SIZE        0x100
#define PM_BITMAP_UNIT		(32*PAGE_SIZE)

#if OS_CPU_CORE > 1
#define MMU_TAG_ADDR1   0x6a000
#endif

#if (MAX_SDRAM_SIZE/PAGE_SIZE  > 256 )
#define ufat_t u16
#else
#define ufat_t u8
#endif


#define  CPU_SR_ALLOC()

#define  OS_TASK_CLR(a)
#define  OS_TASK_SW(a)     __asm__ volatile ("swi 62")      /* 任务级任务切换函数*/
#ifndef __ASSEMBLY__
extern void CPU_INT_DIS();
extern void CPU_INT_EN();
#endif
#define OS_ENTER_CRITICAL()  CPU_INT_DIS();           /* 关中断*/
#define OS_EXIT_CRITICAL()   CPU_INT_EN();          /* 开中断*/

#define MULU(Rm,Rn) __builtin_pi32_umul64(Rm,Rn);
#define MUL(Rm,Rn)  __builtin_pi32_smul64(Rm,Rn);
#define MLA(Rm,Rn)  __builtin_pi32_smla64(Rm,Rn);
#define MLA0(Rm,Rn) MUL(Rm,Rn);
#define MLS(Rm,Rn)  __builtin_pi32_smls64(Rm,Rn);
#define MLS0(Rm,Rn) MUL(-Rm,Rn);
#define MRSIS(Rm,Rn) (Rm = __builtin_pi32_sreadmacc(Rn));//mac read right shift imm&sat
#define MRSRS(Rm,Rn) (Rm = __builtin_pi32_sreadmacc(Rn)); //mac read right shift reg&sat
#define MACCLR()  __asm__ volatile ("clrmacc");
#define MACSET(h,l) __asm__ volatile ("mov maccl, %0; mov macch, %1"::"r" (l), "r" (h));
#define MACRL(l) __asm__ volatile ("mov %0, maccl":"=r" (l));
#define MACRH(l) __asm__ volatile ("mov %0, macch":"=r" (l));
#define MULSIS(Ro,Rm,Rn,Rp) MUL(Rm, Rn); MRSIS(Ro, Rp);
#define MULSRS(Ro,Rm,Rn,Rp) MUL(Rm, Rn); MRSRS(Ro, Rp);

#if 0 
//debug
#define ASSERT(a,...)   \
        do { \
            if(!(a)){ \
                printf("%s %d", __FILE__, __LINE__); \
                printf("ASSERT-FAILD: "#a"\n"__VA_ARGS__); \
                while(1); \
            } \
        }while(0);
#else
/*assert error reset*/
#define ASSERT(a,...)   \
        do { \
            if(!(a)){ \
     			JL_POWER->CON |= BIT(4);\
            } \
        }while(0);
#endif
#endif

