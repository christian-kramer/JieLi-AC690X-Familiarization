#ifndef __ADC_API_H__
#define __ADC_API_H__

#include "typedef.h"

void adc_init_api(u32 channel,u32 lsb_clk,u8 lvd_en);
void adc_off_api(void);
u16 adc_res_api(u32 channel);
void adc_ch_sel(u32 ch);
void adc_lvd_ch_sel(u32 ch);

#endif
