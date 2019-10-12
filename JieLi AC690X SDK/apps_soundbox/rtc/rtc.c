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
#include "dac/dac.h"
#include "rtc.h"
#include "ui/led/led.h"
#include "rtc_ui.h"
#include "rtc/rtc_api.h"
#include "sys_detect.h"
static void auto_set_next_alarm(void);

#if RTC_CLK_EN
RTC_TIME current_time;
RTC_TIME current_alarm;

static void rtc_isr_user_handler(u8 flag)
{
    if(INTERRUPT_TYPE_ALARM_COME == flag)
    {
        rtc_puts("--ALM-ON-ISR--\n");
        rtc_set.alarm_set.alarm_sw = 0;
        rtc_module_alarm_switch(rtc_set.alarm_set.alarm_sw);  //响闹后关闭闹钟
        rtc_set.alarm_set.alarm_flag = 1;
//        if(0 == compare_task_name(RTC_TASK_NAME))
//        {
//            os_taskq_post_msg(RDEF_MAIN_TASK_NAME, 1, MSG_ALM_ON);
//        }
    }
    if(INTERRUPT_TYPE_PCNT_OVF==flag)
    {
        rtc_puts("--TYPE_PCNT_OVF--\n");
    }
    if(INTERRUPT_TYPE_LDO5V_DET==flag)
    {
        rtc_puts("--LDO5V DET--\n");
    }
}

/*----------------------------------------------------------------------------*/
/**@brief 	复位RTC时间
   @param 	void
   @return  void
   @note  	void reset_irtc(void)，根据当前时钟有效值，决定是否复位时钟
*/
/*----------------------------------------------------------------------------*/
static void reset_irtc(RTC_TIME *curr_time,RTC_TIME *curr_alm)
{
    rtc_module_read_rtc_datetime(curr_time);
    rtc_module_read_alarm_datetime(curr_alm);
    if (((curr_time->dYear > 2100)
            || (curr_time->dYear < 2000))
            || (curr_time->bMonth >= 12)
            || (curr_time->bHour >= 24)
            || (curr_time->bMin >= 60)
            || (curr_time->bSec >= 60)
            ||(rtc_module_get_power_up_flag()))
    {
        puts("\n--------RTC RESET--------\n");
        curr_time->dYear = 2016;
        curr_time->bMonth = 9;
        curr_time->bDay = 26;
        curr_time->bHour = 0;
        curr_time->bMin = 0;
        curr_time->bSec = 0;

        //puts("\n--------ALM RESET--------\n");
        memcpy(curr_alm,curr_time,sizeof(RTC_TIME));

        ///update date
        rtc_module_write_rtc_datetime(curr_time);
        rtc_module_write_alarm_datetime(curr_alm , 0);
    }
    rtc_set.alarm_set.alarm_sw = rtc_module_get_alarm_flag();
    rtc_set.calendar_set.curr_rtc_time = curr_time;
    rtc_set.alarm_set.curr_alm_time = curr_alm;

    rtc_module_reset_irtc();
}

/****************************************************

    note        :open rtc hardware
******************************************************/

void irtc_hw_init(void)
{
   /* PORTR_PU(PORTR0 , 0);
	PORTR_PU(PORTR0 , 0);
	PORTR_DIR(PORTR0 , 1);
	PORTR_DIE(PORTR0 , 1);*/

    if(rtc_module_get_alm_come_flag())
    {
        rtc_set.alarm_set.alarm_flag = 1;
        //update mode ,maybe powerup alarming
        rtc_set.alarm_set.current_alarm_mode = current_alarm.bSec & 0x03;
    }
    else
    {
        rtc_set.alarm_set.alarm_flag = 0;
    }
    rtc_module_on(WAKE_UP_ENABLE , 0 ,  rtc_isr_user_handler);
    reset_irtc(&current_time,&current_alarm);
}


/*----------------------------------------------------------------------------*/
/**@brief  RTC DAC通道选择，开启
   @param  无
   @return 无
   @note   void aux_dac_channel_on(void)
*/
/*----------------------------------------------------------------------------*/
static void rtc_dac_channel_on(void)
{
    dac_mute(1,1);
    dac_channel_on(RTC_CHANNEL, FADE_ON);
    os_time_dly(15);  //这个参数会影响到上电第一次进入line in的冲击声。可以根据样机调整
    dac_mute(0,1);

    set_sys_vol(dac_ctl.sys_vol_l , dac_ctl.sys_vol_r, FADE_ON);
}

