#include "common/app_cfg.h"
#include "rtos/os_api.h"
#include "rtos/os_cfg.h"
#include "common/error.h"
#include "common/msg.h"
#include "rtos/task_manage.h"
#include "linein_key.h"
#include "linein.h"
#include "dac/dac_api.h"
#include "play_sel/play_sel.h"
#include "key_drv/key.h"
#include "ui/led/led.h"
#include "dac/dac_api.h"
#include "dev_linein.h"
#include "ui/ui_api.h"
#include "dac/ladc.h"
#include "linein_prompt.h"
#include "linein_ui.h"
#include "echo/echo_api.h"
#include "echo/echo_deal.h"
#include "record.h"
#include "encode/encode.h"
#include "key_drv/key_voice.h"
#include "dac/dac.h"
#include "dac/dac_api.h"
#include "rcsp/rcsp_interface.h"
#include "dac/eq.h"
#include "vm/vm.h"
#include "vm/Vm_api.h"
extern void *malloc_fun(void *ptr, u32 len, char *info);
extern void free_fun(void **ptr);
extern u8 eq_mode;
#if USE_SOFTWARE_EQ 
static void aux_eq_task_init();
static void aux_eq_task_stop();
static void aux_eq_run(void *p);
#endif

RECORD_OP_API * aux_rec = NULL;
REVERB_API_STRUCT *aux_reverb = NULL;
#define get_mute_status   is_dac_mute
/******************************2016-11-29********************************************/
#ifdef MEMERY_MODE
extern u8 curr_work_mode;
#endif
#if MAX_VOL_VOICE
extern u8 key_short_cnt;
u8 linein_voice_cnt=0;
#endif
/*************************************************************************************/

/*----------------------------------------------------------------------------*/
/**@brief  AUX DAC通道选择，开启
 @param  无
 @return 无
 @note   void aux_dac_channel_on(void)
 */
/*----------------------------------------------------------------------------*/
void aux_dac_channel_on(void)
{
	if(LINEIN_CHANNEL == DAC_AMUX0)
	 {
    	JL_PORTA->DIR |=  (BIT(1)|BIT(2));
    	JL_PORTA->DIE &= ~(BIT(1)|BIT(2));
	}
	else if(LINEIN_CHANNEL == DAC_AMUX1)
	{
    	JL_PORTA->DIR |=  (BIT(4)|BIT(3));
    	JL_PORTA->DIE &= ~(BIT(4)|BIT(3));
		JL_PORTA->PD  &= ~(BIT(3));//PA3 default pull_down 
	}
	else if(LINEIN_CHANNEL == DAC_AMUX2)
	{
    	JL_PORTB->DIR |=  (BIT(11)|BIT(12));
    	JL_PORTB->DIE &= ~(BIT(11)|BIT(12));
	}
	else if(LINEIN_CHANNEL == DAC_AMUX1_L_ONLY)
	{
	
	}
	//
#if AUX_AD_ENABLE
	
#if USE_SOFTWARE_EQ  
	aux_eq_task_init();
#endif
    dac_channel_on(MUSIC_CHANNEL, FADE_ON);
	set_sys_vol(0,0,FADE_ON);
    dac_set_samplerate(48000,0);
    ladc_reg_init(ENC_LINE_LR_CHANNEL , LADC_SR48000);
	amux_channel_en(LINEIN_CHANNEL,1);
	os_time_dly(35);
	set_sys_vol(dac_ctl.sys_vol_l,dac_ctl.sys_vol_r,FADE_ON);
#else
    dac_channel_on(LINEIN_CHANNEL, FADE_ON);
    os_time_dly(20);//wait amux channel capacitance charge ok
	set_sys_vol(dac_ctl.sys_vol_l,dac_ctl.sys_vol_r,FADE_ON);
	dac_ldo_set(LDO_1,0);
	#if ECHO_EN
	///开启dac数字通道
	dac_trim_en(0);
	dac_digital_en(1);
	#else
	///关闭dac数字通道
	dac_trim_en(1);
	dac_digital_en(0);
	#endif//end of ECHO_EN
#endif//end of AUX_AD_ENABLE
	JL_PORTA->PD &= ~BIT(3);
	JL_PORTA->PU &= ~BIT(3);
	JL_PORTA->DIR |= BIT(3);
	JL_PORTA->DIE &= ~BIT(3);
	pa_umute();

}

