#include "includes.h"
#include "key_drv/key_drv_ad.h"
#include "key_drv/key.h"
#include "clock_api.h"
#include "adc_api.h"
#include "timer.h"
#include "sys_detect.h"
#include "rtc/rtc_api.h"
#include "clock.h"
#include "sys_detect.h"
#include "dev_linein.h"

#define AD_KEY_IO_PAX   9 
u16 adc_value[3];
/******************zhgq20161103*************************/
u8 index_key=0x00;
#ifdef ADK_VOL_EN
bool vol_flag=0;
bool mutead_flag=0;
bool vol_show_en=0;
#endif
bool g_ad_led_flag;
extern PHONE_VAR g_phone_var; ///<phone in det  variate
extern AUX_VAR g_aux_var; ///<line in检测变量
/*******************************************************/
const u32 ad_table[] =
{
#if KEY_AD_EN
	AD_KEY_PB9,
#endif
    AD_KEY_LDOIN,
    AD_KEY_LDOREF,
};

#if 0
	#define ADC10_33   (0x3dfL)
	#define ADC10_30   (0x3dfL*30/33)
	#define ADC10_27   (0x3dfL*27/33)
	#define ADC10_23   (0x3dfL*23/33)
	#define ADC10_20   (0x3dfL*20/33)
	#define ADC10_17   (0x3dfL*17/33)
	#define ADC10_13   (0x3dfL*13/33)
	#define ADC10_10   (0x3dfL*10/33)
	#define ADC10_07   (0x3dfL*07/33)
	#define ADC10_04   (0x3dfL*04/33)
	#define ADC10_00   (0)

#else 
	#define R_UP       220     //22K

	#define ADC10_33   (0x3dfL)
	#define ADC10_30   (0x3dfL*2200/(2200 + R_UP))     //220K
	#define ADC10_27   (0x3dfL*1000/(1000 + R_UP))     //100K
	#define ADC10_23   (0x3dfL*510 /(510  + R_UP))     //51K
	#define ADC10_20   (0x3dfL*330 /(330  + R_UP))     //33K
	#define ADC10_17   (0x3dfL*240 /(240  + R_UP))     //24K
	#define ADC10_13   (0x3dfL*150 /(150  + R_UP))     //15K
	#define ADC10_10   (0x3dfL*91  /(91   + R_UP))     //9.1K
	#define ADC10_07   (0x3dfL*62  /(62   + R_UP))     //6.2K
	#define ADC10_04   (0x3dfL*30  /(30   + R_UP))     //3K
	#define ADC10_00   (0)
#endif

#define AD_NOKEY        ((ADC10_33 + ADC10_30)/2)
#define ADKEY1_0		((ADC10_30 + ADC10_27)/2)
#define ADKEY1_1		((ADC10_27 + ADC10_23)/2)
#define ADKEY1_2		((ADC10_23 + ADC10_20)/2)
#define ADKEY1_3		((ADC10_20 + ADC10_17)/2)
#define ADKEY1_4		((ADC10_17 + ADC10_13)/2)
#define ADKEY1_5		((ADC10_13 + ADC10_10)/2)
#define ADKEY1_6		((ADC10_10 + ADC10_07)/2)
#define ADKEY1_7		((ADC10_07 + ADC10_04)/2)
#define ADKEY1_8		((ADC10_04 + ADC10_00)/2)
#if JL_SDK
const u16 ad_key_table[] =
{
    ADKEY1_0,ADKEY1_1,ADKEY1_2,ADKEY1_3,ADKEY1_4,
    ADKEY1_5,ADKEY1_6,ADKEY1_7,ADKEY1_8
};
#else
#define FM_KEY    240//961//988//982
#define AUX_KEY   224//899///937//885
#define AUX_NOKEY  194//777//885
const u8 ADKey_Status_Border[4][9] =
{
/*    0R  3K  6.2K  9.1K  15K  24K    33K   51K    100K   220K*/
   {15, 43, 65, 91, 121, 143, 166, 194, 220, 244},  //独立按键
   {15, 43, 64, 87, 115, 140, 160, 187, 220, 244},   //fm	  online
   {15, 42, 63, 84, 110, 132, 156, 178, 218, 242},  //aux  online
   {15, 42, 63, 84, 110, 132, 151, 174, 218, 242},  //fm aux  online
};
#endif
/*----------------------------------------------------------------------------*/
/**@brief   ad按键初始化
   @param   void
   @param   void
   @return  void
   @note    void ad_key0_init(void)
*/
/*----------------------------------------------------------------------------*/
#define ADC_LVD_LDO5V	0
#define ADC_LVD_VBAT	1
void ad_key0_init(void)
{
    s32 ret;
    key_puts("ad key init\n");

#if KEY_AD_EN
	JL_PORTB->PD &= ~BIT(AD_KEY_IO_PAX);
	JL_PORTB->PU &= ~BIT(AD_KEY_IO_PAX);
	JL_PORTB->DIR |= BIT(AD_KEY_IO_PAX);
	JL_PORTB->DIE &= ~BIT(AD_KEY_IO_PAX);

	//PR2 AD key init
	/* clr_PINR_ctl(); */
	/* PORTR_PU(PORTR2,0); */
	/* PORTR_PD(PORTR2,0); */
	/* PORTR_DIR(PORTR2,1); */
	/* PORTR_DIE(PORTR2,0); */
	/* PORTR2_ADCEN_CTL(1); */

	//PR1 AD key init
	/* PORTR_PU(PORTR1,0); */
	/* PORTR_PD(PORTR1,0); */
	/* PORTR_DIR(PORTR1,1); */
	/* PORTR_DIE(PORTR1,0); */
	/* PORTR1_ADCEN_CTL(1); */
#endif

    adc_init_api(ad_table[0],clock_get_lsb_freq(),SYS_LVD_EN);

#if SYS_LVD_EN 
	//adc_lvd_ch_sel(ADC_LVD_LDO5V);


	ret = timer_reg_isr_fun(timer0_hl,5,(void *)battery_check,NULL);
	if(ret != TIMER_NO_ERR)
	{
		printf("battery_check err = %x\n",ret);
	}
#endif//SYS_LVD_EN

	ret = timer_reg_isr_fun(timer0_hl,1,adc_scan,NULL);
	if(ret != TIMER_NO_ERR)
	{
		printf("adc_scan err = %x\n",ret);
	}
}

