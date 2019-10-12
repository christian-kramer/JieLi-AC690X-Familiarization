#include "sdk_cfg.h"
#include "includes.h"
#include "main_key.h"
#include "rtos/os_api.h"
#include "common/app_cfg.h"
#include "dev_manage/dev_ctl.h"
#include "file_operate/file_op_err.h"
#include "rtos/task_manage.h"
#include "usb_device.h"
#include "key_drv/key.h"
#include "common/flash_cfg.h"
#include "vm/vm_api.h"
#include "ui/ui_api.h"
#include "dac/dac_api.h"
#include "linein.h"
#include "sys_detect.h"
#include "bluetooth/avctp_user.h"
#include "key_drv/key_drv_io.h"
#include "ui/led/led.h"
#include "key_drv/key_voice.h"
#include "dac/dac.h"
#include "rtc/rtc_api.h"
#include "update.h"
#include "clock.h"
#include "irq_api.h"
#include "dev_linein.h"
#include "fm/fm_api.h"
#include "rcsp/rcsp_interface.h"
#include "music_ui.h"
#include "music.h"

#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD 语音区分 start//////////////////////////////////
#include "file_operate\file_op.h"
//////////////////////USB SD语音区分 end//////////////////////////////////
#endif

enum
{
	WAKEUP_0 = 0,
	WAKEUP_1,
	WAKEUP_2,
	WAKEUP_3,
	WAKEUP_4,
	WAKEUP_5,
	WAKEUP_6,
	WAKEUP_7,
	WAKEUP_8,
	WAKEUP_9,
	WAKEUP_10,
	WAKEUP_11,
	WAKEUP_12,
	WAKEUP_13,
	WAKEUP_14,
	WAKEUP_15,
};

#define WAKEUP_UP_EDGE     0x0f
#define WAKEUP_DOWN_EDGE   0xf0

extern void music_enter_updata(void);
extern tbool mutex_resource_apply(char *resource,int prio ,void (*apply_response)(), void (*release_request)());
extern tbool mutex_resource_release(char *resource);
extern void resourse_manage_init(void);
extern void user_ctrl_prompt_tone_play(u8 status,void *ptr);
extern tbool play_sel_busy(void);

extern u32 lg_dev_status_chg(u32 status,void *parm,u32 lgdevnum);
extern u32 lg_dev_mount(void *parm,u8 first_let,u32 total_lgdev);
extern u8 *dev_get_phydev_name(void *drv_it);

extern volatile u8 low_power_flag;
extern void set_poweroff_wakeup_io_handle_register(void (*handle)(),void (*sleep_io_handle)(),void (*sleep_handle)());
extern bool device_is_first_start();
extern volatile u8 new_lg_dev_let;

extern void ble_init_config(void);
extern void light_init(void);
extern void fm_radio_powerdown(void);
extern void enter_sleep_mode(void);
/**********************************************************************/
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD 语音区分 start//////////////////////////////////
extern u32 curr_device_using;
extern bool dec_dev_changed;
//////////////////////USB SD 语音区分 end/////////////////////////////////
#endif
#ifdef SD0_IIC_MULT_EN
extern void close_sd0_controller(void);
extern void release_sd0_controller(void);
#endif
/************************2016-11-03**********************************************/
extern u32 file_operate_get_total_lgdev();
extern u32 file_operate_get_total_phydev();

extern u8 power_off_flag;
extern PHONE_VAR g_phone_var; ///<phone in det  variate
extern AUX_VAR g_aux_var; ///<line in检测变量
extern u8 power_voice_flag;
extern bool g_device_in_flag;
u8 power_on_flag;
#ifdef MEMERY_MODE
u8 curr_work_mode;
#endif
u8 key_short_cnt=0;
/*********************************************************************************/
extern void set_vocal_flag(u8 sw);
extern u8 get_vocal_flag(void);


void set_poweroff_wakeup_io()
{
	u8 wakeup_io_en = 0;
	u8 wakeup_edge = 0;

	//BIT(0)  PR0 : 0 disable  1 enable	
	//BIT(1)  PR1 : 0 disable  1 enable	
	//BIT(2)  PR2 : 0 disable  1 enable	
	//BIT(3)  PR3 : 0 disable  1 enable	
	/* wakeup_io_en |= WAKE_UP_PR0 | WAKE_UP_PR1 | WAKE_UP_PR2 | WAKE_UP_PR3; */
	wakeup_io_en |=  WAKE_UP_PR2;

	//BIT(4)  PR0 : 0 rising dege  1 failling edge
	//BIT(5)  PR1 : 0 rising dege  1 failling edge 	
	//BIT(6)  PR2 : 0 rising dege  1 failling edge 
	//BIT(7)  PR3 : 0 rising dege  1 failling edge 
	/* wakeup_edge |= EDGE_PR0 | EDGE_PR1 | EDGE_PR2 | EDGE_PR3;     //failling edge */
	/* wakeup_edge &= ~(EDGE_PR0 | EDGE_PR1 | EDGE_PR2 | EDGE_PR3);  //rising dege */
	wakeup_edge |= EDGE_PR2;     //failling edge

	soft_poweroff_wakeup_io(wakeup_io_en , wakeup_edge); 
}