/*----------------------------------------------------------------------------*/
/**@brief  AUX DAC通道选择，关闭
 @param  无
 @return 无
 @note   void aux_dac_channel_off(void)
 */
/*----------------------------------------------------------------------------*/
void aux_dac_channel_off(void)
{
#if AUX_AD_ENABLE
#if USE_SOFTWARE_EQ 
	aux_eq_task_stop();
#endif	
	dac_channel_off(MUSIC_CHANNEL,FADE_ON);
	ladc_close(ENC_LINE_LR_CHANNEL);
	amux_channel_en(LINEIN_CHANNEL,0);
#else
    dac_channel_off(LINEIN_CHANNEL, FADE_ON);
	dac_trim_en(0);
	/* dac_mute(0, 1); */
#endif
	dac_ldo_set(LDO_1,1);
}

void aux_info_init(void)
{
    aux_dac_channel_on();

    //#if (AUX_REC_EN==0)
    //    ladc_reg_init(ENC_DAC_CHANNEL,LADC_SR16000);   //获取能量要初始化用于采数
    //#endif

	ui_open_aux(&aux_rec,sizeof(RECORD_OP_API **));
    /* ui_open_aux(0, 0); */
}

/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 任务
 @param  p：任务间参数传递指针
 @return 无
 @note   static void linein_task(void *p)
 */
