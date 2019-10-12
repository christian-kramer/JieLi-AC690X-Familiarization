#include "aec/aec_api.h"
#include "sdk_cfg.h"
#include "dac/ladc.h"
#include "record.h"

 //aec配置
AEC_CFG aec_interface =
{
    .dac_analog_gain = 20,   //dac最大模拟增益
    .mic_analog_gain = 25,    //mic模拟增益
	.aec_toggle = 1,         //aec_toggle default open
    .agc_params =  set_agc_params,
    .agc_params2 = set_agc_params2,
    .argv_params =  set_argv_params,
    .argv_eq_table =  set_argv_eq_table,
    .fill_dac_echo_buf =  fill_dac_echo_buf,
    .fill_adc_ref_buf =  fill_adc_ref_buf,
    .aec_task_start = aec_task_start,
    .aec_stop =  aec_stop,
    .aec_ladc_init = ladc_reg_init,
	.aec_ladc_off  = ladc_close,
    .aec_set_mic_gain = ladc_mic_gain,
#if BT_REC_EN
   .rec_mic_buf_write = bt_rec_buf_write,
#else 
   .rec_mic_buf_write = NULL,
#endif
   
};

const int eq_tab[65] =
{
#if 1
	32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,

	32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,
	//32768*2/3,32768*2/3,32768*2/3,32768*2/3,32768*2/3,32768*2/3,32768*2/3,32768*2/3,32768*2/3,32768*2/3,32768*2/3,32768*2/3,
	//32768*2,32768*2,32768*2,32768*2,32768*2,32768*2,32768*2,32768*2,32768*2,32768*2,32768*2,32768*2,
	//32768*3,32768*3,32768*3,32768*3,32768*3,32768*3,32768*3,32768*3,32768*3,32768*3,32768*3,32768*3,

	32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,
	32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,
	32768,32768,32768,32768,32768,32768,
#else
	32768,32768,32768,36601,48867,51321,51811,51909,51929,51933,51934,51934,51934,51934,51934,51934,51934,
	51934,51934,51934,51934,51934,51934,51934,48101,35835,33381,32891,32793,32773,32769,32768,32768,32768,
	32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,
	32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,
#endif
};

int parNum_advance[] = {0,1,2,3,4,5,6};
int value_advance[] = {1,0,60,10,2,28672,13110*4};

#define AEC_REDUCE		BIT(0)
#define AEC_ADVANCE		BIT(14)

void aec_after_init_set()
{
	//set_advanced_params(parNum_advance,sizeof(parNum_advance)/4,value_advance);
}
void agc_init()
{
    //void set_agc_params(u16 gain,u16 NDT_max_gain,u16 NDT_min_gain ,u16 NDT_Fade_Speed)
    if(aec_interface.agc_params)
    {
        aec_interface.agc_params(64 , 512, 64, 20);
    }
    //set_agc_params2(u16 FarEnd_Talk_Threshold,u16 NearEnd_Begin_Threshold,u16 e_slow,u16 x_slow,u16 frame_cnt)
    if(aec_interface.agc_params2)
    {
        aec_interface.agc_params2(1000 , 30, 0, 0, 0);
    }
    //set_argv_params(u16 suppress_coeff1,u16 suppress_coeff2,u16 detect_thr ,u16 detect_min_gain,u16 eq_gain,u16 adc_pre_delay,u16 dac_pre_delay,u16 aec_ctl)
    if(aec_interface.argv_params)
    {
        /*
		  Reduce Version:当dac声音容易被mic采到，aec_ctl = 0，否则aec_ctl = BIT(0),节省部分运算(For Earphone)
		  Advance Version:set aec_ctl = BIT(14),open all aec modules (For Speaker)
		*/
#if BT_KTV_EN
        aec_interface.argv_params(2 ,0, 0 , 64 , 256, 0 , 0 , AEC_REDUCE);//Reduce
#else
        aec_interface.argv_params(2 ,1000, 50 , 64 , 256, 0 , 0 , AEC_ADVANCE);//Advance
#endif
    }
    if(aec_interface.argv_eq_table)
    {
        aec_interface.argv_eq_table(eq_tab);
    }
    aec_set_after_init_register(aec_after_init_set);

}
