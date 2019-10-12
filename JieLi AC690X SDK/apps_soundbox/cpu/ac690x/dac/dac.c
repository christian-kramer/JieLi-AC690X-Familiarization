/*******************************************************************************************
 File Name: dac.c

 Version: 1.00

 Discription:


 Author:yulin deng

 Email :flowingfeeze@163.com

 Date:2014-01-13 17:00:21

 Copyright:(c)JIELI  2011  @ , All Rights Reserved.
 *******************************************************************************************/
#include "sdk_cfg.h"
#include "dac/dac_api.h"
#include "dac/eq_api.h"
#include "cbuf/circular_buf.h"
#include "vm/vm_api.h"
#include "key_drv/key_voice.h"
#include "rtos/os_api.h"
#include "aec/aec_api.h"
#include "echo/echo_api.h"
#include "common/app_cfg.h"
#include "dac/ladc.h"
#include "common/common.h"
#include "cpu/dac_param.h"
#include "dac/dac.h"
#include "dac/eq.h"
#include "timer.h"
#include "echo/echo_deal.h"
#include "play_sel/play_sel.h"
#include "fm/fm_radio.h"

#define DAC_SINE_DEBUG	0
extern void set_dac_cnt(int);

u8 dac_read_en;
DAC_CTL dac_ctl;

u8 vocal_flag;
void set_vocal_flag(u8 sw){
	vocal_flag = sw;
}
u8 get_vocal_flag(void){
	return vocal_flag;
}

extern FM_MODE_VAR *fm_mode_var;
/*----------------------------------------------------------------------------*/
/** @brief:数字音量表，必须通过注册函数注册到库内，
                而且长度必须是DIGITAL_VOL_MAX_L+1
    @param:
    @return:
    @author:Juntham
    @note:
*/
/*----------------------------------------------------------------------------*/
const u16 digital_vol_tab[DIGITAL_VOL_MAX_L+1]=
{
    0	  ,
    93	  ,
    111	  ,
    132	  ,
    158	  ,
    189	  ,
    226	  ,
    270	  ,
    323	  ,
    386	  ,//9
    462	  ,
    552	  ,
    660	  ,
    789	  ,
    943	  ,
    1127  ,
    1347  ,
    1610  ,
    1925  ,
    2301  ,//19
    2751  ,
    3288  ,
    3930  ,
    4698  ,
    5616  ,
    6713  ,
    8025  ,
    9592  ,
    11466 ,
    15200 ,//29
    16000 ,
    16384
};

u32 dac_output_counter()
{
    return 0;
}

void dac_set_remote_counter(u32 counter,u32 time)
{

}

void dac_int_enable(void)
{
	dac_read_en = 0;
	dac_ie_api(1);
#if BT_STEREO
		dac_mute(0,0);
#else
		dac_mute(0,1);	
#endif
	
}
void dac_int_disable(void)
{
#if BT_STEREO
		dac_mute(1,0);
#else
		dac_mute(1,1);	
#endif
	
	dac_ie_api(0);
}

void dac_sr_cb(u16 rate)
{
	//printf("dac_sr_cb,rate = %d\n",rate);
#if EQ_EN
	eq_samplerate_sw(rate);
#endif

#if KEY_TONE_EN
	set_key_voice_sr(rate);
#endif
}

void dac_power_on_time(u8 mode)
{
	if(mode == 2)
	 {
		puts("dac_off_delay\n");
		os_time_dly(15);
		//delay(0xAFFF*70);//120ms

	}
	else if(mode == 1)
	 {
		puts("dac_power_on quickly\n");
		//delay(0xAFFF*70);//120ms
		os_time_dly(15);
	}
	else 
	{
		puts("dac_power_on slowly\n");
		delay(0xAFFF*300);//600ms
	}
}

const u8 sine_buf_32K[] =
{
    0x00, 0x00, 0x00, 0x00, 0xae, 0x11, 0xae, 0x11, 0xad, 0x22, 0xad, 0x22, 0x58, 0x32, 0x58, 0x32, 0x13,
    0x40, 0x13, 0x40, 0x58, 0x4b, 0x58, 0x4b, 0xb8, 0x53, 0xb8, 0x53, 0xe0, 0x58, 0xe0, 0x58, 0x9e, 0x5a,
    0x9e, 0x5a, 0xe0, 0x58, 0xe0, 0x58, 0xb8, 0x53, 0xb8, 0x53, 0x58, 0x4b, 0x58, 0x4b, 0x13, 0x40, 0x13,
    0x40, 0x58, 0x32, 0x58, 0x32, 0xad, 0x22, 0xad, 0x22, 0xae, 0x11, 0xae, 0x11, 0x00, 0x00, 0x00, 0x00,
    0x52, 0xee, 0x52, 0xee, 0x53, 0xdd, 0x53, 0xdd, 0xa8, 0xcd, 0xa8, 0xcd, 0xed, 0xbf, 0xed, 0xbf, 0xa8,
    0xb4, 0xa8, 0xb4, 0x48, 0xac, 0x48, 0xac, 0x20, 0xa7, 0x20, 0xa7, 0x62, 0xa5, 0x62, 0xa5, 0x20, 0xa7,
    0x20, 0xa7, 0x48, 0xac, 0x48, 0xac, 0xa8, 0xb4, 0xa8, 0xb4, 0xed, 0xbf, 0xed, 0xbf, 0xa8, 0xcd, 0xa8,
    0xcd, 0x53, 0xdd, 0x53, 0xdd, 0x52, 0xee, 0x52, 0xee
};

