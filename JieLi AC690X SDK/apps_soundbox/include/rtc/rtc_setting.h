#ifndef _RTC_SETTING_H_
#define _RTC_SETTING_H_

#include "includes.h"
#include "rtc/rtc_api.h"

#define RTC_SETTING_CNT   10///RTC 设置超时时间，500unit
#define ALM_SETTING_CNT  10///ALARM 设置超时时间，500unit

///1--once,2--every day,3-- one day for every week
#define ALARM_MODE_ONCE                      1
#define ALARM_MODE_EVERY_DAY              2
#define ALARM_MODE_EVERY_WEEK            3

//RTC显示类型
typedef enum
{
    RTC_DISPLAY = 0,    //RTC显示
    RTC_SET_MODE,        //R设置显示
    ALM_SET_MODE,      //闹钟设置显示
}ENUM_RTC_UI;

typedef struct _ALM_SET_
{
    RTC_TIME *curr_alm_time;   //当前闹钟时间
    u8 coordinate;            //闹钟设置位
    u8 alm_set_cnt;           //闹钟设置超时计数

    u8 alarm_flag;          //闹钟响闹标志
    u8 alarm_sw;            //闹钟开关
    u8 bAlarmOnCnt ;          //闹钟响闹计时

    u8 current_alarm_mode;   ///1--once,2--every day,3-- one day for every week
//    u8 alm_ring_mode;         //闹钟响闹模式
//    u8 alm_music_type;        //闹钟响闹提示音类型
}ALM_SET;

typedef struct _RTC_SET_
{
    RTC_TIME *curr_rtc_time;   //当前万年历时间
    u8 coordinate;            //RTC设置位
    u8 rtc_set_cnt;           //rtc设置超时计数
}RTC_SET;

typedef struct _RTC_SET_MODE_
{
    ALM_SET alarm_set;          //闹钟设置
    RTC_SET calendar_set;       //万年历设置
    u8 rtc_set_mode;            //RTC设置模式
}RTC_SETTING;


/*
**********************************************************************
*                           VARIABLES DECLARE
**********************************************************************
*/
extern RTC_SETTING rtc_set;
/*
**********************************************************************
*                           FUNCTIONS DECLARE
**********************************************************************
*/
extern void rtc_setting(void);

#endif
