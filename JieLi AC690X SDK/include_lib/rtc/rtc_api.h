#ifndef __RTC_API_H__
#define __RTC_API_H__

#include "typedef.h"

extern int printf(const char *format, ...);

// #define _RTC_LIB_DEBUG_

#ifdef _RTC_LIB_DEBUG_
#define rtc_lib_printf       printf
#else
#define rtc_lib_printf(...)
#endif


///enable or disable alarm or IO wake up
#define WAKE_UP_ENABLE         1
#define WAKE_UP_DISENABLE        0

///RTC PORT BIT(PORTR)
#define PORTR0              0
#define PORTR1              1
#define PORTR2              2
#define PORTR3              3

#define PORT_VAL_HIGH        1
#define PORT_VAL_LOW         0

#define INTERRUPT_TYPE_ALARM_COME           1    ///闹钟时间到
#define INTERRUPT_TYPE_IO_WAKE_UP           2    ///PR IO唤醒
#define INTERRUPT_TYPE_X512_HZ              3    ///512Hz事件发生时
#define INTERRUPT_TYPE_X2_Z                 4    ///2Hz事件发生时
#define INTERRUPT_TYPE_PCNT_OVF             5    ///PCNT_VALUE overflow事件发生时
#define INTERRUPT_TYPE_LDO5V_DET            6    ///LDO5V edge detect事件发生时

#define WKUP_TYOE_NO_WAKE_UP                0
#define WKUP_TYPE_PCNT_OVF					1
#define WKUP_TYPE_EDGE						2
#define WKUP_TYPE_ALMOUT                    3
#define WKUP_TYPE_LVD5V                     4

/////////////////RTC LOWPOWER//////////////////////
#define    WAKE_UP_PR0  BIT(0) 
#define    WAKE_UP_PR1  BIT(1)
#define    WAKE_UP_PR2  BIT(2) 
#define    WAKE_UP_PR3  BIT(3)

#define    EDGE_PR0     BIT(4) 
#define    EDGE_PR1     BIT(5) 
#define    EDGE_PR2     BIT(6) 
#define    EDGE_PR3     BIT(7) 

#define    LOW_RESET                       0
#define    HIGH_RESET                        1

#define    ENABLE              1
#define    DISABLE             0

enum
{
    RTC_MODULE_NO_ERROR                             = 0,
    RTC_MODULE_PARAM_ERROR                      = -1000,
    RTC_MODULE_NO_SUPPORT_ERROR             = -1001,
    RTC_MODULE_UNKNOW_ERROR             = -1100,
};

enum
{
    RTC_YEAR_SETTING = 0x00,
    RTC_MONTH_SETTING,
    RTC_DAT_SETTING,
    RTC_HOUR_SETTING,
    RTC_MIN_SETTING,
    RTC_SEC_SETTING,
};

//以下2个宏设置时钟设置的范围，可根据显示屏的显示范围设置
#define COORDINATE_MIN  RTC_YEAR_SETTING
#define COORDINATE_MAX  RTC_MIN_SETTING

///can't modify
typedef struct _RTC_TIME
{
    u16 dYear;		///<年份
    u8 	bMonth;		///<月份
    u8 	bDay;		///<天数
    u8 	bHour;		///<时
    u8 	bMin;		///<分
    u8 	bSec;		///<秒
//    u8 	bWeekday;	///<星期几
} RTC_TIME;

void rtc_module_on(bool wake_up_falg, bool rtc_clk_check, void (*handle)(u8 flag));
int rtc_module_alarm_switch(u8 value);
void rtc_module_write_rtc_datetime(  RTC_TIME *curr_time );
void rtc_module_read_rtc_datetime(  RTC_TIME *curr_time );
void rtc_module_write_alarm_datetime(  RTC_TIME *curr_time , u8 alarm_mode);
void rtc_module_read_alarm_datetime(  RTC_TIME *curr_time );
bool rtc_module_get_power_up_flag();
bool rtc_module_get_alarm_flag();
void rtc_module_reset_irtc();
bool rtc_module_get_alm_come_flag();
void soft_poweroff_wakeup_io(u8 wakeup_io,u8 wakeup_edge);