/*enter sleep mode wakeup IO setting*/
void enter_sleep_mode_set(u16 wakeup_cfg , u8 wakeup_edge)
{	
	close_wdt();

	dac_off_control(); //close dac mudule


    JL_AUDIO->LADC_CON = 0;
    JL_AUDIO->ADA_CON0 = 0;
    JL_AUDIO->ADA_CON1 = 0;
    JL_AUDIO->ADA_CON2 = 0;

	JL_AUDIO->DAA_CON0 = 0;
	JL_AUDIO->DAA_CON1 = 0;
	JL_AUDIO->DAA_CON2 = 0;
	JL_AUDIO->DAA_CON3 = 0;
	JL_AUDIO->DAA_CON4 = 0;
	JL_AUDIO->DAA_CON5 = 0;

	JL_WAKEUP->CON0 = 0;      //wakeup enbale
	JL_WAKEUP->CON1 = 0;      //wakeup edge
	JL_WAKEUP->CON2 = 0xffff; //wakeup pending(clear)

	switch(wakeup_cfg)
	{
		case WAKEUP_0:
			break;

		case WAKEUP_1:
			break;

		case WAKEUP_2:
			printf("WAKEUP_2\n");
			JL_WAKEUP->CON2 |= BIT(2);
			JL_PORTA->DIR |= BIT(8);
			JL_PORTA->DIE |= BIT(8);
			if(wakeup_edge == WAKEUP_DOWN_EDGE)
			{
				printf("WAKEUP_DOWN_EDGE\n");
				JL_PORTA->PU |= BIT(8);
				JL_PORTA->PD &= ~BIT(8);
				JL_WAKEUP->CON1 |= BIT(2);
			}
			else
			{
				printf("WAKEUP_UP_EDGE\n");
				JL_PORTA->PU &= ~BIT(8);
				JL_PORTA->PD |= BIT(8);
				JL_WAKEUP->CON1 &= ~BIT(2);
			}
			JL_WAKEUP->CON0 |= BIT(2);
			break;

		case WAKEUP_3:
			printf("WAKEUP_3\n");
			JL_WAKEUP->CON2 |= BIT(3);
			JL_PORTA->DIR |= BIT(10);
			JL_PORTA->DIE |= BIT(10);
			if(wakeup_edge == WAKEUP_DOWN_EDGE)
			{
				printf("WAKEUP_DOWN_EDGE\n");
				JL_PORTA->PU |= BIT(10);
				JL_PORTA->PD &= ~BIT(10);
				JL_WAKEUP->CON1 |= BIT(3);
			}
			else
			{
				printf("WAKEUP_UP_EDGE\n");
				JL_PORTA->PU &= ~BIT(10);
				JL_PORTA->PD |= BIT(10);
				JL_WAKEUP->CON1 &= ~BIT(3);
			}
			JL_WAKEUP->CON0 |= BIT(3);
			break;

		case WAKEUP_4:
			printf("WAKEUP_4\n");
			JL_WAKEUP->CON2 |= BIT(4);
			JL_PORTA->DIR |= BIT(14);
			JL_PORTA->DIE |= BIT(14);
			if(wakeup_edge == WAKEUP_DOWN_EDGE)
			{
				printf("WAKEUP_DOWN_EDGE\n");
				JL_PORTA->PU |= BIT(14);
				JL_PORTA->PD &= ~BIT(14);
				JL_WAKEUP->CON1 |= BIT(4);
			}
			else
			{
				printf("WAKEUP_UP_EDGE\n");
				JL_PORTA->PU &= ~BIT(14);
				JL_PORTA->PD |= BIT(14);
				JL_WAKEUP->CON1 &= ~BIT(4);
			}
			JL_WAKEUP->CON0 |= BIT(4);
			break;
		case WAKEUP_5:
			break;
		case WAKEUP_6:
			break;
		case WAKEUP_7:
			break;
		case WAKEUP_8:
			printf("WAKEUP_8\n");
			JL_WAKEUP->CON2 |= BIT(8);
			JL_PORTA->DIR |= BIT(3);
			JL_PORTA->DIE |= BIT(3);
			if(wakeup_edge == WAKEUP_DOWN_EDGE)
			{
				printf("WAKEUP_DOWN_EDGE\n");
				JL_PORTA->PU |= BIT(3);
				JL_PORTA->PD &= ~BIT(3);
				JL_WAKEUP->CON1 |= BIT(8);
			}
			else
			{
				printf("WAKEUP_UP_EDGE\n");
				JL_PORTA->PU &= ~BIT(3);
				JL_PORTA->PD |= BIT(3);
				JL_WAKEUP->CON1 &= ~BIT(8);
			}
			JL_WAKEUP->CON0 |= BIT(8);
			break;
		case WAKEUP_9:
			printf("WAKEUP_9\n");
			JL_WAKEUP->CON2 |= BIT(9);
			JL_PORTA->DIR |= BIT(5);
			JL_PORTA->DIE |= BIT(5);
			if(wakeup_edge == WAKEUP_DOWN_EDGE)
			{
				printf("WAKEUP_DOWN_EDGE\n");
				JL_PORTA->PU |= BIT(5);
				JL_PORTA->PD &= ~BIT(5);
				JL_WAKEUP->CON1 |= BIT(9);
			}
			else
			{
				printf("WAKEUP_UP_EDGE\n");
				JL_PORTA->PU &= ~BIT(5);
				JL_PORTA->PD |= BIT(5);
				JL_WAKEUP->CON1 &= ~BIT(9);
			}
			JL_WAKEUP->CON0 |= BIT(9);
			break;
		case WAKEUP_10:
			printf("WAKEUP_10\n");
			JL_WAKEUP->CON2 |= BIT(10);
			JL_PORTB->DIR |= BIT(0);
			JL_PORTB->DIE |= BIT(0);
			if(wakeup_edge == WAKEUP_DOWN_EDGE)
			{
				printf("WAKEUP_DOWN_EDGE\n");
				JL_PORTB->PU |= BIT(0);
				JL_PORTB->PD &= ~BIT(0);
				JL_WAKEUP->CON1 |= BIT(10);
			}
			else
			{
				printf("WAKEUP_UP_EDGE\n");
				JL_PORTB->PU &= ~BIT(0);
				JL_PORTB->PD |= BIT(0);
				JL_WAKEUP->CON1 &= ~BIT(10);
			}
			JL_WAKEUP->CON0 |= BIT(10);
			break;
		case WAKEUP_11:
			printf("WAKEUP_11\n");
			JL_WAKEUP->CON2 |= BIT(11);
			JL_PORTB->DIR |= BIT(2);
			JL_PORTB->DIE |= BIT(2);
			if(wakeup_edge == WAKEUP_DOWN_EDGE)
			{
				printf("WAKEUP_DOWN_EDGE\n");
				JL_PORTB->PU |= BIT(2);
				JL_PORTB->PD &= ~BIT(2);
				JL_WAKEUP->CON1 |= BIT(11);
			}
			else
			{
				printf("WAKEUP_UP_EDGE\n");
				JL_PORTB->PU &= ~BIT(2);
				JL_PORTB->PD |= BIT(2);
				JL_WAKEUP->CON1 &= ~BIT(11);
			}
			JL_WAKEUP->CON0 |= BIT(11);
			break;
		case WAKEUP_12:
			printf("WAKEUP_12\n");
			JL_WAKEUP->CON2 |= BIT(12);
			JL_PORTB->DIR |= BIT(9);
			JL_PORTB->DIE |= BIT(9);
			if(wakeup_edge == WAKEUP_DOWN_EDGE)
			{
				printf("WAKEUP_DOWN_EDGE\n");
				JL_PORTB->PU |= BIT(9);
				JL_PORTB->PD &= ~BIT(9);
				JL_WAKEUP->CON1 |= BIT(12);
			}
			else
			{
				printf("WAKEUP_UP_EDGE\n");
				JL_PORTB->PU &= ~BIT(9);
				JL_PORTB->PD |= BIT(9);
				JL_WAKEUP->CON1 &= ~BIT(12);
			}
			JL_WAKEUP->CON0 |= BIT(12);
			break;
		case WAKEUP_13:
			printf("WAKEUP_13\n");
			JL_WAKEUP->CON2 |= BIT(13);
			JL_PORTB->DIR |= BIT(11);
			JL_PORTB->DIE |= BIT(11);
			if(wakeup_edge == WAKEUP_DOWN_EDGE)
			{
				printf("WAKEUP_DOWN_EDGE\n");
				JL_PORTB->PU |= BIT(11);
				JL_PORTB->PD &= ~BIT(11);
				JL_WAKEUP->CON1 |= BIT(13);
			}
			else
			{
				printf("WAKEUP_UP_EDGE\n");
				JL_PORTB->PU &= ~BIT(11);
				JL_PORTB->PD |= BIT(11);
				JL_WAKEUP->CON1 &= ~BIT(13);
			}
			JL_WAKEUP->CON0 |= BIT(13);
			break;
		case WAKEUP_14:
			break;
		case WAKEUP_15:
			break;

		default:
			return;	
	}
}