/*----------------------------------------------------------------------------*/
static void linein_task(void *p)
{
    int msg[6];
    aux_reverb = NULL;
    tbool psel_enable;
	u8 line_do_mute_flag = 0;

    aux_puts("\n************************LINEIN TASK********************\n");
    malloc_stats();
    led_fre_set(15,0);

    psel_enable = tone_play_by_name(LINEIN_TASK_NAME,1, BPF_LINEIN_MP3); //line in提示音播放

#if (AUX_AD_ENABLE && EQ_EN)
	eq_enable();
    eq_mode = get_eq_default_mode();
#endif
/****************************2016-11-29***********************************************/
#ifdef MEMERY_MODE
		curr_work_mode=0;
		curr_work_mode |= BIT(AUX_MODE);
		vm_write_api(VM_MODE,&curr_work_mode);
#endif
#if MAX_VOL_VOICE
	key_short_cnt = 0;
#endif

    while (1)
    {
        memset(msg, 0x00, sizeof(msg));
        os_taskq_pend(0, ARRAY_SIZE(msg), msg);

        if(psel_enable)
		{
            switch(msg[0])
            {
            case SYS_EVENT_PLAY_SEL_END:
			case SYS_EVENT_DEL_TASK: 				//请求删除music任务
                puts("\n----play_sel_stop------\n");
                play_sel_stop();
                psel_enable = 0;
                break;
            default:
                msg[0] = NO_MSG;
                break;
            }
		}

        if(msg[0] != MSG_HALF_SECOND)
        {
            printf("\nlinein_msg= 0x%x\n",msg[0]);
        }

        clear_wdt();
#if SUPPORT_APP_RCSP_EN
		rcsp_linein_task_msg_deal_before(msg);
#endif
		switch (msg[0])
        {
            case SYS_EVENT_DEL_TASK:
	            if (os_task_del_req_name(OS_TASK_SELF) == OS_TASK_DEL_REQ)
	            {
	                //				    UI_menu(MENU_WAIT);
	                aux_puts("AUX_SYS_EVENT_DEL_TASK\n");
	                ui_close_aux();

	                //                  exit_rec_api(&rec_linin_api); //停止录音和释放资源
	                echo_exit_api((void **)&aux_reverb);
                	exit_rec_api(&aux_rec); //停止录音和释放资源
	                //				    disable_ladc(ENC_DAC_CHANNEL);
	                aux_dac_channel_off();
	                if(line_do_mute_flag)
					{
						puts("line unmute\n");
						line_do_mute_flag = 0;
						dac_mute(0, 1);
					}
					os_task_del_res_name(OS_TASK_SELF);
	            }
	            break;

            case SYS_EVENT_PLAY_SEL_END: //提示音结束
				pa_mute();
	            aux_puts("AUX_SYS_EVENT_PLAY_SEL_END\n");
	            if(linein_prompt_break)
	            {
	                linein_prompt_break = 0;
	                break;
	            }
#if MAX_VOL_VOICE
				if(key_short_cnt == 1)
				{
					key_short_cnt = 2;
					break;
				}
#endif
	            aux_puts("AUX_play_tone_end\n");
            case MSG_AUX_INIT:
	            aux_puts("MSG_AUX_INIT\n");
	            aux_info_init();
	            break;

            case MSG_AUX_MUTE:

	            if (get_mute_status())
	            {
	                aux_puts("MSG_AUX_UNMUTE\n");
	                dac_mute(0, 1);
                    /* send_key_voice(500); */
	                led_fre_set(15,0);
				}
	            else
	            {
	                aux_puts("MSG_AUX_MUTE\n");
	                os_time_dly(30);
	                dac_mute(1, 1);
	                led_fre_set(0,0);
	            }
				line_do_mute_flag = 1; 
                UI_menu(MENU_REFRESH);
            	break;

#if AUX_AD_ENABLE
			case MSG_MUSIC_EQ:
				eq_mode++;
				if(eq_mode > 5)
					eq_mode = 0;
				printf("Aux_EQ:%d\n",eq_mode);
				eq_mode_sw(eq_mode);
				break;
#endif
			case MSG_PROMPT_PLAY:
            case MSG_LOW_POWER:
	            puts("aux_low_power\n");
				aux_dac_channel_off();
	            linein_prompt_play(BPF_LOW_POWER_MP3);
	            break;
			case MSG_MAX_VOL:
				aux_dac_channel_off();
				linein_prompt_play(BPF_MAX_VOL_MP3);
				break;
			case MSG_LOW_POWER_VOICE:
				aux_dac_channel_off();
	            linein_prompt_play(BPF_LOW_POWER_MP3);
	            break;

#if LCD_SUPPORT_MENU
            case MSG_ENTER_MENULIST:
	            UI_menu_arg(MENU_LIST_DISPLAY,UI_MENU_LIST_ITEM);
	            break;
#endif

            case MSG_HALF_SECOND:
/********************************************/
#if MAX_VOL_VOICE
				if(key_short_cnt == 3)
				{
					key_short_cnt = 0;
	                os_taskq_post_msg(LINEIN_TASK_NAME, 1, SYS_EVENT_PLAY_SEL_END); //触发AUX模块初始化
				}
				if(key_short_cnt == 2)
				{
					key_short_cnt = 3;
				}
#endif
/***************************************/
	            //aux_puts(" Aux_H ");
#if AUX_REC_EN
	            if(updata_enc_time(aux_rec))
	            {
	                UI_menu(MENU_HALF_SEC_REFRESH);
	            }
#endif

	            break;

            default:
				if(msg[0] == MSG_ECHO_START)//start echo
				{
                	exit_rec_api(&aux_rec); //stop rec
				}
				else if (msg[0] == MSG_REC_START)//start rec
				{
	                echo_exit_api((void **)&aux_reverb);//stop echo
				}
				echo_msg_deal_api((void **)&aux_reverb, msg);
	            rec_msg_deal_api(&aux_rec, msg);
	            break;
        }

#if SUPPORT_APP_RCSP_EN
		rcsp_linein_task_msg_deal_after(msg);
#endif
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 任务创建
 @param  priv：任务间参数传递指针
 @return 无
 @note   static void linein_task_init(void *priv)
 */
/*----------------------------------------------------------------------------*/
static void linein_task_init(void *priv)
{
    u32 err;
#if AB_D_EN
	ABD_HIGH();
#endif
    err = os_task_create(linein_task, (void*) 0, TaskLineinPrio, 10
#if OS_TIME_SLICE_EN > 0
            ,1
#endif
            , LINEIN_TASK_NAME);

    if (OS_NO_ERR == err)
    {
        key_msg_register(LINEIN_TASK_NAME, linein_ad_table, linein_io_table, linein_ir_table, NULL);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 任务删除退出
 @param  无
 @return 无
 @note   static void linein_task_exit(void)
 */
/*----------------------------------------------------------------------------*/
static void linein_task_exit(void)
{
    if (os_task_del_req(LINEIN_TASK_NAME) != OS_TASK_NOT_EXIST)
    {
        os_taskq_post_event(LINEIN_TASK_NAME, 1, SYS_EVENT_DEL_TASK);
        do
        {
            OSTimeDly(1);
        } while (os_task_del_req(LINEIN_TASK_NAME) != OS_TASK_NOT_EXIST);
        aux_puts("del_linein_task: succ\n");
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 任务信息
 @note   const struct task_info linein_task_info
 */
/*----------------------------------------------------------------------------*/
TASK_REGISTER(linein_task_info) =
{
    .name = LINEIN_TASK_NAME,
    .init = linein_task_init,
    .exit = linein_task_exit,
};



#if USE_SOFTWARE_EQ 
#define AUX_SOFT_EQ_BUF_SIZE  5*1024
#define AUX_EQ_TASK_NAME "AUX_SOFT_EQ"
typedef struct
{
		u8 *data_buf;
		cbuffer_t aux_ladc_cbuf;
		OS_SEM aux_eq_start;
		u8 run_flag;
}aux_soft_eq_var;
aux_soft_eq_var aux_soft_eq;
static void aux_eq_run(void *p)
{
	int msg[3];
	u8 read_buf[512];
	puts("aux_eq_run===\n");
	while(aux_soft_eq.run_flag == 0xaa)
	{	
		os_sem_pend(&aux_soft_eq.aux_eq_start , 0);
		if(cbuf_read(&aux_soft_eq.aux_ladc_cbuf ,read_buf ,512 )==512)
		{	
			dac_write(read_buf , 512);
		}
	}
	while(1)
	{	
		memset(msg , 0x0 ,sizeof(msg));
		os_taskq_pend(0 , ARRAY_SIZE(msg), msg);
		switch(msg[0])
		{
			case SYS_EVENT_DEL_TASK:
				puts("DEL AUX EQ TASK\n");
				if(os_task_del_req_name(OS_TASK_SELF) == OS_TASK_DEL_REQ)
				{
					if(aux_soft_eq.data_buf)
						free(aux_soft_eq.data_buf);
					aux_soft_eq.data_buf = NULL;
					os_task_del_res_name(OS_TASK_SELF);
				}
			default:
				break;
		}
	}
}
void aux_ladc_cbuf_write(u8 *data , u16 length)
{
	if(aux_soft_eq.run_flag != 0xaa)
		return;
	if(length != cbuf_write(&aux_soft_eq.aux_ladc_cbuf, data, length))
		putchar('&');
	os_sem_post(&aux_soft_eq.aux_eq_start);
}
static void aux_eq_task_init()
{
	u32 err;
	aux_soft_eq.data_buf = malloc(AUX_SOFT_EQ_BUF_SIZE);
	ASSERT(aux_soft_eq.data_buf);
	cbuf_init(&aux_soft_eq.aux_ladc_cbuf , aux_soft_eq.data_buf , AUX_SOFT_EQ_BUF_SIZE );
	os_sem_create(&aux_soft_eq.aux_eq_start, 0);
	aux_soft_eq.run_flag = 0xaa;
	err = os_task_create_ext(aux_eq_run,
			(void *)0,
			TaskLineinEqPrio,
			10
#if OS_TIME_SLICE_EN > 0
			,1
#endif
			, AUX_EQ_TASK_NAME
			,1024*4
			);

	if(OS_NO_ERR == err)
		puts("aux eq init ok\n");
}
static void aux_eq_task_stop()
{
	aux_soft_eq.run_flag = 0;
	os_sem_post(&aux_soft_eq.aux_eq_start);
	if( os_task_del_req(AUX_EQ_TASK_NAME)!= OS_TASK_NOT_EXIST)
	{
		os_taskq_post_event(AUX_EQ_TASK_NAME , 1, SYS_EVENT_DEL_TASK);
		do{
			OSTimeDly(1);
		}while(os_task_del_req(AUX_EQ_TASK_NAME)!= OS_TASK_NOT_EXIST);
	}
}
#endif
