#ifndef _AEC_API_H_
#define _AEC_API_H_

#include "typedef.h"

typedef struct
{
    //--MIC和DAC设置--//
    u16 dac_analog_gain;     //dac模拟增益,取值范围:0-30
    u16 mic_analog_gain;     //mic模拟增益,取值范围:0-63
	u32 aec_toggle;		     //aec toggle 
    void (*agc_params)(u16 gain,u16 NDT_max_gain,u16 NDT_min_gain ,u16 NDT_Fade_Speed);
    void (*agc_params2)(u16 FarEnd_Talk_Threshold,u16 NearEnd_Begin_Threshold,
                            u16 e_slow,u16 x_slow,u16 frame_cnt);
    void (*argv_params)(u16 suppress_coeff1,u16 suppress_coeff2,
                            u16 detect_thr ,u16 detect_min_gain,u16 eq_gain,u16 adc_pre_delay,u16 dac_pre_delay,u16 aec_ctl);
    void (*argv_eq_table)(const int *eq_tab);
    void (*fill_dac_echo_buf)(s16 *buf,u16 len);
    void (*fill_adc_ref_buf)(s16 *buf, u16 len);
    void (*aec_task_start)();
    void (*aec_stop)();
    void (*aec_ladc_init)(u8 ch_sel, u16 sr);
	void (*aec_ladc_off)(u8 ch_sel);
    void (*aec_set_mic_gain)(u8 gain,u8 gx2);
    void (*rec_mic_buf_write)(s16 *buf,u32 len);
}AEC_CFG;
extern AEC_CFG aec_interface;    //aec配置

void agc_init();
extern void set_agc_params(u16 gain,u16 NDT_max_gain,u16 NDT_min_gain ,u16 NDT_Fade_Speed);
extern void set_agc_params2(u16 FarEnd_Talk_Threshold,u16 NearEnd_Begin_Threshold,
                            u16 e_slow,u16 x_slow,u16 frame_cnt);
extern void set_argv_params(u16 suppress_coeff1,u16 suppress_coeff2,
                            u16 detect_thr ,u16 detect_min_gain,u16 eq_gain,u16 adc_pre_delay,u16 dac_pre_delay,u16 aec_ctl);
extern void set_argv_eq_table(const int *eq_tab);
extern void fill_dac_echo_buf(s16 *buf,u16 len);
extern void fill_adc_ref_buf(s16 *buf, u16 len);
extern void aec_task_start();
extern void aec_stop();
extern void aec_get_macro_handle_register(u8 (*handle)());
extern void aec_set_after_init_register(void (*after_init_set_call)());
/*
0:fUseAagc
1:DT_MIC_GAIN
2:MAX_NDT_MIC_GAIN
3:MIN_NDT_MIC_GAIN
4:DT_fadeout_speed
5:Aagc_Target
6:Aagc_TimeConstant
7:DT_EngThr
8:NLPClearThr
9:SpeechSuppress
10:SpeechProb_Thr
*/
extern int set_advanced_params(int *parNum,int nParSet,int *value);
#endif/*_AEC_API_H_*/