/*sleep mode wakeup io setting fuction*/
void set_sleep_mode_wakeup_io()
{
	enter_sleep_mode_set(WAKEUP_3,WAKEUP_DOWN_EDGE);
}

/*sleep mode before close IRQ callback fuction*/
void set_sleep_before_close_irq()
{
#if FM_RADIO_EN
    if(compare_task_name(FM_TASK_NAME))
	{
    	fm_radio_powerdown();
	}
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief  main task
   @param
   @return
   @note   task always running,can't delete

 */
/*----------------------------------------------------------------------------*/

extern void ram1_fast_run_standby(void);
extern void call_main_deal();
extern u32 test_power_off(u32 usec);
extern u32 pc_limh;
extern u32 pc_liml;
extern void delay(u32 d);

static u32 power_off_cnt = 0;

static void delay_us(u32 n)
{
    delay(n*(SYS_Hz/1000000L));
}


extern const u32 ad_table[]; 
extern void adc_init_api(u32 channel,u32 lsb_clk,u8 lvd_en); 
extern void adc_scan(void *param);
extern u16 get_battery_level(void);

static u8 low_power_cnt = 0;
static u32 normal_power_cnt = 0;

/* mode 1:power on     2:power off*/
void ldo5v_detect_deal(u8 mode) 
{
	static u32 delay_2ms_cnt = 0; 
	u8 val = 0;
	u8 tmp;


    return;
}
//message in this table can do when a phone call
//be careful to add a message
const int check_msg_table[] = 
{
	SYS_EVENT_LGDEV_ONLINE,
	SYS_EVENT_LGDEV_OFFLINE,
	MSG_VOL_DOWN,
	MSG_VOL_UP ,
	MSG_DAC_ON,
	MSG_DAC_OFF,
	MSG_MUTE   ,
	SYS_EVENT_TASK_RUN_REQ ,
	MSG_LOW_POWER,
    MSG_POWER_OFF_KEY_MSG,
    MSG_POWER_OFF,
    MSG_LOW_POWER_VOICE,
    MSG_ENTER_MENUMAIN,
    MSG_HALF_SECOND,
};
extern void printf_test(void);
static void TaskMain(void *p)
{
    int msg[6];
    u32 status;
    u32 bt_status=0;
    curr_task = NULL;
    soft_poweroff_cnt = 0;
    going_to_pwr_off = 0;
#if(BT_MODE!=NORMAL_MODE)
    bd_work_mode = BD_PAGE_MODE;
#else
    bd_work_mode = BD_CONN_MODE;
#endif
	puts("****TaskMain********\n");
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD 语音区分 start//////////////////////////////////
    power_on_flag = 1;
//////////////////////USB SD 语音区分 end//////////////////////////////////
#endif
	update_deal(UPDATE_OK_WHILE);

#if 0
	    #include "iic.h"
	    eeprom_verify();
		while(1); 
#endif


    /*soft_power_ctl*/
#if 0
  #if 0 //soft power ctl by mos transistor
    soft_power_ctl(PWR_OFF);
    while(1)
    {
        os_taskq_pend(0, ARRAY_SIZE(msg), msg);
        if(msg[0] == MSG_POWER_ON)
        {	
            puts("\n****************power_on***************\n");
            soft_power_ctl(PWR_ON);
            break;
        }
    }
  #else
	while(1)
	{	
		rtc_module_ldo5v_detect(1,0);
		soft_power_ctl(PWR_OFF);
	    memset(msg,0x00,sizeof(msg));
		os_taskq_pend(0, ARRAY_SIZE(msg), msg);
		if(msg[0] != 0xa9)
		{
			printf("key:%x\n",msg[0]);
		}
	    if(msg[0] == MSG_POWER_ON)
		{	 
			puts("****************power_on***************\n");
	        soft_poweroff_cnt = 0;
            soft_power_ctl(PWR_ON);
	    }
		else if(msg[0] == MSG_POWER_OFF)
		{	   
			puts("****************power_off***************\n");
		    soft_power_ctl(PWR_OFF);
		}
	}
#endif		
#endif		

	//SFR(JL_SYSTEM->LDO_CON,7,3,1);
    led_init();
    JL_SYSTEM->LVD_CON |= BIT(2);
#if DAC_AUTO_MUTE_EN
	digit_auto_mute_set(AUTO_MUTE_CFG, 4, 1200, 200);
#endif
    resourse_manage_init();
    vm_cache_start();
    audio_init();
    dev_ctrl_sys_init(0);

#if RCSP_LIGHT_HW
	light_init();
#endif

    while(1)
    {
        memset(msg,0x00,sizeof(msg));
        os_taskq_pend(0, ARRAY_SIZE(msg), msg);
        clear_wdt();

        if(msg[0] != MSG_HALF_SECOND)
        {
            printf("\nmain_msg= 0x%x\n",msg[0]);
        }
		bt_status = msg_mask_off_in_bt();//when there is a phone call,it can't exit bluetooth
		if(bt_status)
		{
			int i,flag=0;
			for(i = 0;i <( sizeof(check_msg_table)/sizeof(int)); i++)
			{
				if(check_msg_table[i]==msg[0])
				{ // check table
					flag=1;
					break;
				}
			}
			if(!flag)
			   continue;	
		}
        //printf("main_msg %08x %08x \n",msg[0],msg[1]);
#if SUPPORT_APP_RCSP_EN
		rcsp_main_task_msg_deal_before(msg);	
#endif
		switch(msg[0])
        {
        case SYS_EVENT_LGDEV_ONLINE:
#if FM_RADIO_EN
		#ifdef SD0_IIC_MULT_EN
						if(compare_task_name(FM_TASK_NAME))
						{
							pa_mute();
							close_sd0_controller();
							os_time_dly(1);
							fm_radio_powerdown();
							os_time_dly(1);
							release_sd0_controller();
							pa_umute();
						}
		#endif
#endif
            printf("LGDEV ONLINE--%08x-%08x\n",msg[1],msg[2]);
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD 语音区分 start//////////////////////////////////
			if((sd0==msg[1])||(usb==msg[1]))
            {
                curr_device_using=msg[1];
            }
//////////////////////USB SD 语音区分 end//////////////////////////////////
#endif
            status=lg_dev_status_chg(SYS_EVENT_LGDEV_ONLINE,(void *)msg[1],msg[2]);
            if((status!=FILE_OP_ERR_LGDEV_FULL)/*&&(status!='A')*/)
            {
                if(((char)status == 'A'))       ///< SPIFLASH璁惧mount
                {
                    puts("up\n");
                    lg_dev_mount((void *)msg[1],status,msg[2]);
                    puts("power up\n");

#if UI_ENABLE
                    ui_init_api();///必须挂载flash后才能初始化LCD显示
#endif

					puts("*************Select Task***************\n");
/**************************2016-11-03**************************************************/
#ifdef POWER_VOICE
					power_voice_flag=1;
					task_switch(RDEF_IDLE_TASK_NAME, 0, SWITCH_SPEC_TASK);
#else
#ifdef MEMERY_MODE
					vm_read_api(VM_MODE, &curr_work_mode);
                    os_time_dly(50);
					if(curr_work_mode&BIT(BLUE_MODE))
					{
						task_switch("BtStackTask", 0,SWITCH_SPEC_TASK);
					}
					else if(curr_work_mode&BIT(MUSIC_MODE))
					{
						if(file_operate_get_total_phydev())
							task_switch("MusicTask", 0,SWITCH_SPEC_TASK);
						else
							task_switch("BtStackTask", 0,SWITCH_SPEC_TASK);
					}
#if FM_RADIO_EN
					else if(curr_work_mode&BIT(FM_MODE))
					{
						task_switch("FMRadioTask", 0,SWITCH_SPEC_TASK);
					}
#endif
					else if(curr_work_mode&BIT(AUX_MODE))
					{
							task_switch("LineinTask", 0,SWITCH_SPEC_TASK);
					}
					else
					{
						if(file_operate_get_total_lgdev())
							task_switch("MusicTask", 0,SWITCH_SPEC_TASK);
						else
							task_switch("BtStackTask", 0,SWITCH_SPEC_TASK);
					}
#else
                    os_time_dly(100);
					if(file_operate_get_total_phydev())
					{
							task_switch(MUSIC_TASK_NAME, 0,SWITCH_SPEC_TASK);
					}
					else
					{
	           			task_switch(BTSTACK_TASK_NAME, 0, SWITCH_SPEC_TASK);
					}
#endif
#endif
/************************************************************************************/
                    //task_switch(LINEIN_TASK_NAME, 0, SWITCH_SPEC_TASK);
//                    task_switch(RTC_TASK_NAME, 0,SWITCH_SPEC_TASK);

                    os_taskq_post_event((char *)"CheckTask", 1, SYS_EVENT_LGDEV_ONLINE);
                    break;
                }
                else
                {
                    printf("\n---------new phydev name: %s ---------\n",dev_get_phydev_name((void *)msg[1]));
                    new_lg_dev_let=status;
                }
#ifdef POWERUP_DEV_IGNORE
                if(!msg[3])
                    break;
#endif

                if((os_time_get() > 200) && (!bt_status))
                {
                    if(compare_task_name(UDISK_TASK_NAME)) ///<PC Audio
                        break;
                    if((task_switch(MUSIC_TASK_NAME, 0,SWITCH_SPEC_TASK)==RUN_TASK_ERR_HAS_RPT))
                    {
                        printf("music has run...\n");
                        os_taskq_post_event((char *)MUSIC_TASK_NAME, 2, SYS_EVENT_DEV_ONLINE,status);
                    }
#ifndef MUSIC_USB_SD_VOICE
					else
					{
						g_device_in_flag = 1;
					}
#endif
                }
            }
            break;

        case SYS_EVENT_LGDEV_OFFLINE:
            printf("------dev offline ---\n");
            if(compare_task_name(MUSIC_TASK_NAME))
            {
				pa_mute();
            	os_time_dly(20);	///稍作延时，等待设备检测稳定
				if(get_phydev_total())//if(file_operate_get_total_lgdev())
				{
					puts("********111111*********\n");
					if(sd0 == curr_device_using)
					{
						puts("********BPF_USB_MP3*********\n");
						if ((music_ui.ui_curr_device != SD0_DEVICE))
		                	os_taskq_post_msg("MusicTask", 1, MSG_USB_MP3);
						else
							pa_umute();
							
							
					}
					else if(usb == curr_device_using)
					{
						puts("********BPF_SD_MP3*********\n");
						if (music_ui.ui_curr_device != USB_DEVICE)
		                	os_taskq_post_msg("MusicTask", 1, MSG_SD_MP3);
						else
							pa_umute();
					}
				}
				else
					task_switch(BTSTACK_TASK_NAME, 0, SWITCH_SPEC_TASK);
			}
			else
			{
				os_taskq_post_event((char *)keymsg_task_name, 2, SYS_EVENT_DEV_OFFLINE,msg[1]);
            	lg_dev_status_chg(SYS_EVENT_LGDEV_OFFLINE,(void *)msg[1],1);
			}
            break;

		case SYS_EVENT_AUX_IN:
			puts("SYS_EVENT_AUX_IN\n");
			if(os_time_get() > 200)
				task_switch(LINEIN_TASK_NAME,0,SWITCH_SPEC_TASK);
            break;

		case SYS_EVENT_AUX_OUT:
			puts("SYS_EVENT_AUX_OUT\n");
            if(compare_task_name(LINEIN_TASK_NAME))
        	{
				task_switch("BtStackTask", 0,SWITCH_SPEC_TASK);
        	}
			break;
/************************2016-11-03*************************************************/
		case SYS_EVENT_PHONE_IN:
			puts("SYS_EVENT_PHOME_IN\n");
			break;
		case SYS_EVENT_PHONE_OUT:
			puts("SYS_EVENT_PHOME_out\n");
			break;
/********************************************************************************/
        case SYS_EVENT_PC_IN:
            puts("SYS_EVENT_PC_IN\n");
			if(os_time_get() > 200)
            	task_switch(PC_TASK_NAME, 0, SWITCH_SPEC_TASK);
            break;

        case SYS_EVENT_PC_OUT:
            puts("SYS_EVENT_PC_OUT\n");
            if(compare_task_name(UDISK_TASK_NAME))
                task_switch(0, 0, SWITCH_NEXT_TASK);
            break;

#if RTC_ALM_EN
        case MSG_ALM_ON:
            puts("\n\n\n\n------MSG_ALM_ON\n");
            task_switch("RTCTask",0,SWITCH_SPEC_TASK);
//            Post_msg_to_rcsp_Task(MSG_RSCP_CHANGE_WORDMODE_ACTION,0,0);
            break;
#endif
        case MSG_CHANGE_BT:
			task_switch("BtStackTask", 0,SWITCH_SPEC_TASK);
			break;
        case MSG_CHANGE_WORKMODE:
            puts("MSG_CHANGE_WORKMODE\n");
		    if(get_call_status() != BT_CALL_HANGUP)
            {
				going_to_pwr_off = 0xFF;
                user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP,0,NULL);
				break;
            }
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD 语音区分 start//////////////////////////////////
			if(!strcmp(curr_task->name,"MusicTask"))
            {
                puts("M ODE\n");
                if((file_operate_get_total_phydev()>1) && (!dec_dev_changed))
                {
                    puts("MSGORKMODE\n");
                    os_taskq_post_msg("MusicTask", 1, MSG_MUSIC_U_SD);
                    break;
                }
                else
				{
                    task_switch(0,0,SWITCH_NEXT_TASK);
                }
            }
            else
			{
            	task_switch(0,0,SWITCH_NEXT_TASK);
			}
//////////////////////USB SD 语音区分 end//////////////////////////////////
#else
				task_switch(0,0,SWITCH_NEXT_TASK);
#endif
//            Post_msg_to_rcsp_Task(MSG_RSCP_CHANGE_WORDMODE_ACTION,0,0);
            break;
		case MSG_EXIT_IDLE_TASK:
			os_time_dly(50);
			if(file_operate_get_total_lgdev())
				task_switch("MusicTask", 0,SWITCH_SPEC_TASK);
			else
				task_switch("BtStackTask", 0,SWITCH_SPEC_TASK);
			break;
		case MSG_LAST_WORKMOD:
			puts("MSG_LAST_MODE\n");
			task_switch(0,0,SWITCH_LAST_TASK);
			break;
        case SYS_EVENT_TASK_RUN_REQ:
            task_switch((char *)msg[1], 0,SWITCH_SPEC_TASK);
//            Post_msg_to_rcsp_Task(MSG_RSCP_CHANGE_WORDMODE_ACTION,0,0);
            break;


        case SYS_EVENT_DEV_BEGIN_MOUNT:
			puts("SYS_EVENT_DEV_BEGIN_MOUNT\n");
            if(compare_task_name(MUSIC_TASK_NAME))
            {
                os_taskq_post_event((char *)MUSIC_TASK_NAME, 1, SYS_EVENT_DEV_BEGIN_MOUNT);
            }
            break;
		case SYS_EVENT_DEV_MOUNT_ERR:
			puts("SYS_EVENT_DEV_MOUNT_ERR\n");
            if(compare_task_name(MUSIC_TASK_NAME))
            {
                os_taskq_post_event((char *)MUSIC_TASK_NAME, 1, SYS_EVENT_DEV_MOUNT_ERR);
            }
			break;

        case MSG_VOL_DOWN:
			if(play_sel_busy())
				break;
			if(dac_ctl.sys_vol_r == 0)
	    	{
				dac_mute(0,1);
				pa_umute();
#if MAX_VOL_VOICE
				 if(key_short_cnt!=1)
				 {
				 	key_short_cnt = 1;
					 os_taskq_post_msg(keymsg_task_name,1,MSG_MAX_VOL);
				 }
#endif
	    	}
            if(dac_ctl.sys_vol_l)
                dac_ctl.sys_vol_l--;
#if BT_STEREO
            if(is_check_stereo_slave())
            {
                os_taskq_post("btmsg", 1, msg[0]);
            }
            else
            {
                user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_SEND_VOL,0,NULL);
                os_taskq_post("btmsg", 2,MSG_VOL_STEREO,dac_ctl.sys_vol_l);
            }
#else
			user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_SEND_VOL,0,NULL);
#endif


            printf("V- = %d\n",dac_ctl.sys_vol_l);
            dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
            if(is_dac_mute())
			{
                dac_mute(0,1);
            }
			//os_time_dly(10);
            set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
#if (SYS_DEFAULT_VOL==0)
            vm_cache_write(VM_SYS_VOL,&dac_ctl.sys_vol_l,2);
#endif
            UI_menu_arg(MENU_MAIN_VOL,0);
			
            break;

        case MSG_VOL_UP:
			if(play_sel_busy())
				break;
			if(msg[1]&0x80)
            {
				dac_ctl.sys_vol_l = (u8)(msg[1]&0x7f);
                printf("set_vol=  %d\n",dac_ctl.sys_vol_l);
                if(dac_ctl.sys_vol_l > MAX_SYS_VOL_L)
                dac_ctl.sys_vol_l = MAX_SYS_VOL_L;
            }
            else
            {
                if(dac_ctl.sys_vol_l < MAX_SYS_VOL_L)
                	dac_ctl.sys_vol_l++;

#if BT_STEREO
                if(is_check_stereo_slave())
                {
                    os_taskq_post("btmsg", 1, msg[0]);
                }
                else
                {
                    user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_SEND_VOL,0,NULL);
                    os_taskq_post("btmsg", 2,MSG_VOL_STEREO,dac_ctl.sys_vol_l );
                }
#else
				user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_SEND_VOL,0,NULL);
