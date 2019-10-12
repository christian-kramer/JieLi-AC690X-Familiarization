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

#ifndef _RTC_H_
#define _RTC_H_

#include "sdk_cfg.h"
#include "includes.h"
#include "common/app_cfg.h"
#include "rtos/os_api.h"
#include "rtos/os_cfg.h"
#include "common/error.h"
#include "common/msg.h"
#include "rtos/task_manage.h"
#include "rtc_key.h"
#include "rtc_setting.h"
#include "dac/dac_api.h"
#include "play_sel/play_sel.h"
#include "key_drv/key.h"
#include "key_drv/key_voice.h"
//#include "led.h"
#include "ui/ui_api.h"
#include "rtc_ui.h"

#define _RTC_DEBUG_

#ifdef _RTC_DEBUG_
#define rtc_puts         puts
#define rtc_printf       printf
#else
#define rtc_puts(...)
#define rtc_printf(...)
#endif

#define RTC_TASK_NAME   "RTCTask"

extern RTC_TIME current_time;
extern RTC_TIME current_alarm;

extern void irtc_hw_init(void);

extern const struct task_info rtc_task_info;
extern void *malloc_fun(void *ptr,u32 len,char *info);
extern void free_fun(void **ptr);


#endif//#ifndef _RTC_H_

