#include "common/app_cfg.h"
#include "rtos/os_api.h"
#include "rtos/os_cfg.h"
#include "common/msg.h"
#include "rtos/task_manage.h"
#include "usb_device.h"
#include "usb_key.h"
#include "dac/dac.h"
#include "dac/dac_api.h"
#include "play_sel/play_sel.h"
#include "dev_pc.h"
#include "dev_manage/dev_ctl.h"
#include "pc_prompt.h"
#include "sys_detect.h"
#include "pc_ui.h"
#include "key_drv/key.h"
#include "rcsp/rcsp_interface.h"

///extern void *malloc_fun(void *ptr,u32 len,char *info);
///extern void free_fun(void **ptr);

u8 usb_spk_vol_value;

//extern volatile u32 dac_energy_value;
extern u32 usb_device_unmount(void);
extern DAC_CTL dac_ctl;


#if USB_PC_EN

#undef 	NULL
#define NULL	0
/*----------------------------------------------------------------------------*/
/**@brief  PC读卡器 任务
   @param  p：任务间参数传递指针
   @return 无
   @note   static void pc_card_task(void *p)
*/
/*----------------------------------------------------------------------------*/
static void pc_card_task(void *p)
{
    int msg[6];

    pc_puts("--------pc_card_task-----\n");
    while(1)
    {
        memset(msg,0x00,sizeof(msg));
        os_taskq_pend(0, ARRAY_SIZE(msg), msg);

        switch(msg[0])
        {
        case SYS_EVENT_DEL_TASK:
            pc_puts("PCCARD_SYS_EVENT_DEL_TASK\n");
            if (os_task_del_req_name(OS_TASK_SELF) == OS_TASK_DEL_REQ)
            {
                os_task_del_res_name(OS_TASK_SELF);
            }
            break;
        }
        while(1)
        {
            if(0 == app_usb_slave_online_status())
            {
                pc_puts("pc_tast_offline\n");
                break; //PC离线
            }
            if(0 == ((card_reader_status *)msg[0])->working)
            {
                pc_puts("pc_card_notworking\n");
                break;
            }
            ((card_reader_status *)msg[0])->busy = 1;
            app_usb_slave_card_reader(MSG_PC_UPDATA); //读卡器流程
        }
        ((card_reader_status *)msg[0])->busy = 0;
    }
}


/*----------------------------------------------------------------------------*/
/**@brief  PC读卡器 任务创建
   @param  priv：任务间参数传递指针
   @return 无
   @note   static void pc_card_task_init(void *priv)
*/
/*----------------------------------------------------------------------------*/
static void pc_card_task_init(void *priv)
{

    os_task_create(pc_card_task,
                   (void*)0,
                   TaskPcCardPrio,
                   10
#if OS_TIME_SLICE_EN > 0
                   ,1
#endif
                   ,PC_CARD_TASK_NAME);

}


/*----------------------------------------------------------------------------*/
/**@brief  PC读卡器 任务删除退出
   @param  无
   @return 无
   @note   static void pc_card_task_exit(void)
*/
/*----------------------------------------------------------------------------*/
static void pc_card_task_exit(void)
{
    if (os_task_del_req(PC_CARD_TASK_NAME) != OS_TASK_NOT_EXIST)
    {
        puts("send pc SYS_EVENT_DEL_TASK\n");
        os_taskq_post_event(PC_CARD_TASK_NAME, 1, SYS_EVENT_DEL_TASK);
        do
        {
            OSTimeDly(1);
            putchar('D');
        }
        while(os_task_del_req(PC_CARD_TASK_NAME) != OS_TASK_NOT_EXIST);
        pc_puts("del_pc_card_task: succ\n");
    }
}


/*----------------------------------------------------------------------------*/
/**@brief  PC 任务资源释放
   @param  susb_var：PC模式申请的变量资源指针
   @return 无
   @note   void close_usb_slave(card_reader_status *card_status)
*/
/*----------------------------------------------------------------------------*/
static void close_usb_slave(card_reader_status *card_status)
{
	card_status->working = 0;
	while(card_status->busy)
	{
		pc_puts("-busy-");
		os_time_dly(5);
	}
	app_usb_slave_close(); //关闭usb slave模块
    pc_card_task_exit(); //删除读卡器任务
}