#endif
			}

#if KEY_TONE_EN
             if(dac_ctl.sys_vol_l == MAX_SYS_VOL_L)
			{
		#if MAX_VOL_VOICE
				 if(key_short_cnt!=1)
				 {
				 	key_short_cnt = 1;
					 os_taskq_post_msg(keymsg_task_name,1,MSG_MAX_VOL);
				 }
		#endif
               //  send_key_voice(500);
         	}
#endif
            printf("V+ = %d\n",dac_ctl.sys_vol_l);
            dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
            if(is_dac_mute())
			{
                dac_mute(0,1);
                //led_fre_set(15,0);
            }
			if(dac_ctl.sys_vol_l<2)
			{
				pa_umute();
                dac_mute(0,1);
			}
            set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
#if (SYS_DEFAULT_VOL==0)
            vm_cache_write(VM_SYS_VOL,&dac_ctl.sys_vol_l,2);
#endif
            UI_menu_arg(MENU_MAIN_VOL,0);
			
            break;
#if BT_STEREO
        case MSG_VOL_STEREO:
            dac_ctl.sys_vol_l = msg[1] ;
             printf("V = %d\n",dac_ctl.sys_vol_l);
			//printf("-------MSG_VOL_STEREO=%d\n",dac_var.cur_sys_vol_l );
            if(dac_ctl.sys_vol_l >= MAX_SYS_VOL_L)
                dac_ctl.sys_vol_l = MAX_SYS_VOL_L ;