extern u32 get_dac_open_clk();
extern int __get_stereo_en_flag();
int dac_cbuf_enough(void)
{
	 if(get_dac_open_clk()&&__get_stereo_en_flag())
	 {
		return 0;
	 }
	 else
	 {
		 if((get_dac_cbuf_len() >= OUTPUT_BUF_SIZE /2)
#if ECHO_EN
				 ||(cbuf_get_data_len_echo()>= (cbuf_get_space_len_echo()/2))
#endif
		   )
		 {
			 return 0;
		 }
		 else
		 {
			 return 1;
		 }
	 	
	 }
}
extern void bt_rec_get_data(s16 *buf,u32 len);//录音获取pcm数据函数
/*----------------------------------------------------------------------------*/
/** @brief:  DAC isr callback function
    @param:
    @return:
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
static u32 read_buf[DAC_SAMPLE_POINT] AT(.dac_buf_sec);
extern int check_a2dp_en();

extern void inc_dac_cnt(u32 dac_points);
void dac_isr_callback(void *dac_buf,u8 buf_flag)
{
#if DAC_SINE_DEBUG
	memcpy(dac_buf,sine_buf_32K,128);
	return;
#endif

#if BT_STEREO
      inc_dac_cnt(32);
#endif
	if(0 == dac_read_en)
	{
		if(dac_cbuf_enough() != 0)	
		{
			memset(dac_buf,0x00,DAC_BUF_LEN);
			#if KEY_TONE_EN
			add_key_voice((s16*)dac_buf,DAC_SAMPLE_POINT*2);
			#endif
			dac_digit_energy_value(dac_buf, DAC_BUF_LEN);
			return;
		}
		dac_read_en = 1;
	}

	if(dac_read(read_buf,DAC_BUF_LEN) != DAC_BUF_LEN) 
	{
		dac_read_en = 0;
	}
#if KEY_TONE_EN
	add_key_voice((s16*)read_buf,DAC_SAMPLE_POINT*2);
#endif

	if(is_dac_mute()) 
	{
		memset(read_buf,0x00,DAC_BUF_LEN);
	}
	else
	{
#if VOCAL_REMOVER
		if((vocal_flag)&&(get_decode_nch() == 2))
		{
			dac_digital_lr_sub(read_buf,DAC_BUF_LEN);
		}
#endif

#if DAC_SOUNDTRACK_COMPOUND
		dac_digital_lr2one(read_buf,DAC_BUF_LEN);
#endif
	}

	dac_digit_energy_value(read_buf, DAC_BUF_LEN);
    digital_vol_ctrl(read_buf, DAC_BUF_LEN);

#if ECHO_EN
    if(fm_mode_var && ((fm_mode_var->scan_mode >= FM_SCAN_BUSY)|| (fm_mode_var->fm_mute)))   ///FM正在搜台，只响应部分按键 //MUTE
	{
	}
	else
	{
		cbuf_mixture_echo(read_buf,DAC_BUF_LEN);
	}
#endif

#if BT_REC_EN
	bt_rec_get_data((s16 *)dac_buf,DAC_BUF_LEN);
#endif

	if(aec_interface.fill_dac_echo_buf)
	{
		aec_interface.fill_dac_echo_buf((s16*)read_buf,DAC_SAMPLE_POINT << 1);
	}

#if EQ_EN
	 eq_run((short*)read_buf,(short*)dac_buf,DAC_SAMPLE_POINT);
#else
	memcpy(dac_buf,read_buf,DAC_BUF_LEN);
#endif

	dac_cbuf_write_detect();
}

/*----------------------------------------------------------------------------*/
/** @brief: Audio Moudule initial
    @param:
    @return:
    @author:Juntham
    @note:
*/
/*----------------------------------------------------------------------------*/
static u32 dac_buf[2][DAC_SAMPLE_POINT] AT(.dac_buf_sec);
void audio_buf_init(void)
{
	puts("audio buf init\n");
	dac_ctl.dma_buf_len = DAC_BUF_LEN;
	dac_ctl.cbuf_len = OUTPUT_BUF_SIZE;
	dac_ctl.dac_dma_buf = (u8*)dac_buf;
	//dac_ctl.dac_dma_buf = malloc(dac_ctl.dma_buf_len*2);
	//ASSERT(dac_ctl.dac_dma_buf);
	dac_ctl.dac_cbuf = malloc(dac_ctl.cbuf_len);
	ASSERT(dac_ctl.dac_cbuf);

	memset(dac_ctl.dac_dma_buf,0x00,sizeof(dac_buf));
	dac_buf_init(dac_ctl.dac_dma_buf,dac_ctl.dma_buf_len,dac_ctl.dac_cbuf,dac_ctl.cbuf_len);
}

