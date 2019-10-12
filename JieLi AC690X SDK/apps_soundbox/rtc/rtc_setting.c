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
#include "rtc.h"
#include "rtc_setting.h"
#include "sys_detect.h"
#if RTC_CLK_EN

RTC_SETTING rtc_set;

/**************************************************************
*Description: RTC时间设置
*Arguments  : NULL
*Returns    : NULL
*Note       :
*By         : WGW
**************************************************************/
void rtc_setting(void)
{
    int msg[3];

    rtc_set.calendar_set.coordinate = COORDINATE_MIN;
    rtc_set.alarm_set.coordinate = COORDINATE_MIN;

    rtc_set.calendar_set.rtc_set_cnt = 0;
    rtc_set.alarm_set.alm_set_cnt = 0;

    if(rtc_set.rtc_set_mode == RTC_SET_MODE)
        UI_menu(MENU_RTC_MAIN);
    else
        UI_menu(MENU_ALM_SET);

    while(1)
    {
        memset(msg,0x00,sizeof(msg));
        os_taskq_pend(0, ARRAY_SIZE(msg), msg);
        clear_wdt();
        if(MSG_HALF_SECOND!= msg[0])
        {
            rtc_set.calendar_set.rtc_set_cnt = 0;
            rtc_set.alarm_set.alm_set_cnt = 0;
        }

        switch(msg[0])
        {
//        case MSG_KEY_OK: //确定返回
//            rtc_printf("rtc set OK\n");
//            rtc_set.rtc_set_mode = RTC_DISPLAY;
////            ui_display(MSG_UI_RTC_TIME,&rtc_set,sizeof(RTC_SET),1);
//            goto _rtc_setting_exit;

        case MSG_RTC_SETTING://移动光标到下一项
            rtc_set.rtc_set_mode = RTC_SET_MODE;
            rtc_set.calendar_set.coordinate++;

            if(rtc_set.calendar_set.coordinate > COORDINATE_MAX)   //LED显示时只显示“时”、“分”，设置也只设置这两个
            {
                rtc_set.calendar_set.coordinate = COORDINATE_MIN;
            }
            UI_menu(MENU_RTC_MAIN);
            break;

        case MSG_ALM_SETTING://移动光标到下一项
            rtc_set.rtc_set_mode = ALM_SET_MODE;
            rtc_set.alarm_set.coordinate++;

            if(rtc_set.alarm_set.coordinate > COORDINATE_MAX)   //LED显示时只显示“时”、“分”，设置也只设置这两个
            {
                rtc_set.alarm_set.coordinate = COORDINATE_MIN;
            }
            UI_menu(MENU_ALM_SET);
            break;

        case MSG_RTC_PLUS:
            if(rtc_set.rtc_set_mode == RTC_SET_MODE)  //万年历
            {
                calendar_time_plus(rtc_set.calendar_set.curr_rtc_time,rtc_set.calendar_set.coordinate);
                rtc_module_write_rtc_datetime(rtc_set.calendar_set.curr_rtc_time);
                //rtc_clk_out(rtc_set.calendar_set.curr_rtc_time);
                UI_menu(MENU_RTC_MAIN);
            }
            else if(rtc_set.rtc_set_mode == ALM_SET_MODE)
            {
                calendar_time_plus(rtc_set.alarm_set.curr_alm_time,rtc_set.alarm_set.coordinate);
                rtc_module_write_alarm_datetime(rtc_set.alarm_set.curr_alm_time , rtc_set.alarm_set.current_alarm_mode);
                //rtc_clk_out(rtc_set.alarm_set.curr_alm_time);
                UI_menu(MENU_ALM_SET);
            }
            break;

        case MSG_RTC_MINUS:
            if(rtc_set.rtc_set_mode == RTC_SET_MODE)  	//万年历
            {
                calendar_time_minus(rtc_set.calendar_set.curr_rtc_time,rtc_set.calendar_set.coordinate);
                rtc_module_write_rtc_datetime(rtc_set.calendar_set.curr_rtc_time);
                //rtc_clk_out(rtc_set.calendar_set.curr_rtc_time);
                UI_menu(MENU_RTC_MAIN);
            }
            else if(rtc_set.rtc_set_mode == ALM_SET_MODE)
            {
                calendar_time_minus(rtc_set.alarm_set.curr_alm_time, rtc_set.alarm_set.coordinate);
                rtc_module_write_alarm_datetime(rtc_set.alarm_set.curr_alm_time , rtc_set.alarm_set.current_alarm_mode);
                //rtc_clk_out(rtc_set.alarm_set.curr_alm_time);
                UI_menu(MENU_ALM_SET);
            }
            break;

        case MSG_ALM_SW:
            //按键开关闹钟时候，自动改为设置闹钟模式
            rtc_set.rtc_set_mode = ALM_SET_MODE;
            if(rtc_set.alarm_set.alarm_sw)
            {
                rtc_set.alarm_set.alarm_sw = 0;
            }
            else
            {
                rtc_set.alarm_set.alarm_sw = 1;
            }
            rtc_module_alarm_switch(rtc_set.alarm_set.alarm_sw);
            UI_menu(MENU_ALM_SET);
            break;

        case MSG_HALF_SECOND:
            //时间设置计时
            if(rtc_set.rtc_set_mode == RTC_SET_MODE)
            {
                rtc_set.calendar_set.rtc_set_cnt++;
                //时间设置超时退出
                if(rtc_set.calendar_set.rtc_set_cnt == RTC_SETTING_CNT)
                {
                    rtc_set.calendar_set.rtc_set_cnt = 0;
                    rtc_printf("calendar_set timeout\n");
                    goto _rtc_setting_exit;
                }
            }

            if(rtc_set.rtc_set_mode == ALM_SET_MODE)
            {
                rtc_set.alarm_set.alm_set_cnt++;
                if(rtc_set.alarm_set.alm_set_cnt == ALM_SETTING_CNT)
                {
                    rtc_set.alarm_set.alm_set_cnt = 0;
                    rtc_printf("Alarm Set TimeOut\n");
                    goto _rtc_setting_exit;
                }
            }
//            UI_menu(MENU_MAIN);
//            UI_menu(MENU_HALF_SEC_REFRESH);
            break;

        default:
            rtc_printf("default : %d\n",msg[0]);
            if(msg[0] != MSG_RTC_POWER_DOWN)//MSG_RTC_POWER_DOWN消息返回不转发
                os_taskq_post(RTC_TASK_NAME, 1, msg[0]);//其他非设置的按键将转发回主线程
            goto _rtc_setting_exit;
            break;
        }
    }

_rtc_setting_exit:
    puts("_rtc_setting_exit\n");
    rtc_set.alarm_set.curr_alm_time->bSec = 0;
    rtc_module_write_alarm_datetime(rtc_set.alarm_set.curr_alm_time , rtc_set.alarm_set.current_alarm_mode);
    rtc_set.rtc_set_mode = RTC_DISPLAY;
    rtc_module_read_rtc_datetime(rtc_set.calendar_set.curr_rtc_time);
    UI_menu(MENU_RTC_MAIN);
    return;
}

#endif  /*RTC_CLK_EN*/