#if KEY_TONE_EN
             if(dac_ctl.sys_vol_l == MAX_SYS_VOL_L)
                 send_key_voice(500);
#endif
            printf("V+ = %d\n",dac_ctl.sys_vol_l);
            dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
            if(is_dac_mute()) 
			{
                dac_mute(0,1);
                //led_fre_set(15,0);
            }
            set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
#if (SYS_DEFAULT_VOL==0)
            vm_cache_write(VM_SYS_VOL,&dac_ctl.sys_vol_l,2);
#endif
            UI_menu_arg(MENU_MAIN_VOL,0);

            break;
#endif
        case MSG_MUTE:
			puts("MSG_MUTE\n");
            if(is_dac_mute())
			{
                dac_mute(0,1);
            }
			else
			 {
                dac_mute(1,1);
            }
            UI_menu(MENU_REFRESH);
            break;
        
        case MSG_LOW_POWER:
            puts("**MSG_LOW_POWER,auto shutdown**\n");
            going_to_pwr_off = 0;
			soft_power_ctl(PWR_OFF);
			/* task_switch(IDLE_TASK_NAME,0,SWITCH_SPEC_TASK); */
            break;

        case MSG_LOW_POWER_VOICE:
	//send_key_voice(500);
			break;
#if 1
		case MSG_POWER_OFF:
		//	puts("MSG_POWER_OFF\n");
			power_voice_flag=0;