void ad_key0_init_io()
{
#if KEY_AD_EN
	if(ad_table[0] == AD_KEY_PR1)
	{
		/* printf("AD_KEY_PR1\n"); */
		PORTR_PU(PORTR1,0);
		PORTR_PD(PORTR1,0);
		PORTR_DIR(PORTR1,1);
		PORTR_DIE(PORTR1,0);
		PORTR1_ADCEN_CTL(1);  //开PR1 ADC 功能
	}
	else if(ad_table[0] == AD_KEY_PR2)
	{
		/* printf("AD_KEY_PR2\n"); */
		PORTR_PU(PORTR2,0);
		PORTR_PD(PORTR2,0);
		PORTR_DIR(PORTR2,1);
		PORTR_DIE(PORTR2,0);
		PORTR2_ADCEN_CTL(1);  //开PR2 ADC 功能
	}
#endif
}
static int ad_key0_clk_reset(void)
{
    adc_init_api(ad_table[0],clock_get_lsb_freq(),SYS_LVD_EN);
    return 0;
}

static struct clock_switch clock_switch_key;

CLOCK_SWITCH_USER_REGISTER(CLOCK_SWITCH_EXIT, &clock_switch_key, ad_key0_clk_reset, "AD_KEY");

/*----------------------------------------------------------------------------*/
/**@brief   ad通道采样函数
   @param   void
   @param   void
   @return  void
   @note    void adc_scan(void)
*/
/*----------------------------------------------------------------------------*/
u8 next_channel;
volatile u8 adc_busy_flag = 0;
void adc_scan(void *param)
{
	if(adc_busy_flag)
		return;

    static u8 channel = 0;
	next_channel = channel + 1;
	if(next_channel == MAX_AD_CHANNEL)
		next_channel = 0;
#if 1
		if(next_channel==0)	//ad复用推led灯
		{
			g_ad_led_flag=1;
			JL_PORTB->PD &= ~BIT(AD_KEY_IO_PAX);
			JL_PORTB->PU &= ~BIT(AD_KEY_IO_PAX);
			JL_PORTB->DIR |= BIT(AD_KEY_IO_PAX);
			JL_PORTB->DIE &= ~BIT(AD_KEY_IO_PAX);
		}
		else
		{
			g_ad_led_flag=0;
			JL_PORTB->PD &= ~BIT(AD_KEY_IO_PAX);
			JL_PORTB->PU &= ~BIT(AD_KEY_IO_PAX);
			JL_PORTB->DIR &= ~BIT(AD_KEY_IO_PAX);
			JL_PORTB->DIE |= BIT(AD_KEY_IO_PAX);
		}
#endif
    adc_value[channel++] = adc_res_api(ad_table[next_channel]);

    if (channel == MAX_AD_CHANNEL)
        channel = 0;
}