void audio_init(void)
{
#if EQ_EN
    eq_init();
#endif

#if KEY_TONE_EN
    key_voice_init();
#endif

	dac_read_en = 0;

	reg_set_sr_fun(dac_sr_cb);
	reg_dac_power_on_time_fun(dac_power_on_time);
	//dac_vcm_rsel_set(0);

	memset(&dac_ctl,0x00,sizeof(DAC_CTL));
    set_max_dac_vol(ANALOG_VOL_MAX_L, ANALOG_VOL_MAX_R, DIGITAL_VOL_MAX_L,DIGITAL_VOL_MAX_R);
    set_max_sys_vol(MAX_SYS_VOL_L, MAX_SYS_VOL_R);

    reg_digital_vol_tab((void *)digital_vol_tab);

	audio_buf_init();

#if ECHO_EN
	echo_reg_digital_vol_tab((void *)digital_vol_tab, sizeof(digital_vol_tab)/sizeof(digital_vol_tab[0]) - 1);
	echo_set_vol(MAX_SYS_VOL_L);
#endif

#if VCOMO_EN   
    dac_init_api(VCOMO_EN, LDO_SLECT, DAC_CHANNEL_SLECT,DAC_ISEL_THIRD);
#else
    dac_init_api(VCOMO_EN, LDO_SLECT, DAC_CHANNEL_SLECT,DAC_ISEL5U);
#endif
	dac_ctl.toggle = 1;
	timer_reg_isr_fun(timer0_hl,5,dac_check,NULL);

	dac_reg_isr_cb_api(3,dac_isr_callback);

    s32 ret;
	ret = timer_reg_isr_fun(timer0_hl,1,dac_fade,NULL);
	if(ret != TIMER_NO_ERR) 
	{
		printf("reg dac_fade err = %x\n",ret);
	}
	//os_time_dly(20);

#if SYS_DEFAULT_VOL
	dac_ctl.sys_vol_l = SYS_DEFAULT_VOL;
	vm_read_api(VM_PHONE_VOL,&dac_ctl.phone_vol);
	//printf("phone_vol:%d\n",dac_ctl.phone_vol);
#else

	///read sys vol from vm
	vm_read_api(VM_SYS_VOL, &dac_ctl.sys_vol_l);
	vm_read_api(VM_PHONE_VOL,&dac_ctl.phone_vol);
	//printf("---------vm_vol:%d\n",dac_ctl.sys_vol_l);
	//printf("phone_vol:%d\n",dac_ctl.phone_vol);

	///limit
	if(dac_ctl.sys_vol_l < 10) 
	{
		dac_ctl.sys_vol_l = 10;
	}
	if(dac_ctl.sys_vol_l > MAX_SYS_VOL_L) 
	{
		dac_ctl.sys_vol_l = MAX_SYS_VOL_L;
	}
#endif
	dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;

	init_sys_vol(dac_ctl.sys_vol_l,dac_ctl.sys_vol_r);
    dac_set_samplerate(32000,1);
}

void dac_on_control()
{
	puts("***dac_on***");
	dac_ctl.toggle = 1;
	dac_on_api(VCOMO_EN, LDO_SLECT, DAC_CHANNEL_SLECT);
	set_sys_vol(dac_ctl.sys_vol_l,dac_ctl.sys_vol_r,FADE_ON);

}

void dac_off_control()
{
	puts("***dac_off***");
	if(play_sel_busy()) 
	{
		puts("--busy,return\n");
		//return;
	}
	//set_sys_vol(0,0,FADE_OFF);
	dac_off_api();
	dac_ctl.toggle = 0;
}

void dac_check()
{
	if(dac_ctl.sniff_check)
	{
		//printf("[%d %d]",key_voice_busy(),dac_ctl.keyvoice_check);
		if((key_voice_busy() == 0) && (dac_ctl.keyvoice_check)) 
		{
			puts("dac_check_end\n");
			dac_ctl.keyvoice_check = 0;
			dac_ctl.sniff_check = 0;
		}
	}
}