#ifdef SYS_POWER_KEY_EN
			power_off_flag=1;
			task_switch("IdleTask",0,SWITCH_SPEC_TASK);
#else
		//	soft_power_ctl(PWR_OFF);
			power_off_flag=1;
			task_switch("IdleTask",0,SWITCH_SPEC_TASK);

#endif
			break;
#else
        case MSG_POWER_OFF:
            puts("\n*************power_off******************\n");
    		if(keymsg_task_name)
   			{
                /* os_taskq_post_msg(keymsg_task_name, 1, MSG_PROMPT_PLAY); */
    		}

            if(get_call_status() != BT_CALL_HANGUP)
            {
				going_to_pwr_off = 0xFF;
                user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP,0,NULL);
				break;
            }
			else
			{
				/* putchar('a'); */
				if(going_to_pwr_off != 0xFF)
					going_to_pwr_off = 1;
			}

			if(!low_power_flag)
			{
				R_LED_OFF();
				B_LED_OFF();
			}

            os_taskq_post(MAIN_TASK_NAME,1,MSG_POWER_OFF_KEY_MSG);
            break;
        case MSG_POWER_OFF_HOLD:
            /* puts("MSG_POWER_OFF_HOLD\n"); */
			if(!low_power_flag)
			{
				R_LED_OFF();
				B_LED_OFF();
			}
            break;

		case MSG_POWER_KEY_UP:
			if(going_to_pwr_off == 0xFF)
			{
				going_to_pwr_off = 0;
			}
			break;

        case MSG_POWER_OFF_KEY_MSG:
            puts("MSG_POWER_OFF_KEY_MSG\n");

			if(going_to_pwr_off == 0xFF)
			{
				going_to_pwr_off = 0;
				break;
			}

            if((BT_STATUS_CONNECTING == get_bt_connect_status())   ||
               (BT_STATUS_TAKEING_PHONE == get_bt_connect_status())||
               (BT_STATUS_PLAYING_MUSIC == get_bt_connect_status()))
            {
                /* puts("bt_disconnect\n"); */
				user_send_cmd_prepare(USER_CTRL_POWER_OFF,0,NULL);
            }

			user_ctrl_prompt_tone_play(BT_STATUS_POWER_OFF,NULL);
			do
			{
				os_time_dly(1);
			}
			while(play_sel_busy());

			set_sys_vol(0,0,FADE_ON);
			dac_off_control();

			ldo5v_detect_deal(2);
            if(going_to_pwr_off)
			{
				soft_power_ctl(PWR_OFF);
				going_to_pwr_off = 0;
			}
            break;