extern void rtc_calculate_next_day(RTC_TIME *data_time);
extern void rtc_calculate_next_weekday(RTC_TIME *data_time);
extern void calendar_time_plus(RTC_TIME *curr_rtc_time,u8 coordinate);
extern void calendar_time_minus(RTC_TIME *curr_rtc_time,u8 coordinate);

void PORTR_DIR(u8 port, bool val);
void PORTR_OUT(u8 port, u8 val);
void PORTR_HD(u8 port, u8 val);
void PORTR_PU(u8 port, u8 val);
void PORTR_PD(u8 port, u8 val);
void PORTR_DIE(u8 port, u8 val);
u8   PORTR_IN(u8 port);     ///读取PORTR的IO口状态，必须设置PORTR_DIR(port, 0); 和  PORTR_HD(port, 1);

/**************************************************************************

   @brief  XCKS 2bits    ,    RTC CLOCK SELECT
   @param 	u8 value
                 0/1   : use X32K1P or X32K2P oscillator
                 2 : use X24M2P oscillator  , need 12MHz crystal oscillator
                 3 : use X24M2P oscillator  , need 24MHz crystal oscillator
   @return  int  --------error flag
   @note  	int rtc_clock_src_select(u8 value)
******************************************************************************/
int rtc_clock_src_select(u8 value);

/**************************************************************************
   @brief portr0-port3 is support 4s to reset cpu
   @param  u8 port , u8  enable , u8 edge
                port to choose io  ,value for  PORTR0/PORTR1/PORTR2/PORTR3
                enable   to SET ENABLE  ,value for ENABLE/DISABLE
                edge    to choose EDGE   ,value for  LOW_RESET/HIGH_RESET
                ps: other value will return RTC_MODULE_PARAM_ERROR
   @return  int  --------error flag
   @note  	int rtc_module_port_4s_reset(u8 port , u8  enable , u8 edge )
******************************************************************************/
int rtc_module_port_4s_reset(u8 port , u8  enable , u8 edge );

int rtc_module_ldo5v_detect(u8  enable , u8 edge );
int get_ldo5v_detect_flag();
/**************************************************************************
   @brief    set counter PR WAKE UP
   @param  u8 port , u8  enable , u8 edge
                port to choose io  ,value for   PORTR0/PORTR1/PORTR2/PORTR3
                enable   to SET ENABLE  ,value for ENABLE/DISABLE
                edge    to choose EDGE   ,value for UP_EDGE/FALL_EDGE
                init_cnt    to set counter init value , 0-254
                ps: other value will return RTC_MODULE_PARAM_ERROR

   @return int  --------error flag
   @note int rtc_module_pluse_cnt_wake_up(u8 port , u8  enable , u8 edge, u8 init_cnt)
******************************************************************************/
int rtc_module_pluse_cnt_wake_up(u8 port , u8  enable , u8 edge, u8 init_cnt);
int get_pluse_cnt_value();
int set_pluse_cnt_value(u8 value);
/*1: PR1 port voltage  drive ADC  , 0: no this function*/
//if you want to use port1 to be ADKEY io, you can set as below
/*	PORTR1_ADCEN_CTL(1);
	PORTR_PD(PORTR1 , 0);
	PORTR_PU(PORTR1 , 0);
	PORTR_DIR(PORTR1 , 1);
	PORTR_DIE(PORTR1 , 0);*/
void PORTR1_ADCEN_CTL(u8 val);
void PORTR2_ADCEN_CTL(u8 val);


u8 get_wake_up_type(void);
u8 get_wake_up_io(void);
#endif // __RTC_API_H__
