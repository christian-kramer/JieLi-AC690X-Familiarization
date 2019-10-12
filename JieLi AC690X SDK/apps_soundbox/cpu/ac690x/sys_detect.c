#include "sdk_cfg.h"
#include "sys_detect.h"
#include "key_drv/key_drv_ad.h"
#include "rtos/task_manage.h"
#include "file_operate/file_op.h"
#include "key_drv/key.h"
#include "ui/led/led.h"
#include "dac/dac.h"
#include "key_drv/key_voice.h"
#include "linein/dev_linein.h"
#include "rtc/rtc.h"
#include "uart.h"
#include "dev_manage/drv_manage.h"
#include "common/common.h"

u8 soft_poweroff_cnt = 0;
u8 going_to_pwr_off = 0;
u8 bd_work_mode;
tu8 power_level;
/**********************2016-10-06**************************************************************/
PHONE_VAR g_phone_var; ///<phone in det  variate
/*----------------------------------------------------------------------------*/
PHONE_STATUS phone_check(u8 cnt)
{
	u16 cur_phone_status;
	cur_phone_status = PHONE_IN_CHECK; /////Get Current PHONE status
	if(cur_phone_status != g_phone_var.pre_status)
	{
		g_phone_var.pre_status = cur_phone_status;
		g_phone_var.status_cnt = 0;
	}
	else
	{
		g_phone_var.status_cnt++;
	}

	if((PHONE_ON == g_phone_var.bDevOnline) && g_phone_var.pre_status)
	{
		g_phone_var.bDevOnline = PHONE_OFF;
		return PHONE_OFF;
	}

	if(g_phone_var.status_cnt < cnt) //disappears shakes
	{
		return NULL_PHONE;
	}
	g_phone_var.status_cnt = 0;

    ///detected PHONE insert
	if((PHONE_OFF == g_phone_var.bDevOnline) && (!g_phone_var.pre_status))
	{
		g_phone_var.bDevOnline = PHONE_ON;
		return PHONE_ON;
	}
#ifndef AUX_OUT_WITHOUT_CNT
   ///detected PHONE pull out
	else if((PHONE_ON == g_phone_var.bDevOnline) && g_phone_var.pre_status)
	{
		g_phone_var.bDevOnline = PHONE_OFF;
		return PHONE_OFF;
	}
#endif
	return NULL_PHONE;
}
/*----------------------------------------------------------------------------*/
/**@brief  phone IN online det func
   @param  no
   @return onlin status
   @note   s32 aux_detect(void)
*/
/*----------------------------------------------------------------------------*/
s32 phone_detect(void)
{
	u32 dev_cur_sta = DEV_HOLD;
	PHONE_STATUS res;

	res = phone_check(20); ///phoneonlin det 锛宒isappears shakes cnt 50
	if(PHONE_OFF == res)
	{
		dev_cur_sta = DEV_OFFLINE;
	}
	else if(AUX_ON == res)
	{
		dev_cur_sta = DEV_ONLINE;
	}

	if(g_phone_var.last_sta == dev_cur_sta)
	{
		return DEV_HOLD;///device status remain unchanged
	}
	g_phone_var.last_sta = dev_cur_sta;
	return dev_cur_sta;
}
/*----------------------------------------------------------------------------*/
/**@brief PHONE IN online status
   @param  no
   @return PHONE IN online status
   @note   u32 aux_is_online(void)
*/
/*----------------------------------------------------------------------------*/
u32 phone_is_online(void)
{
	return g_phone_var.bDevOnline;
}
/*----------------------------------------------------------------------------*/
/**@brief  phone IN init func
   @param  no
   @return no
   @note   void aux_init_api(void)
*/
/*----------------------------------------------------------------------------*/
void phone_init_api(void)
{
#if	PHONE_CHECK_EN
	PHONE_DIR_SET;
	PHONE_PU_SET;
#endif
	my_memset((u8 *)&g_phone_var, 0, sizeof(PHONE_VAR));
}
/*----------------------------------------------------------------------------*/
/**@brief phone IN online listing
   @note   const u8 aux_event_tab[]
*/
/*----------------------------------------------------------------------------*/
const u8 phone_event_tab[] =
{
	SYS_EVENT_PHONE_OUT, //phone pull out
	SYS_EVENT_PHONE_IN,  //phone insert
};
/*----------------------------------------------------------------------------*/
/**@brief  phone IN online det API func
   @param  no
   @return no
   @note   void aux_check_api(void)
*/
/*----------------------------------------------------------------------------*/
void phone_check_api(u32 info)
{
	s32 dev_status;
	dev_status = phone_detect();
	if(dev_status != DEV_HOLD)
	{
		os_taskq_post_event("MainTask", 2, phone_event_tab[dev_status],info);
	}
}
/*----------------------------------------------------------------------------*/
void pa_mute(void)                                                                                                
{
#if PA_EN
     /* sys_det_puts("pa_mute\n"); */
	 MUTE();
 //  PORTR_DIR(PORTR2,0);   
//   PORTR_OUT(PORTR2,1);   
    /* JL_PORTA->DIR &= ~BIT(4); */ 
     /* JL_PORTA->OUT |= BIT(4); */  
#endif
}
void pa_umute(void)
{	
#if PA_EN
     /* sys_det_puts("pa_umute\n"); */
#if	PHONE_CHECK_EN
	if(g_phone_var.bDevOnline == PHONE_ON)
	{
		pa_mute();
	}
	else
#endif
	{
		UMUTE();
	}
 //  PORTR_DIR(PORTR2,0);
 //  PORTR_OUT(PORTR2,0);
    /* JL_PORTA->DIR &= ~BIT(4); */ 
     /* JL_PORTA->OUT &= ~BIT(4); */ 
#endif
}
void high_port(void)
{
	JL_PORTA->DIR |= BIT(3);
	JL_PORTA->PD &= ~BIT(3);
	JL_PORTA->PU &= ~BIT(3);
	
	JL_PORTA->DIR |= BIT(4);
	JL_PORTA->PD &= ~BIT(4);
	JL_PORTA->PU &= ~BIT(4);
	
	JL_PORTC->DIR |= BIT(3);
	JL_PORTC->PD &= ~BIT(3);
	JL_PORTC->PU &= ~BIT(3);
}
/*----------------------------------------------------------------------------*/
/**@brief
   @param  void
   @return void
   @note   void sys_init(void)
*/
/*----------------------------------------------------------------------------*/
void sys_init(void)
{

#if RTC_CLK_EN
    irtc_hw_init();
#endif // RTC_CLK_EN
#if UART_UPDATA_EN
   uart_update_init();
#endif
	usb_2_io();

	key_init();
	high_port();
    //led_init();
#if AB_D_EN
	ABD_IO_INIT();
#endif
	aux_init_api(); //AUX init
	phone_init_api();//phone init
}