/*----------------------------------------------------------------------------*/
/**@brief  PC 任务
   @param  p：任务间参数传递指针
   @return 无
   @note   static void pc_task(void *p)
*/
/*----------------------------------------------------------------------------*/
u8 sys_mute_flag = 0;
static void pc_task(void *p)
{
    int msg[6];
    u32 pc_mute_status = 0;
	static card_reader_status card_status;
    u8 sync_pc_vol_flag = 0;

    pc_puts("\n************************PC TASK********************\n");
	usb_spk_vol_value = dac_ctl.sys_vol_l;
	memset(&card_status, 0x00, sizeof(card_reader_status));
    tone_play_by_name(PC_TASK_NAME, 1,BPF_PC_MP3);

	/* os_time_dly(300); */

    pc_prompt_playing = 0;
    //led_fre_set(15,0);

	sys_mute_flag = is_dac_mute();

    while(1)
    {
        memset(msg,0x00,sizeof(msg));
        os_taskq_pend(0, ARRAY_SIZE(msg), msg);

        if(msg[0] != MSG_HALF_SECOND)
        {
            printf("\nudisk_msg= 0x%x\n",msg[0]);
        }

        clear_wdt();


#if SUPPORT_APP_RCSP_EN
		rcsp_pc_task_msg_deal_before(msg);	
#endif
		switch(msg[0])
        {
        case SYS_EVENT_DEL_TASK:
            pc_puts("PC_SYS_EVENT_DEL_TASK 1\n");

            if(pc_mute_status && !sys_mute_flag) 
           		pc_dac_mute(0, FADE_ON);

            if (os_task_del_req_name(OS_TASK_SELF) == OS_TASK_DEL_REQ)
            {
                ///UI_menu(MENU_WAIT);
            pc_puts("PC_SYS_EVENT_DEL_TASK 2\n");
                ui_close_pc();
            pc_puts("PC_SYS_EVENT_DEL_TASK3\n");
                play_sel_stop(); //关闭提示音
            pc_puts("PC_SYS_EVENT_DEL_TASK4\n");
                close_usb_slave(&card_status);
            pc_puts("PC_SYS_EVENT_DEL_TASK5\n");
                os_task_del_res_name(OS_TASK_SELF);
            }

            sync_pc_vol_flag  = 0;
            break;

        case SYS_EVENT_PLAY_SEL_END: //提示音结束
            puts("PC_SYS_EVENT_PLAY_SEL_END\n");
            play_sel_stop(); //关闭提示音

            puts("PC_SYS_EVENT_PLAY_SEL_END-----\n");
#ifdef PC_PROMPT_EN
            if(pc_prompt_playing)
            {
                pc_puts("PC_play_tone_end_1\n");
                pc_prompt_playing = 0;
                dac_set_samplerate(48000,0); //DAC采样率设置为48K
                pc_channel_on(); //开PC DAC模拟通道
                break;
            }
#endif
            sync_pc_vol_flag  = 6;
        case MSG_PC_MODE_INIT:
            puts("MSG_PC_MODE_INIT\n");
			if(NULL == app_usb_slave_init())
			{
				break;
			}
			card_status.busy = 1;
            ui_open_pc(NULL, 0);

        /*************** for CARD READER ******************/
        case MSG_PC_CARD_READER:
            pc_puts("MSG_PC_CARD_READER\n");
			card_status.working = 1;
            os_taskq_post_msg(PC_CARD_TASK_NAME, 1, &card_status);
            break;

        /*************** for AUDIO ******************/
        case MSG_PC_SPK_MUTE:
            pc_puts("MSG_PC_SPK_MUTE\n");
            pc_mute_status = 1;
            pc_dac_mute(1, FADE_ON);
            UI_menu(MENU_REFRESH);
            break;

        case MSG_PC_SPK_UNMUTE:
            pc_puts("MSG_PC_SPK_UNMUTE\n");
            pc_mute_status = 0;
            pc_dac_mute(0, FADE_ON);
            UI_menu(MENU_REFRESH);

        case MSG_PC_SPK_VOL:
//            puts("MSG_PC_SPK_VOL\n");
            usb_spk_vol_value = app_pc_set_speaker_vol(pc_mute_status);
            puts("pc_vol\n");
			if(sync_pc_vol_flag != 1)
			{
               sync_pc_vol_flag = 1;
			}
			break;

        /*************** for HID KEY ******************/
        case MSG_PC_MUTE:
            pc_puts("p_m\n");
            app_usb_slave_hid(USB_AUDIO_MUTE);
			/* if(pc_mute_status == 0)
			{
    os_taskq_post_msg(PC_TASK_NAME, 1, MSG_PC_SPK_MUTE);
			}
			else
			{
    os_taskq_post_msg(PC_TASK_NAME, 1, MSG_PC_SPK_MUTE);
			} */
            break;

        case MSG_PC_VOL_DOWN:
            pc_puts("vd\n");
            app_usb_slave_hid(USB_AUDIO_VOLDOWN);
            ///UI_menu(MENU_PC_VOL_DOWN);
            break;

        case MSG_PC_VOL_UP:
            pc_puts("vu\n");
            app_usb_slave_hid(USB_AUDIO_VOLUP);
            ///UI_menu(MENU_PC_VOL_UP);
            break;

        case MSG_PC_PP:
            pc_puts("pp\n");
            app_usb_slave_hid(USB_AUDIO_PP);
            break;

        case MSG_PC_PLAY_NEXT:
            pc_puts("ne\n");
            app_usb_slave_hid(USB_AUDIO_NEXTFILE);
            break;

        case MSG_PC_PLAY_PREV:
            pc_puts("pr\n");
            app_usb_slave_hid(USB_AUDIO_PREFILE);
            break;

        case MSG_LOW_POWER:
            puts("pc_low_power\n");
            ///pc_prompt_play(BPF_LOW_POWER_MP3);
            break;

        /*************** for PC UPDATA ******************/
        case MSG_PC_UPDATA:
            pc_puts("MSG_PC_UPDATA\n");
            dev_updata_mode(usb_device_unmount, PC_UPDATA); //进入PC升级模式
            break;

        case MSG_HALF_SECOND:
//            pc_puts(" PC_H ");

            /* printf("eng = %d\n",get_dac_energy_value()); */
            if(sync_pc_vol_flag)
            {
                if(sync_pc_vol_flag > 2)
                {
                    sync_pc_vol_flag--;
                }
                else if(sync_pc_vol_flag == 2)
                {
                    puts("get sync_vol\n");
                    app_usb_slave_hid(USB_AUDIO_VOLUP);
                    sync_pc_vol_flag = 4;
                }
				else
				{
					;
				}

            }
            break;

        default:
            //pc_puts("pc_default\n");
            break;
        }

#if SUPPORT_APP_RCSP_EN
		rcsp_pc_task_msg_deal_after(msg);	
#endif

	}
}


