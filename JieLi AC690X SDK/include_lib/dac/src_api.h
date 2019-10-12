/*
*********************************************************************************************************
*                                             BC51
*
*                                             CODE
*
*                          (c) Copyright 2015-2016, ZHUHAI JIELI
*                                           All Rights Reserved
*
* File : *
* By   : jamin.li
* DATE : 11/11/2015 build this file
*********************************************************************************************************
*/
#ifndef _SRC_H_
#define _SRC_H_

#include "rtos/os_api.h"
#include "hw_cpu.h"
#include "cpu.h"
#include "cbuf/circular_buf.h"
#include "common/printf.h"
#include "mem/malloc.h"

#define SRC_MAX_CHANNEL 2

///DAC_BUF_LEN
#define SRC_INPUT_BUF_LEN  (128)
#define SRC_OUTPUT_BUF_LEN (128)

#define SRC_CBUFF_SIZE         (SRC_INPUT_BUF_LEN*12)   ///1.5K,输入数据缓冲池大小
#define SRC_START_CHANGE_LEN   (SRC_INPUT_BUF_LEN*4)    ///512,启动转换的缓冲池数据长度
#define SRC_FLTB_CACHE_SIZE    (SRC_MAX_CHANNEL*48)     ///硬件缓存,每个通道缓冲需要48bytes,4字节对齐

#define SRC_PMALLOC_SIZE       (SRC_INPUT_BUF_LEN + SRC_OUTPUT_BUF_LEN + SRC_FLTB_CACHE_SIZE)///物理ram申请空间

typedef struct
{
    u8 nchannel;    ///一次转换的通道个数，(1 ~ SRC_MAX_CHANNEL)
    u8 reserver[3];
    u16 in_rate;    ///输入采样率
    u16 out_rate;   ///输出采样率
    u16 in_chinc;   ///输入方向,多通道转换时，每通道数据的地址增量
    u16 in_spinc;   ///输入方向,同一通道后一数据相对前一数据的地址增量
    u16 out_chinc;  ///输出方向,多通道转换时，每通道数据的地址增量
    u16 out_spinc;  ///输出方向,同一通道后一数据相对前一数据的地址增量
    void (*output_cbk)(u8* ,u16 ); ///转换后数据输出回调，中断调用
} src_param_t;

typedef struct
{
    u8 src_read_empty;   ///输入CBUFF读空标记
    u8 src_channel_cnt;  ///转换的通道个数
    u8 reserver[2];
    void (*src_output_cbk)(u8* ,u16 );///转换后数据输出回调，中断调用
    cbuffer_t *src_cb ;   ///CBUFF控制
    u8  *src_c_buf_addr;  ///数据写入CBUFF
    u16 *src_input_buff;  ///转换输入物理buffer
    u16 *src_output_buff; ///转换输出物理buffer
    u32 *src_fltb_cache;  ///每个通道缓冲需要48bytes
}src_var_t;



struct src_driver {

    int (*src_init)(src_param_t *parm);
    void (*src_exit)(void);
    u32 (*src_write_data)(u8 *buf,u16 len);
    void (*src_clear_data)(void);
};


// #define REGISTER_SRC_DRIVER(drv) \
	// const struct src_driver *__src_drv \
			// __attribute__((section(".text"))) = &drv
#define REGISTER_SRC_DRIVER(drv) \
	const struct src_driver *__src_drv \
			sec(.text) = &drv

extern const struct src_driver *__src_drv;
int src_init_api(src_param_t *parm);
u32 src_write_data_api(u8 *buf , u16 len);
void src_clear_data_api(void);
void src_exit_api(void);
#endif