/*----------------------------------------------------------------------------*/
/**@brief  RTC DAC通道选择，关闭
   @param  无
   @return 无
   @note   void aux_dac_channel_off(void)
*/
/*----------------------------------------------------------------------------*/
static void rtc_dac_channel_off(void)
{
    dac_channel_off(RTC_CHANNEL, FADE_ON);
    dac_mute(0,1);
}
/*----------------------------------------------------------------------------*/
/**@brief  RTC初始化
   @param  void
   @return 无
   @note   void rtc_info_init(void)
*/
/*----------------------------------------------------------------------------*/
static void rtc_info_init(void)
{
    rtc_module_alarm_switch(rtc_set.alarm_set.alarm_sw);

    rtc_dac_channel_on();

    ui_open_rtc(&rtc_set, sizeof(RTC_SETTING));
}

static void stop_alm(void)
{
    rtc_set.alarm_set.alarm_flag = 0;
    SET_UI_MAIN(MENU_RTC_MAIN);
    UI_menu(MENU_RTC_MAIN);
}
/*----------------------------------------------------------------------------*/
/**@brief  RTC 任务
   @param  p：任务间参数传递指针
   @return 无
   @note   static void rtc_task(void *p)
*/
/*----------------------------------------------------------------------------*/
static void rtc_task(void *p)
{
    int msg[6];
    //int rtc_init_flag = 0;
    rtc_puts("\n*********************RTC TASK********************\n");

   if(rtc_set.alarm_set.alarm_flag)
   {
        puts("alarm start...\n");
        os_taskq_post_msg(RTC_TASK_NAME, 1, MSG_RTC_INIT);
   }
   else
   {
        tone_play_by_name(RTC_TASK_NAME, 1 ,  BPF_RTC_MP3);
   }

    while(1)
    {
        memset(msg,0x00,sizeof(msg));
        os_taskq_pend(0, ARRAY_SIZE(msg), msg);
        clear_wdt();
        if(msg[0] != MSG_HALF_SECOND)
        {
            rtc_printf("rtc_msg: = 0x%x\n",msg[0]);
        }


        if(rtc_set.alarm_set.alarm_flag)        //闹钟响的时候，部分按键消息转为停止闹钟过滤
        {
            if((msg[0]==MSG_RTC_SETTING)
                    ||(msg[0]==MSG_ALM_SETTING)
                    ||(msg[0]==MSG_ALM_SW)
                    ||(msg[0]==MSG_RTC_POWER_DOWN)
                    ||(msg[0]==MSG_ENTER_MENUMAIN)
                    ||(msg[0]==MSG_ENTER_MENULIST))
            {
                rtc_puts("stop alarm_ui\n");
                stop_alm();
                rcsp_report_status(RCSP_REPORT_STATUS_CONTROL);
                if(rtc_set.alarm_set.current_alarm_mode > ALARM_MODE_ONCE)
                {
                    auto_set_next_alarm();
                }
                continue;
            }
            else
            {
            	;
			}

        }

        switch(msg[0])
        {
        case SYS_EVENT_DEL_TASK:
            if (os_task_del_req_name(OS_TASK_SELF) == OS_TASK_DEL_REQ)
            {
                if(rtc_set.alarm_set.alarm_flag)
                {
                    rtc_set.alarm_set.alarm_flag = 0;
                    rtc_puts("del alarm\n");
                }

                ui_close_rtc();
                rtc_puts("AUX_SYS_EVENT_DEL_TASK\n");
                rtc_set.alarm_set.alarm_flag = 0;//清闹钟标志
                rtc_dac_channel_off();
                play_sel_stop(); //关闭提示音
                os_task_del_res_name(OS_TASK_SELF);
            }
            break;

        case SYS_EVENT_PLAY_SEL_END: //提示音结束
            rtc_puts("RTC_SYS_EVENT_PLAY_SEL_END\n");
            play_sel_stop(); //关闭提示音
            rtc_puts("RTC_play_tone_end\n");
            os_taskq_post_msg(RTC_TASK_NAME, 1, MSG_RTC_INIT); //触发AUX模块初始化
            break;

        case MSG_RTC_INIT:
            rtc_puts("MSG_RTC_INIT\n");
            rtc_info_init();
            break;

        case MSG_RTC_SETTING:                  ///RTC时间设置
            rtc_puts("MSG_RTC_SETTING\n");
            rtc_set.rtc_set_mode = RTC_SET_MODE;
            rtc_setting();
            break;

#if RTC_ALM_EN
        case MSG_ALM_SETTING:                     ///闹钟时间设置
            rtc_puts("MSG_ALM_SETTING\n");
            rtc_set.alarm_set.alarm_sw = 1;
            rtc_set.alarm_set.current_alarm_mode = ALARM_MODE_ONCE;         ///keymsg,only once alarm
            rtc_module_alarm_switch(rtc_set.alarm_set.alarm_sw);
            rtc_set.rtc_set_mode = ALM_SET_MODE;
            rtc_setting();
            break;
#endif

        case MSG_ALM_SW:
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

        case MSG_RTC_POWER_DOWN:
			puts("enter low power\n");
			set_keep_osci_flag(1);
			soft_poweroff();
            break;

        case MSG_LOW_POWER:
            puts("rtc_low_power\n");
            //tone_play_by_name(RTC_TASK_NAME, BPF_LOW_POWER_MP3);
            break;

#if LCD_SUPPORT_MENU
        case MSG_ENTER_MENULIST:
            UI_menu_arg(MENU_LIST_DISPLAY,UI_MENU_LIST_ITEM);
            break;
#endif

        case MSG_HALF_SECOND:
            if(rtc_set.alarm_set.alarm_flag)
            {
                SET_UI_MAIN(MENU_ALM_UP);
                UI_menu(MENU_ALM_UP);
                puts("alarm ring\n");
                send_key_voice(500);
            }
            else
            {
                rtc_module_read_rtc_datetime(&current_time);
                UI_menu(MENU_HALF_SEC_REFRESH);
                rtc_printf("rtc time,%d-%d-%d, %d:%d:%d\n",current_time.dYear,current_time.bMonth,current_time.bDay,\
                       current_time.bHour,current_time.bMin,current_time.bSec);
            }

            break;

        default:
            break;
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  根据闹钟模式设置循环闹钟的日期
   @param  none
   @return none
   @note
*/
/*----------------------------------------------------------------------------*/
static void auto_set_next_alarm(void)
{
    if(rtc_set.alarm_set.current_alarm_mode == ALARM_MODE_EVERY_DAY)
    {
        //every day
        rtc_calculate_next_day(&current_alarm);
    }
    else
    {
        //one day for every week
        rtc_calculate_next_weekday(&current_alarm);
    }

    rtc_printf("auto_set_alarm:%d-%d-%d,%d:%d:%d\n",current_alarm.dYear,current_alarm.bMonth,current_alarm.bDay,\
           current_alarm.bHour,current_alarm.bMin,current_alarm.bSec);

    rtc_module_write_alarm_datetime(&current_alarm , rtc_set.alarm_set.current_alarm_mode);
    rtc_set.alarm_set.alarm_sw = 1;
    rtc_module_alarm_switch(rtc_set.alarm_set.alarm_sw);
}


/*----------------------------------------------------------------------------*/
/**@brief  RTC 任务创建
   @param  priv：任务间参数传递指针
   @return 无
   @note   static void rtc_task_init(void *priv)
*/
/*----------------------------------------------------------------------------*/
static void rtc_task_init(void *priv)
{
    u32 err;

    err = os_task_create(rtc_task, 0, TaskRTCPrio, 40, RTC_TASK_NAME);
    if(OS_NO_ERR == err)
    {
        key_msg_register(RTC_TASK_NAME, rtc_ad_table, rtc_io_table, rtc_ir_table, NULL);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  RTC 任务删除退出
   @param  无
   @return 无
   @note   static void rtc_task_exit(void)
*/
/*----------------------------------------------------------------------------*/
static void rtc_task_exit(void)
{
    if (os_task_del_req(RTC_TASK_NAME) != OS_TASK_NOT_EXIST)
    {
        os_taskq_post_event(RTC_TASK_NAME, 1, SYS_EVENT_DEL_TASK);
        do
        {
            OSTimeDly(1);
        }
        while(os_task_del_req(RTC_TASK_NAME) != OS_TASK_NOT_EXIST);
        rtc_puts("del_rtc_task: succ\n");
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  RTC 任务信息
   @note   const struct task_info linein_task_info
*/
/*----------------------------------------------------------------------------*/
TASK_REGISTER(rtc_task_info) =
{
    .name = RTC_TASK_NAME,
    .init = rtc_task_init,
    .exit = rtc_task_exit,
};

#endif