#endif
		case MSG_DAC_ON:
			puts("main_dac_on\n");
			dac_mute(0,1);
			break;
		case MSG_DAC_OFF:
			puts("main_dac_off\n");
			dac_mute(1,1);
			break;

		case MSG_VOCAL_PP:
			if(get_vocal_flag()){
				set_vocal_flag(0);
			}
			else{
				set_vocal_flag(1);
			}
			break;

		case MSG_ENTER_SLEEP_MODE:
			puts("MSG_ENTER_SLEEP_MODE\n");
			enter_sleep_mode();
			break;

        case MSG_UPDATA:
			if(msg[1] == DEV_UPDATA)
			{
				music_enter_updata();
			}
			dev_updata_mode((void *)msg[2],msg[1]);
			break;

        case MSG_0:
        case MSG_1:
        case MSG_2:
        case MSG_3:
        case MSG_4:
        case MSG_5:
        case MSG_6:
        case MSG_7:
        case MSG_8:
        case MSG_9:

            input_number = input_number * 10 + msg[0];
            input_number_cnt++;
            if ((input_number > 9999) ||(input_number_cnt>4))
            {
                input_number = msg[0];
                input_number_cnt= 1;
            }
            printf("input_number = %u   \n",input_number);

#if UI_ENABLE
            UI_menu_arg(MENU_INPUT_NUMBER,input_number|(input_number_cnt<<24));
