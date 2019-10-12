#ifndef _ENCHO_DEAL_H_
#define _ENCHO_DEAL_H_

#include "typedef.h"
#include "string.h"
#include "mem/malloc.h"
#include "common/app_cfg.h"
#include "rtos/os_api.h"
#include "rtos/os_cfg.h"
#include "common/error.h"
#include "dac/ladc_api.h"
#include "dac/dac_api.h"
#include "reverb_api.h"
#include "aec/AnalogAGC.h"

#define ECHO_RUN_NAME   "ECHO_RUN"

///echo spec define src
#define ECHO_11K_SR    10500
#define ECHO_12K_SR    11424
#define ECHO_08K_SR    7616

#define ECHO_22K_SR    21000
#define ECHO_24K_SR    22848
#define ECHO_16K_SR    15232

#define ECHO_44K_SR    42000
#define ECHO_48K_SR    45696
#define ECHO_32K_SR    30464

///normal src
#define NOR_11K_SR    11025
#define NOR_12K_SR    12000 
#define NOR_08K_SR    8000 

#define NOR_22K_SR    22050 
#define NOR_24K_SR    24000 
#define NOR_16K_SR    16000 

#define NOR_44K_SR    44100 
#define NOR_48K_SR    48000 
#define NOR_32K_SR    32000 

#define REVERB_SR 	NOR_16K_SR


typedef struct _MIC_VAR_SET
{
	u8 use_magc;//agc_sw
	s8 max_gain;//agc最大增益
	s8 min_gain;//agc最小增益
    s8 space;   //
	s32 tms;    //爆掉之后的音量抑制多少ms
	s32 target; //
}MIC_VAR_SET;

typedef struct _MIC_VAR
{
    u8 howling_flag;       //
	u8 howling_cnt;
 	s8 vol_hold;
	u8 howling_cnt_threshold;
	u8 use_magc;
	s8 cur_mic_gain;
	s8 last_mic_gain;	
	short *buff;	
	MIC_INIT_SET agcp;
	Analog_AGC Aagc;
}MIC_VAR;

typedef struct _REVERB_API_STRUCT_
{
    REVERB_PARM_SET *reverb_parm_obj;  //参数
	reverb_audio_info *audio_info;
    unsigned char *ptr;                   //运算buf指针
    REVERB_FUNC_API *func_api;            //函数指针
    REVERB_IO *func_io;                   //输出函数接口
    LADC_CTL *ladc_p;
	MIC_VAR  *mic_var;
}REVERB_API_STRUCT;


/*----------------------------------------------------------------------------*/
/**@brief  混响mic增益设置函数
   @param  gain:(-27-0 : 0-63: 64-76), immediate:是否马上使能
   @return 无
   @note   AGC使能，设置AGC最大值，AGC关闭，则设置mic增益。
*/
/*----------------------------------------------------------------------------*/
extern void echo_set_mic_vol(u8 gain,u8 immediate);

extern void reverb_stop(void *reverb_api_obj);
extern REVERB_FUNC_API *get_reverb_func_api(void);

extern void echo_set_vol(u8 vol);
extern void echo_reg_digital_vol_tab(void *tab, u8 len);

void reg_mic_var(MIC_VAR_SET* parm);
void reg_ef_reverb_parm2(EF_REVERB_PARM2 * parm);

#if ECHO_PARM
void set_reverb_parm(void *reverb_obj, u16 sr, u16 pre_delay_ms);
#else
void set_reverb_parm(void *reverb_obj, u16 deep_val, u16 vol_val);
#endif

void howlingsuppress_sw(u8 on);//啸叫抑制开关
void howlingsuppress_suppression_val(u8 val);//0-250: 啸叫抑制程度，值越大抑制效果越大，但会影响声音频谱，建议值120。

#endif