/*----------------------------------------------------------------------------*/
/**@brief  软开关控制
   @param  ctl :power_on or power_off
   @return null
   @note   void soft_power_ctl(u8 ctl)
*/
/*----------------------------------------------------------------------------*/
void soft_power_ctl(u8 ctl)
{
#if 0   //soft power ctl by mos transistor
    if(PWR_ON == ctl)
    {
        led_init();
        JL_PORTA->DIR &= ~BIT(14);
        JL_PORTA->OUT |= BIT(14);
    }
    else
    {
        LVD_CON &= ~BIT(2);
        //LED_INIT_DIS();
        JL_PORTA->DIR &= ~BIT(14);
        JL_PORTA->OUT &= ~BIT(14);
    }
#else   //soft power ctl by power off

    if(PWR_ON == ctl)
    {

    }
    else
    {
		printf("soft off\n");
        soft_poweroff();
    }

#endif
}
/*----------------------------------------------------------------------------*/
/**@brief   power off 检测延时关机
   @param   null
   @return  null
   @note    void check_power_off(void)
*/
/*----------------------------------------------------------------------------*/
void check_power_off(void)
{
    if(soft_poweroff_cnt)
    {
        if(--soft_poweroff_cnt == 0)
        {
            soft_power_ctl(PWR_OFF);
        }
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   关闭IO长按复位功能
   @param   null
   @return  null
   @note    void clr_PINR_ctl(void)
*/
/*----------------------------------------------------------------------------*/
void clr_PINR_ctl(void)
{
    rtc_module_port_4s_reset(PORTR2 , 0 , 0 );
}

static u32 start_charge_cnt = 0;
void charge_detect(void)
{
#if CHARGE_POWER_OFF
	JL_PORTA->DIR |= BIT(3);
	JL_PORTA->PU  &= ~BIT(3);
	JL_PORTA->PD  |= BIT(3);

	if(JL_PORTA->IN & BIT(3))
	{
		start_charge_cnt ++;
		if(start_charge_cnt > 50)
		{
			going_to_pwr_off = 1;
			os_taskq_post("MainTask", 2, MSG_POWER_OFF_KEY_MSG ,0x44);
		}
	}
	else
	{
		start_charge_cnt = 0;
	}
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief  实时电量检测
   @param  void
   @return void
   @note   void battery_check(void *)
*/
/*----------------------------------------------------------------------------*/
#if SYS_LVD_EN

u8 charge_full_flag = 0;             //充满电标志
#define CHARGE_PROTECT_VOLTAGE 42    //保护电压
static u32 charge_full_cnt = 0;
static u32 repeat_charge_cnt = 0;
extern void set_pwrmd(u8 mode);
extern u32 get_fast_charge_sta(void);
u8 send_msg_flag = 0;
static u8 send_msg_cnt = 0;

#define POWER_CHECK_CNT   100 //POWER_CHECK_CNT*10ms
u16 unit_cnt = 0;             //计数单位时间POWER_CHECK_CNT*10ms
u16 low_warn_cnt = 0;         //单位时间内监测到报警电压次数
u16 low_off_cnt = 0;          //单位时间内监测到关机电压次数
static u8 charge_cnt = 0;     //充电检测计数
extern u8 is_pwrtimer_en();
static u8 low_voice_cnt = 0;
volatile u8 low_power_flag = 0;
void battery_check(void *prt)
{
    u16 val;
	u8 cnt;
    static u8 lastLevel;
	static u8 levelCnt;
	u8 leveltemp;
	charge_detect();

    val = get_battery_level();
    unit_cnt++;
	if(val<36)
	{
		leveltemp=0x00;
	}
	else if(val<39)
	{
		leveltemp=0x01;
	}
	else if(val<43)
	{
		leveltemp=0x02;
	}
	else
		leveltemp=0x03;
	if(lastLevel==leveltemp)
	{
		if(levelCnt<55)
		{
			levelCnt++;
		}
		if(levelCnt==50)
		{
			power_level=leveltemp;
		}
	}
	else
	{
		levelCnt=0x00;
		lastLevel=leveltemp;
	}
    if(val < 31)
        low_off_cnt++;
    if(val < 33)
        low_warn_cnt++;

    if(unit_cnt >= POWER_CHECK_CNT)
    {
        //printf("val:%d,low_off_cnt:%d,low_warn_cnt:%d\n",val,low_off_cnt,low_warn_cnt);
		if(is_sniff_mode())
			cnt = 4;
		else if(is_pwrtimer_en())
			cnt = 4;
		else
			cnt = 10;

        if(low_off_cnt > POWER_CHECK_CNT/2)//低电关机
        {
            puts("\n*******Low Power********\n");
			low_power_flag = 1;
            os_taskq_post(MAIN_TASK_NAME,1,MSG_POWER_OFF);
        }
        else if(low_warn_cnt > POWER_CHECK_CNT/2)//低电提醒
        {
			low_power_flag = 1;
			if(!going_to_pwr_off)
			{
				R_LED_ON();
				B_LED_OFF();
			}

			low_voice_cnt ++;
			if(low_voice_cnt > cnt)
			{
				/* puts("\n**Low Power,Please Charge Soon!!!**\n"); */
				low_voice_cnt = 0;
            	os_taskq_post(keymsg_task_name,1,MSG_LOW_POWER_VOICE);
			}
        }
		else
		{
			if(low_power_flag)
			{
				R_LED_OFF();
				B_LED_OFF();
			}
			low_power_flag = 0;
		}

        unit_cnt = 0;
        low_off_cnt = 0;
        low_warn_cnt = 0;
    }
}
#endif

/*----------------------------------------------------------------------------*/
/**@brief  恢复USB口为普通IO口
   @param  USB_IO_CON
   BIT(15)-BIT(8):  --   --   --   --  DMIN DPIN  --    --
   BIT(7) -BIT(0): DMPU DPPU DMPD DPPD DMIE DPIE DMOUT DPOUT
   @return void
   @note   void usb_2_io(void)

*/
/*----------------------------------------------------------------------------*/
void usb_2_io(void)
{
//    sys_det_puts("USB_2_IO\n");
    JL_USB->CON0 |= BIT(0)|BIT(1);//PHY_ON(enable)  IO_MODE(enable)
    JL_USB->IO_CON = (BIT(3)|BIT(2));//IO输入
}

/*----------------------------------------------------------------------------*/
/**@brief   watch dog timer config
   @param   void
   @return  void
   @note
 */
/*----------------------------------------------------------------------------*/
/*
cfg:1010:1s  1011:2s    1100:4s
    1101:8s  1110:16s   1111:32s
*/
void open_wdt(u8 cfg)//开看门狗
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    //puts("open_wdt\n");
    JL_CRC->REG  = 0X6EA5;
    JL_SYSTEM->WDT_CON  = cfg&0x0f;
    JL_SYSTEM->WDT_CON &= ~BIT(5);
    JL_SYSTEM->WDT_CON |=  BIT(4);
    JL_CRC->REG  = 0XAAAA;
    OS_EXIT_CRITICAL();
}

void close_wdt(void)//关看门狗
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    //puts("close_wdt\");
    JL_CRC->REG = 0x6EA5;
    JL_SYSTEM->WDT_CON &= ~BIT(4);
    JL_CRC->REG = 0;
    OS_EXIT_CRITICAL();
}

void clear_wdt(void)//清看门狗
{
    //puts(" clr_wdt ");
    JL_SYSTEM->WDT_CON |= BIT(6);
}

/*----------------------------------------------------------------------------*/
/**@brief  	sd data multiplex 
   @param   void
   @return  void
   @note
 */
/*----------------------------------------------------------------------------*/
#if SD_DADA_MULT
u16 adc_value_PA1;
u8 g_aux_online_flag;
extern u32 get_sd0_dev_busy_status_api(void);
extern void close_sd0_controller(void);
extern void release_sd0_controller(void);
void sd_data_multiplex(void)
{
    u16 res = 0;
    static u8 dev_online_cnt,dev_offline_cnt,adc_timeout_cnt,adc_err_cnt;

    //if(task_start_complete == 0)
        //return;

    if(!get_sd0_dev_busy_status_api())
    {
		adc_busy_flag = 1;

		//CPU_SR_ALLOC();
        //OS_ENTER_CRITICAL();

        //关闭 sd controller
        close_sd0_controller();

        //设置PA1 IO
        JL_PORTA->DIR |= BIT(1);
        JL_PORTA->HD &= ~BIT(1);
        JL_PORTA->PU |= BIT(1);
        JL_PORTA->PD &= ~BIT(1);
        JL_PORTA->DIE &= ~BIT(1);

        //启动ADC
		//adc_ch_sel(AD_KEY_PA1);
		res = adc_res_api(AD_KEY_PA1);
		res = adc_res_api(get_next_adc_ch());

        //恢复 sd io
        JL_PORTA->DIR &= ~BIT(1);
        JL_PORTA->HD |= BIT(1);
        JL_PORTA->PU |= BIT(1);
        JL_PORTA->DIE |= BIT(1); 

        //恢复 sd controller
        release_sd0_controller();
        //OS_EXIT_CRITICAL();
		adc_busy_flag = 0;
    }
    else 
	{
        putchar('B');
        return;
    }

    adc_value_PA1 = (res*33)/0x3ffL;
    if(++adc_timeout_cnt > 20)
    {
        //printf("online_cnt:%d     offline_cnt:%d       err_cnt:%d\n"   ,dev_online_cnt,dev_offline_cnt,adc_err_cnt);
        if(dev_online_cnt > 13) 
		{
            //puts("aux_online\n");
            g_aux_online_flag = 1;
        }
        else if(dev_offline_cnt > 13) 
		{
            //puts("*************aux_offline*************\n");
            g_aux_online_flag = 0;
        }
        dev_online_cnt = 0;
        adc_timeout_cnt = 0;
        dev_offline_cnt = 0;
        adc_err_cnt = 0;
    }

    if((adc_value_PA1>27)&&(adc_value_PA1<31)) 
	{
        dev_online_cnt++;
    }
    else if(adc_value_PA1 >= 31) 
	{
        dev_offline_cnt++;
    }
    else 
	{
        adc_err_cnt++;
        printf("\n========err======= %d   %d  \n",res ,adc_value_PA1);
    }
    //printf("adc_val:%d,online_cnt:%d,offline_cnt:%d\n",adc_value_PA1,dev_online_cnt,dev_offline_cnt);
    printf("[%d]",adc_value_PA1);
}
#endif //SD_DADA_MULT

/*----------------------------------------------------------------------------*/
/**@brief  	pwm4_cfg 
   @param	toggle:switch
  			pre_div:0-2-4-8-16-32-64-128(div = 0~7) 
			duty:duty/16(dety = 0~15)
   @return  void
   @note    pwm4 clk = lsb_clk/16div(default:48M/16 = 3M)
 */
/*----------------------------------------------------------------------------*/
void pwm4_cfg(u8 toggle,u8 pre_div,u8 duty)
{
	u8 pwm4_scaler;
	u8 pwm4_duty;
	if(toggle) 
	{
		pwm4_scaler = pre_div & 0x7;	
		pwm4_duty = duty & 0xF;
    	JL_PWM4->CON = (pwm4_scaler << 4) | pwm4_duty;
		JL_IOMAP->CON1 |= BIT(11)|BIT(12)|BIT(13);
    	//JL_PORTA->DIR &= ~BIT(2);JL_PORTA->PU |= BIT(2);JL_PORTA->PD |= BIT(2);
	}
 else 
	{
    	JL_PWM4->CON = 0;
		JL_IOMAP->CON1 &= ~(BIT(11)|BIT(12)|BIT(13));
	}
}



extern u32 get_sd0_dev_busy_status_api(void);
extern u32 get_sd1_dev_busy_status_api(void);
extern void close_sd0_controller(void);
extern void close_sd1_controller(void);
extern void release_sd0_controller(void);
extern void release_sd1_controller(void);
extern s32 sd0_mult_det_api(void);
extern s32 sd1_mult_det_api(void);
extern void usb_host_mult_det_api(void);

/* extern void resume_sd0_io(void); */
extern u8 get_io_status(void);


static void suspend_sd_io(void)
{
#if USB_SD0_MULT_EN	//sd0a
	JL_PORTA->DIR|= (BIT(11) | BIT(12));
	JL_PORTA->PU &=~(BIT(11) | BIT(12));
	JL_PORTA->PD &=~(BIT(11) | BIT(12));
#else				//sd1b
	JL_PORTC->DIR|= (BIT(3) | BIT(4));
	JL_PORTC->PU &=~(BIT(3) | BIT(4));
	JL_PORTC->PD &=~(BIT(3) | BIT(4));
#endif
}

void usb_sd_detect_mult_api(void)
{
	u32 sd_online;
	u32 usb_online;

#if USB_SD0_MULT_EN
	sd0_mult_det_api();
	sd0->dev_io->ioctrl(&sd_online, DEV_GET_STATUS);
	if(sd_online == 0)//sd_offline
	{
		suspend_sd_io();
		usb_host_mult_det_api();
	}
	else//sd_online
	{
		if(get_io_status() == IO2SD0)//sd_is_using
		{
			if(!get_sd0_dev_busy_status_api())//free
			{
				//关闭 sd controller
				close_sd0_controller();

				delay(100);//wait io stably
				usb_host_mult_det_api();

				//恢复 sd controller
				release_sd0_controller();
			}
		}
		else
		{
			usb_host_mult_det_api();
		}
	}

#endif

#if USB_SD1_MULT_EN
	sd1_mult_det_api();
	sd1->dev_io->ioctrl(&sd_online, DEV_GET_STATUS);
	if(sd_online == 0)//sd_offline
	{
		suspend_sd_io();
		usb_host_mult_det_api();
	}
	else//sd_online
	{
		if(get_io_status() == IO2SD0)//sd_is_using
		{
			if(!get_sd1_dev_busy_status_api())//free
			{
				//关闭 sd controller
				close_sd1_controller();

				delay(100);//wait io stably
				usb_host_mult_det_api();

				//恢复 sd controller
				release_sd1_controller();
			}
		}
		else
		{
			usb_host_mult_det_api();
		}
	}
#endif
}