u32 get_next_adc_ch(void)
{
	return ad_table[next_channel];
}
/*-----------------------------------zhgq 2016-11-08-----------------------------------------*/
#ifdef ADK_VOL_EN
void deal_ad_vol_change(u8 tmp)
{
	u8 volume_temp;
	static u8 cnt1=0,cnt2=0;

	if(tmp>dac_ctl.sys_vol_l*mainVOL_step)
	{
       cnt2 = 0;
		if((tmp - dac_ctl.sys_vol_l * mainVOL_step )> 3)
		{
				volume_temp = tmp /mainVOL_step;
				if(volume_temp > MAX_SYS_VOL_L)
					volume_temp =MAX_SYS_VOL_L;
				if(dac_ctl.sys_vol_l <  volume_temp )
				{
				   cnt1++;
				   if(cnt1>12)
				   {
				      cnt1 = 0;				
						 dac_ctl.sys_vol_l=volume_temp;
						 dac_ctl.sys_vol_r=volume_temp;
						 set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
						 vol_flag=1;
						 if(mutead_flag==1)
						 {
							 mutead_flag=0;
							 pa_umute();
						 }
						 if(vol_show_en) 
						 	UI_menu(MENU_MAIN_VOL);
				   	}
				}	
			    else
				cnt1 = 0;	
		}
		else
		{
			cnt1 = 0;
		}
	}
	else
	{
	   cnt1 = 0;
		if((dac_ctl.sys_vol_l * mainVOL_step - tmp )> 3)
		{
			volume_temp = tmp /mainVOL_step;
			 if(volume_temp > MAX_SYS_VOL_L)
			  volume_temp =MAX_SYS_VOL_L;
				if(dac_ctl.sys_vol_l > volume_temp )
				{	
				    cnt2++;
				    if(cnt2>12)
				   	{
					   	cnt2 = 0;				
						dac_ctl.sys_vol_l=volume_temp;
						dac_ctl.sys_vol_r=volume_temp;
						set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
						vol_flag=1;
						 if(dac_ctl.sys_vol_l==0)
						 {
							 mutead_flag=1;
							 pa_mute();
						 }
						if(vol_show_en) 
							UI_menu(MENU_MAIN_VOL);
				    }

				}
				else
				cnt2 = 0;			   	
		}
		else
		cnt2 = 0;
	}	
}
#endif
/*----------------------------------------------------------------------------*/
#if AUX_AD_DET
void aux_Ad_check_api(u32 info)
{
   u16 key_value;
	static u8 auxcount=0x00;
	key_value= adc_value[AD_CH_KEY]>>2;
    if(key_value>240) 	// no device 3.06v
	{
        auxcount=0;
		if(g_aux_var.bDevOnline)
		{
            puts("****AUX OUT\n");
			os_taskq_post_event("MainTask", 2, SYS_EVENT_AUX_OUT,info);
			g_aux_var.bDevOnline=AUX_OFF;
		}
	}
	else if(key_value>AUX_KEY) // only aux online//2.85v
	{
		if(auxcount<30)
			auxcount++;
		if(auxcount==25)
		{
			if(!g_aux_var.bDevOnline)
			{
				puts("AUX IN....\n");
				os_taskq_post_event("MainTask", 2, SYS_EVENT_AUX_IN,info);
				g_aux_var.bDevOnline=AUX_ON;
			}
		}
	}
	if(g_aux_var.bDevOnline)
	{
		  index_key|=0x01;
	}
}
#endif
/*----------------------------------------------------------------------------*/
#if AUX_PHONE_ADDET
void aux_phone_check_api(u32 info)
{
	   u16 key_value;
		static u8 fmcount=0x00;
		static u8 auxcount=0x00;
		static u8 auxfmcount=0x00;
		key_value= adc_value[AD_CH_KEY]>>2;
		//put_u16hex(adc_value[AD_CH_KEY]>>2);
		if(key_value>244)	// no device
		{
				auxcount=0;
				fmcount=0;
				auxfmcount=0;
			if(g_phone_var.bDevOnline)
			{
					puts("****FM OUT\n");
				os_taskq_post_event("MainTask", 2, SYS_EVENT_PHONE_OUT,info);
				g_phone_var.bDevOnline=PHONE_OFF;
			}
			if(g_aux_var.bDevOnline)
			{
					puts("****AUX OUT\n");
				os_taskq_post_event("MainTask", 2, SYS_EVENT_AUX_OUT,info);
				g_aux_var.bDevOnline=AUX_OFF;
			}
		}
		else if(key_value>224) // only fm online//3.05//220K
		{
			auxcount=0;
			auxfmcount=0;
			if(fmcount<30)
				fmcount++;
			if(fmcount==25)
			{
				if(!g_phone_var.bDevOnline)
				{
					puts("FM IN....\n");
					os_taskq_post_event("MainTask", 2, SYS_EVENT_PHONE_IN,info);
					g_phone_var.bDevOnline=PHONE_ON;
				}
				if(g_aux_var.bDevOnline)
				{
						puts("****AUX OUT\n");
					os_taskq_post_event("MainTask", 2, SYS_EVENT_AUX_OUT,info);
					g_aux_var.bDevOnline=AUX_OFF;
				}
			}
		}
		else if(key_value>200) // only aux online//2.75v 100k
		{
			fmcount=0;
			auxfmcount=0;
			if(auxcount<30)
				auxcount++;
			if(auxcount==25)
			{
				if(!g_aux_var.bDevOnline)
				{

					puts("AUX IN....\n");
					os_taskq_post_event("MainTask", 2, SYS_EVENT_AUX_IN,info);
					g_aux_var.bDevOnline=AUX_ON;
				}
				if(g_phone_var.bDevOnline)
				{
						puts("****FM OUT\n");
					os_taskq_post_event("MainTask", 2, SYS_EVENT_PHONE_OUT,info);
					g_phone_var.bDevOnline=PHONE_OFF;
				}
			}
		}
		else if(key_value>185)//aux&fm online 2.65V
		{
			fmcount=0;
			auxcount=0;
			if(auxfmcount<30)
				auxfmcount++;
			if(auxfmcount==25)
			{
				if(!g_phone_var.bDevOnline)
				{

					puts("FM IN....\n");
					os_taskq_post_event("MainTask", 2, SYS_EVENT_PHONE_IN,info);
					g_phone_var.bDevOnline=PHONE_ON;
				}
				if(!g_aux_var.bDevOnline)
				{
						puts("****AUX OUT\n");
					os_taskq_post_event("MainTask", 2, SYS_EVENT_AUX_IN,info);
					g_aux_var.bDevOnline=AUX_ON;
				}
			}
		}
		if(g_phone_var.bDevOnline)
		{
		  index_key|=0x01;
		}
		if(g_aux_var.bDevOnline)
		{
		  index_key|=0x02;
		}
}
#endif
/*-------------------------------------------end-----------------------------------------*/
/**@brief   获取ad按键值
   @param   void
   @param   void
   @return  key_number
   @note    tu8 get_adkey_value(void)
*/
/*----------------------------------------------------------------------------*/
tu8 get_adkey_value(void)
{
#if KEY_AD_EN
    tu8 key_number;
    u32 key_value;
    u32 key_value1;	
    u32 key_value2;
#if JL_SDK
    key_value = adc_value[AD_CH_KEY];
    if (key_value > AD_NOKEY)
		return NO_KEY;

    for (key_number = 0; key_number < sizeof (ad_key_table) / sizeof (ad_key_table[0]); key_number++)
	{	
		if (key_value > ad_key_table[key_number])
		break;
	}
	/* printf("adkey_value:%d   key_num:0x%x\n",key_value*33/0x3ff,key_number); */
	return key_number;
#else
//	if(adc_value[AD_CH_KEY]>250)
//	{
//		enable=0x01;
//	}
//	if(enable)
//	{
		key_value1=adc_value[AD_CH_KEY]>>2;
#ifdef DOUBLE_AD
		key_value2=adc_value[AD_CH_KEY_PB9]>>2;
#endif
		for(key_number=0;key_number<9;key_number++)
		{
			if(key_value1<ADKey_Status_Border[index_key][key_number])
			{
				return key_number;
			}
		}
#ifdef DOUBLE_AD
		for(key_number=0;key_number<10;key_number++)
		{
			if(key_value2<ADKey_Status_Border[index_key][key_number])
			{
				return key_number+10;
			}
		}
#endif
	return NO_KEY;
#endif
#else
    return 0xff;
#endif
}	

