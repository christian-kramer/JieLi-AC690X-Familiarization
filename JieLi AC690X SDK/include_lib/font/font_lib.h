/*
*********************************************************************************************************
*                                                BC51
*                                      Remote Control System Protocol (RCSP)
*                                             CODE
*
*                          (c) Copyright 2015-2016, ZHUHAI JIELI
*                                           All Rights Reserved
*
* File : *
* By   : jamin.li
* DATE : 8/20/2015 build this file
*********************************************************************************************************
*/

#ifndef __FONT_LIB_H__
#define __FONT_LIB_H__

///#define FONT_DEBUG_PRINT  ///

#ifdef FONT_DEBUG_PRINT

#define font_printf        printf
#define font_printf_buf    printf_buf
#define font_puts          puts

#else

#define font_printf(...)
#define font_printf_buf(...)
#define font_puts(...)

#endif

#endif /* #ifndef __FONT_LIB_H__ */