/*----------------------------------------------------------------------------*/
/**@brief  PC任务和PC读卡器任务创建
   @param  priv：任务间参数传递指针
   @return 无
   @note   static void pc_task_init(void *priv)
*/
/*----------------------------------------------------------------------------*/
static void pc_task_init(void *priv)
{
    u32 err;

    //初始化PC任务
    err = os_task_create(pc_task,
                         (void*)0,
                         TaskPcPrio,
                         10
#if OS_TIME_SLICE_EN > 0
                         ,1
#endif
                         ,PC_TASK_NAME);

    //初始化读卡器任务
    pc_card_task_init(NULL);

    //按键映射
    if(OS_NO_ERR == err)
    {
        key_msg_register(PC_TASK_NAME, adkey_msg_usb_table, iokey_msg_usb_table, irff00_msg_usb_table, NULL);
    }
}


/*----------------------------------------------------------------------------*/
/**@brief  PC 任务删除退出
   @param  无
   @return 无
   @note   static void pc_task_exit(void)
*/
/*----------------------------------------------------------------------------*/
static void pc_task_exit(void)
{
    if (os_task_del_req(PC_TASK_NAME) != OS_TASK_NOT_EXIST)
    {
        os_taskq_post_event(PC_TASK_NAME, 1, SYS_EVENT_DEL_TASK);
        do
        {
            OSTimeDly(1);
        }
        while(os_task_del_req(PC_TASK_NAME) != OS_TASK_NOT_EXIST);
        pc_puts("del_pc_task: succ\n");
    }
}


/*----------------------------------------------------------------------------*/
/**@brief  PC 任务信息
   @note   const struct task_info pc_task_info
*/
/*----------------------------------------------------------------------------*/
TASK_REGISTER(pc_task_info) =
{
    .name = PC_TASK_NAME,
    .init = pc_task_init,
    .exit = pc_task_exit,
};

#undef 	NULL

#endif