/*----------------------------------------------------------------------------*/
/**@brief   获取ad值
   @param   void
   @return  ad value 1.8v return 180 
   @note    u32 get_ad_value(void)
*/
/*----------------------------------------------------------------------------*/
u32 get_ad_value(void)
{
#if KEY_AD_EN
	u32 value;
	u32 LDO_ref;
    u32 ad_value;

    value = adc_value[AD_CH_KEY];
	LDO_ref  = adc_value[AD_CH_LDOREF];
	ad_value = 124*value/LDO_ref;

    return ad_value;
#else
    return 0;
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief   获取电池电量
   @param   void
   @param   void
   @return  电池电量值
   @note    tu8 get_battery_level(void)
*/
/*----------------------------------------------------------------------------*/
u16 get_battery_level(void)
{
    u16 battery_value,LDOIN_12,LDO_ref;
    LDOIN_12 = adc_value[AD_CH_LDOIN];
    LDO_ref  = adc_value[AD_CH_LDOREF];//0x181,1.2v

	battery_value = (((u16)LDOIN_12*372)/10)/((u16)LDO_ref); //针对AC69 
	/* battery_value = (((u16)LDOIN_12*368)/10)/((u16)LDO_ref); */
	/* printf("battery_value:0x%x    0x%x   %d\n",LDOIN_12,LDO_ref,battery_value); */
    return battery_value;

}
