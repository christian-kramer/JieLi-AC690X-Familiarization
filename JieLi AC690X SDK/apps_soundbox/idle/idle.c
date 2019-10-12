#include "common/app_cfg.h"
#include "rtos/os_api.h"
#include "rtos/os_cfg.h"
#include "common/error.h"
#include "common/msg.h"
#include "rtos/task_manage.h"
#include "sys_detect.h"
#include "dac/dac_api.h"
#include "play_sel/play_sel.h"
#include "key_drv/key.h"
#include "idle_key.h"
#include "idle.h"
#include "ui/led/led.h"
#include "play_sel/play_sel.h"
#include "rcsp/rcsp_interface.h"
#include "ui/ui_api.h"

extern void *malloc_fun(void *ptr,u32 len,char *info);
extern void free_fun(void **ptr);
/*******************zhgq2016-11-03**********************************/
u8 auto_power_off_cnt = 0;
u8 power_voice_flag=0;
u8 power_off_flag=0x00;
extern bool g_ad_led_flag;

/*----------------------------------------------------------------------------*/
/**@brief  Idle 任务
   @param  p：任务间参数传递指针
   @return 无
   @note   static void idle_task(void *p)
*/
/*----------------------------------------------------------------------------*/
static void idle_task(void *p)
{
	int msg[3];

	idle_puts("\n************************Idle TASK********************\n");
	//led_fre_set(15,0);
    //tone_play(IDLE_TASK_NAME, BPF_IDLE_MP3); //Idle提示音播放
//  os_taskq_post_msg(IDLE_TASK_NAME, 1, MSG_IDLE_MODE); //触发idle模块初始化
	  going_to_pwr_off=1;
	  B_LED_OFF();
	//  R_LED_ON();	  
	  os_time_dly(10);
	//  R_LED_ON();		
	  tone_play_by_name(IDLE_TASK_NAME,1,BPF_POWER_OFF_MP3);

	while(1)
	{
		memset(msg,0x00,sizeof(msg));
		os_taskq_pend(0, ARRAY_SIZE(msg), msg);
		clear_wdt();
		switch(msg[0])
		{
			case SYS_EVENT_DEL_TASK:
				if (os_task_del_req_name(OS_TASK_SELF) == OS_TASK_DEL_REQ)
				{
				    idle_puts("IDLE_SYS_EVENT_DEL_TASK\n");
				    //play_sel_stop(); //关闭提示音
					os_task_del_res_name(OS_TASK_SELF);
				}
				break;

            case SYS_EVENT_PLAY_SEL_END: //提示音结束
                idle_puts("IDLE_SYS_EVENT_PLAY_SEL_END\n");
/*********************zhgq2016-11-03*************************************/
                play_sel_stop(); //关闭提示音
#ifdef POWER_VOICE
				if(power_voice_flag)
				{
				
					puts("power_voice_flag\n");
					os_taskq_post_msg(MAIN_TASK_NAME, 1, MSG_EXIT_IDLE_TASK);
				}
				else
#endif
				{
#if 1//def SYS_POWER_KEY_EN
					if(power_off_flag)
					{
						puts("power_off_flag\n");
						auto_power_off_cnt = 1;
					}
					os_taskq_post_msg(IDLE_TASK_NAME, 1, MSG_IDLE_MODE);
#endif
				}
/********************************************************************/
                break;

            case MSG_IDLE_MODE:
					power_off_flag=1;
					pa_mute();
					dac_mute(1,0);
					os_time_dly(5);
			//		g_ad_led_flag=1;
					//JL_PORTB->DIR |= BIT(9);
					//JL_PORTB->PD &= ~BIT(9);
					//JL_PORTB->PU &= ~BIT(9);
				//	led_fre_set(0,0);
                	puts("MSG_IDLE_MODE\n");
//                ui_open_idle(NULL,0);
                //low_power_mode(WAKEUP_13,WAKEUP_DOWN_EDGE);
                break;

            case MSG_IDLE_CTL:
                puts("MSG_IDLE_CTL\n");
                os_taskq_post_msg(MAIN_TASK_NAME, 1, MSG_EXIT_IDLE_TASK);
                break;

            case MSG_HALF_SECOND:
/*******************2016-11-03************************************************/
#ifdef SYS_POWER_KEY_EN
			if(auto_power_off_cnt)
			{
				if(!(--auto_power_off_cnt))
				{
				
					//led_close();
			
				//	JL_PORTB->PU &= ~BIT(9);
				//	JL_PORTB->PD &= ~BIT(9);
				//	JL_PORTB->DIR &= ~BIT(9);
				//	puts("sys reset\n");
			
				//	soft_power_ctl(PWR_OFF);
				}
			}
#endif
/***************************************************************************/
                //idle_puts(" Idle_H ");
                break;

			default:
				break;
		}
	}
}


/*----------------------------------------------------------------------------*/
/**@brief  IDLE 任务创建
   @param  priv：任务间参数传递指针
   @return 无
   @note   static void idle_task_init(void *priv)
*/
/*----------------------------------------------------------------------------*/
static void idle_task_init(void *priv)
{
    u32 err;
	err = os_task_create(idle_task,
			(void*)0,
			TaskIdlePrio,
			10
#if OS_TIME_SLICE_EN > 0
			,1
#endif
			,IDLE_TASK_NAME);

    if(OS_NO_ERR == err)
    {
        key_msg_register(IDLE_TASK_NAME, idle_ad_table, idle_io_table, idle_ir_table, NULL);
    }
}


/*----------------------------------------------------------------------------*/
/**@brief  IDLE 任务删除退出
   @param  无
   @return 无
   @note   static void idle_task_exit(void)
*/
/*----------------------------------------------------------------------------*/
static void idle_task_exit(void)
{
	if (os_task_del_req(IDLE_TASK_NAME) != OS_TASK_NOT_EXIST)
	{
		os_taskq_post_event(IDLE_TASK_NAME, 1, SYS_EVENT_DEL_TASK);
		do{
			OSTimeDly(1);
		} while(os_task_del_req(IDLE_TASK_NAME) != OS_TASK_NOT_EXIST);
		idle_puts("del_idle_task: succ\n");
	}
}


/*----------------------------------------------------------------------------*/
/**@brief  IDLE 任务信息
   @note   const struct task_info idle_task_info
*/
/*----------------------------------------------------------------------------*/
TASK_REGISTER(idle_task_info) = 
{
	.name = IDLE_TASK_NAME,
	.init = idle_task_init,
	.exit = idle_task_exit,
};