#endif
            break;

        case MSG_ENTER_MENUMAIN:
#if LCD_SUPPORT_MENU
			if(BT_STATUS_TAKEING_PHONE != get_bt_connect_status())//list_main forbiden when phone_call
			{
				UI_menu_arg(MENU_LIST_DISPLAY,UI_MENU_LIST_MAIN);
			}
#endif
            break;
		case SYS_EVENT_SYS_TIMER_DELAY:
			break;
        //case MSG_HALF_SECOND:
           // puts("half-sec \n");
            //break;
        }

#if SUPPORT_APP_RCSP_EN
		rcsp_main_task_msg_deal_after(msg);	
#endif

	}
}

/*----------------------------------------------------------------------------*/
/**@brief  创建main task
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
int main_task_init(void)
{
    u32 err;
    puts("create main task\n");
    err = os_task_create_ext(TaskMain,
                             (void*)0,
                             TaskMainPrio,
                             10
                             ,MAIN_TASK_NAME,
                             MAIN_TASK_STACK_SIZE
                            );

    if(OS_NO_ERR == err)
    {
        key_msg_register(MAIN_TASK_NAME, main_ad_table, main_io_table, main_ir_table, NULL);
    }
    else
    {
        printf("main task err = %08x \n",err);
    }

    return 0;
}
