/***********************************Jieli tech************************************************
  File : timer.c
  By   : Juntham
  date : 2014-07-04 11:21
********************************************************************************************/
#include "sdk_cfg.h"
#include "timer.h"
#include "common/jiffies.h"
#include "rtos/os_api.h"
#include "key_drv/key.h"
#include "rtos/task_manage.h"
#include "clock_api.h"
#include "rtc/rtc.h"
#include "sys_detect.h"
#include "ui/led/led.h"
#include "key_drv/key_drv_ad.h"
#include "key_drv/key_drv_ir.h"
#include "ui/led/led7_drv.h"
#include "irq_api.h"
#include "clock.h"
/*
中断用到的变量可以都定义到结构体，
然后通过接口获取，方便管理
*/
extern void light_timer_10ms(void);
extern void sys_timer_schedule();
struct sys_global_var_str
{
    volatile u8 sys_halfsec;
    volatile u32 g_2ms_seed ;
};

struct sys_global_var_str g_var;

///定时器0计数器，时间片轮换
__timer_handle  *timer0_hl;
u16 t0_cnt1;
u16 t0_cnt2;
extern u8 power_on_cnt;
/*----------------------------------------------------------------------------*/
/**@brief  get halfsec flag
   @param
   @return
   @note

 */
/*----------------------------------------------------------------------------*/
u8 get_sys_halfsec(void)
{
    return g_var.sys_halfsec;
}
/*---------------------------------zhgq 2016-11-08 start-------------------------------------------*/
#ifdef CODER_WHELL
/*----------------------------------------------------------------------------*/
#define WHELLSPEED 4 // must be X*4
#define phase_no_move 0
#define phase_dec 1
#define phase_inc 2
#define phase_bad 3
/*----------------------------------------------------------------------------*/
void Coder_check(void)
{
	const u8 table_Phase[16]=
	{
		phase_no_move,phase_dec,phase_inc,phase_bad,
		phase_inc,phase_no_move,phase_bad,phase_dec,
		phase_dec,phase_bad,phase_no_move,phase_inc,
		phase_bad,phase_inc,phase_dec,phase_no_move,
	};
	static s8 PhaseCount=0;
	static u8 PhaseShift=0;
	CODER_INIT();
	u8 key=NO_KEY;
	PhaseShift<<=2;
	if(CODER_READ_LEFT())
	{
		PhaseShift|=0x02;
	}

	if(CODER_READ_RIGHT())
	{
    	PhaseShift|=0x01;
	}
	PhaseShift &= 0x0f;
	if(table_Phase[PhaseShift]==phase_inc) PhaseCount++;
	if(table_Phase[PhaseShift]==phase_dec) PhaseCount--;
	if(PhaseCount>=WHELLSPEED)
	{
		PhaseCount-=WHELLSPEED;
		os_taskq_post_msg("MainTask", 1, MSG_VOL_UP);
	}
	else if(PhaseCount<=-WHELLSPEED)
	{

		PhaseCount+=WHELLSPEED;
		os_taskq_post_msg("MainTask", 1, MSG_VOL_DOWN);
	}
	if(key!=NO_KEY)
	{

		OSTaskQFlush("MainTask");
	}
}
#endif

/*--------------------------end--------------------------------------------------*/
/**@brief  get 2ms's count
   @param
   @return counkt
   @note

 */
/*----------------------------------------------------------------------------*/
u32 get_sys_2msCount(void)
{
    return g_var.g_2ms_seed;
}
int fun_printf;
void halfsec_msg_sender(char *name, u32 msg)
{
	u32 cnt = 0;
	os_taskq_count(name, &cnt);
	if(cnt > 8)
	{
		os_taskq_post_msg(name,1,msg);
		fun_printf = 0;
	}
	else
	{
		u32 tmp;	
		fun_printf++;
		OS_TCB *self_task = os_get_task_tcb(OS_TASK_SELF);
		/*asm("mov %0, RETI " : "=r" (tmp));
		printf("--half_second = %s	",name);
		printf("msg = 0x%x\n",msg);
		printf("RETI = 0x%x\n",tmp);
		printf("PRIO = 0x%x\n",self_task->OSTCBPrio);*/
	}
}
void timer0_isr_callback()
{
    t0_cnt1++;
    t0_cnt2++;
    g_var.g_2ms_seed++;
#ifdef CODER_WHELL
	Coder_check();
#endif

    if((t0_cnt1 % 5) == 0)
    {
        OSTimeTick();
    }

    if((t0_cnt2 % 5) == 0)
    {

#if RCSP_LIGHT_HW
		light_timer_10ms();
#endif
		sys_timer_schedule();
#ifdef ADK_VOL_EN
		deal_ad_vol_change(adc_value[AD_CH_KEY_VOL]>>2);
#endif
	}

    if(t0_cnt1 == 250)
    {
        t0_cnt1 = 0;
		/* puts("."); */

        g_var.sys_halfsec = !g_var.sys_halfsec;//led7 driver
		halfsec_msg_sender(keymsg_task_name,MSG_HALF_SECOND);
    }


	struct list_head *pos;
    __timer_handle  *timer;
	list_for_each(pos,&(timer0_hl->list))
	{
	    timer = list_entry(pos,__timer_handle,list);

   		if(++timer->cnt >= timer->time)
   		{
   			timer->cnt = 0;
   			if(timer->handler)
   			{
   		        timer->handler(timer->param);
   			}
   		}
	}
}

s32 timer0_init(void)
{
	s32 ret;
    __timer_param   timer_parm;
	timer_module_on();
	timer0_hl = timer_open(TIMER0,TIMER_MAX_ISR_FUN);
	if(NULL == timer0_hl)
	{
		printf("timer_open err");
		ret = TIMER_DRV_OPEN_ERR;
		return ret;
	}
    timer_parm.work_mode  = TIMER_WORK_MODE_COUNTER;
    timer_parm.tick_time  = 2000;//(2ms)
	ret = timer_init_api(timer0_hl,&timer_parm);

	if(ret != TIMER_NO_ERR)
	{
		printf("timer_init err = %x\n",ret);
		return ret;
	}

	ret = timer_start(timer0_hl);
	if(ret != TIMER_NO_ERR)
	{
		printf("timer_start err = %x\n",ret);
		return ret;
	}
	ret = timer_reg_isr_callback_fun(timer0_hl,1,timer0_isr_callback);
	if(ret != TIMER_NO_ERR)
	{
		printf("timer_reg_isr_callback_fun err = %x\n",ret);
		return ret;
	}
	return ret;
}

static s32 timer0_clk_reset(void)
{
	s32 ret;
    __timer_param   timer_parm;
	puts("timer_clk_reset\n");

    timer_parm.work_mode  = TIMER_WORK_MODE_COUNTER;
    timer_parm.tick_time  = 2000;//(2ms)

	ret = timer_init_api(timer0_hl,&timer_parm);
	if(ret != TIMER_NO_ERR)
	{
		printf("timer_init err = %x\n",ret);
		return ret;
	}

	ret = timer_start(timer0_hl);
	if(ret != TIMER_NO_ERR)
	{
		printf("timer_start err = %x\n",ret);
		return ret;
	}

	puts("timer_init_OK\n");
	return ret;
}

static struct clock_switch clock_switch_timer;

CLOCK_SWITCH_USER_REGISTER(CLOCK_SWITCH_EXIT, &clock_switch_timer, timer0_clk_reset, "TIMER1");


