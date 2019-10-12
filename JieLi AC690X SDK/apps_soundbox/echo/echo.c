#include "common/app_cfg.h"
#include "rtos/os_api.h"
#include "rtos/os_cfg.h"
#include "common/error.h"
#include "common/msg.h"
#include "rtos/task_manage.h"
#include "dac/dac_api.h"
#include "play_sel/play_sel.h"
#include "key_drv/key.h"
#include "ui/led/led.h"
#include "dac/dac_api.h"
#include "dac/ladc.h"
#include "ui/ui_api.h"
#include "echo/echo_api.h"
#include "echo_ui.h"
#include "echo_prompt.h"
#include "echo_key.h"
#include "echo.h"
#include "record.h"
#include "encode/encode.h"


#if ECHO_EN

#if ECHO_TASK_EN
/*----------------------------------------------------------------------------*/
/**@brief  ECHO任务
   @param  NON
   @return 无
   @note   static void echo_init(void)
*/
/*----------------------------------------------------------------------------*/
static void echo_init(void)
{
    dac_channel_on(ECHO_CHANNEL, FADE_ON);

//    set_sys_vol(dac_var.cur_sys_vol_l, dac_var.cur_sys_vol_r, FADE_ON);

    dac_set_samplerate(16000,0); //DAC采样率设置
    ui_open_echo(NULL, 0);
}

/*----------------------------------------------------------------------------*/
/**@brief  ECHO任务
   @param  p：任务间参数传递指针
   @return 无
   @note   static void linein_task(void *p)
*/
/*----------------------------------------------------------------------------*/
static void echo_mic_task(void *p)
{
    int msg[3];
    void *echo_mic_reverb;

    echo_puts("\n************************ECHO_MIC TASK********************\n");
    led_fre_set(15,0);

    tone_play_by_name(ECHO_TASK_NAME, 1, BPF_ECHO_MP3); //BPF_ECHO_MP3提示音播放

    while(1)
    {
        os_taskq_pend(0, ARRAY_SIZE(msg), msg);
        clear_wdt();
        switch(msg[0])
        {
        case SYS_EVENT_DEL_TASK:
            if (os_task_del_req_name(OS_TASK_SELF) == OS_TASK_DEL_REQ)
            {
                ui_close_echo();
                echo_puts("ECHO_SYS_EVENT_DEL_TASK\n");

                play_sel_stop(); //关闭提示音
                echo_exit(&echo_mic_reverb);
                dac_channel_off(ECHO_CHANNEL, FADE_ON);
                os_task_del_res_name(OS_TASK_SELF);
            }
            break;

        case SYS_EVENT_PLAY_SEL_END: //提示音结束
            echo_puts("ECHO_SYS_EVENT_PLAY_SEL_END\n");
            play_sel_stop(); //关闭提示音
            echo_init();
            os_taskq_post_msg(OS_TASK_SELF, 1, MSG_ECHO_START);
            break;

#if 0//LCD_SUPPORT_MENU
            case MSG_ENTER_MENULIST:
                UI_menu_arg(MENU_LIST_DISPLAY,UI_MENU_LIST_ITEM);
            break;
#endif

        case MSG_HALF_SECOND:
//                echo_puts(" Aux_H ");
            break;

        default:
            echo_msg_deal(&echo_mic_reverb, msg);
            break;
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  ECHO 任务创建
   @param  priv：任务间参数传递指针
   @return 无
   @note   static void echo_task_init(void *priv)
*/
/*----------------------------------------------------------------------------*/
static void echo_task_init(void *priv)
{
    u32 err;
    err = os_task_create(echo_mic_task,
                         (void*)0,
                         TaskEchoMicPrio,
                         10
#if OS_TIME_SLICE_EN > 0
                         ,1
#endif
                         ,ECHO_TASK_NAME);

    if(OS_NO_ERR == err)
    {
        key_msg_register(ECHO_TASK_NAME, echo_ad_table, echo_io_table, echo_ir_table, NULL);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  ECHO 任务删除退出
   @param  无
   @return 无
   @note   static void echo_task_exit(void)
*/
/*----------------------------------------------------------------------------*/
static void echo_task_exit(void)
{
    if (os_task_del_req(ECHO_TASK_NAME) != OS_TASK_NOT_EXIST)
    {
        os_taskq_post_event(ECHO_TASK_NAME, 1, SYS_EVENT_DEL_TASK);
        do
        {
            OSTimeDly(1);
        }
        while(os_task_del_req(ECHO_TASK_NAME) != OS_TASK_NOT_EXIST);
        echo_puts("del_echo_task: succ\n");
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  ECHO 任务信息
   @note   const struct task_info echo_task_info
*/
/*----------------------------------------------------------------------------*/
TASK_REGISTER(echo_task_info) =
{
    .name = ECHO_TASK_NAME,
    .init = echo_task_init,
    .exit = echo_task_exit,
};
#endif
#endif
