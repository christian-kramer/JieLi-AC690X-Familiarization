#ifndef _EQ_API_H_
#define _EQ_API_H_


#include "typedef.h"

//#define EF_ONLINE_DEBUG
#ifdef EF_ONLINE_DEBUG
#define eq_printf printf
#define eq_puts puts
#define eq_put_buf put_buf
#else
#define eq_printf(...)
#define eq_puts(...)
#define eq_put_buf(...)
#endif

void soft_eq_init(u8 flag);
void soft_eq_set_segment_num(u8 num);
/*eq_set_eq_bypass_flag在eq正常开启的情况下可以设置1，
 * 此时将不做eq运算，设置0又会重新开始eq运算。例如在提示音播放的不做eq可成对使用
 * */
void eq_set_eq_bypass_flag(u8 flag);
void eq_init_api(int EQ_isr_priority);
void eq_en_api(const int *coeff, int nChannel, int nSection);
void eq_dis_api(void);
void eq_run_api(short *in,short *out,int npoint);

u32 get_eq_state_api(void);
void eq_mode_api(int mode);
void eq_samplerate_api(int sr);

int eq_config_api(int *eq_filter_44100,
              	  int *eq_filter_22050,
              	  int *eq_filter_11025,
              	  int *eq_filter_48000,
              	  int *eq_filter_24000,
              	  int *eq_filter_12000,
              	  int *eq_filter_32000,
              	  int *eq_filter_16000,
              	  int *eq_filter_8000,
              	  int (*freq_gain)[10],
              	  int *global_gain, int en_flag);
extern void set_sys_eq(u8 type);
#endif // EQ_driver_h__
