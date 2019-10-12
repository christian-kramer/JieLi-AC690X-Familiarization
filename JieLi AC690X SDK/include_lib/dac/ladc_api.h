#ifndef _LADC_API_H_
#define _LADC_API_H_

#include "typedef.h"
#include "cbuf/circular_buf.h"

typedef struct _LADC_CTL
{
    cbuffer_t *c_buff[4];
    u8 *buff[4];
    void (*save_ladc_buf)(void *,void *,u8, u32);
    u8  source_chl;
}LADC_CTL;
extern LADC_CTL *p_ladc;
extern LADC_CTL *p_ladc_rec;
 
void ladc_reg_isr_cb_api(void (*callback_fun)(void *buf,u32 buf_flag,u32 buf_len));
void ladc_enable(u8 ch_sel,u16 sr,u8 vcom_outen);
void ladc_disable(u8 ch_sel);
void ladc_mic_en(u8 en);
void ladc_mic_gain(u8 gain,u8 gx2);
void ladc_mic_neg12(u8 en);//-12db enable
void ladc_mic_mute(u8 mute);
void ladc_sr_set(u16 sr);
void ladc_pga_en(u8 index,u8 en);
/*
 *********************************************************************************************************
 *
 * Description: ladc pga gain 
 * Arguments  : index=0,1,2 
 * Arguments  : gain = 0x0~0xF 
 * Returns    : NULL 
 *
 * Note       :	void ladc_pga(u8 index,u8 gain);
 *********************************************************************************************************
 */
void ladc_pga_gain(u8 index,u8 gain);
/*
 *********************************************************************************************************
 *
 * Description: ladc isel main_switch,effects all adc channels
 * Arguments  : isel=0,Full;isel=1,Half 
 * Returns    : NULL 
 *
 * Note       :	void ladc_isel_set(u8 isel);
 *********************************************************************************************************
 */
void ladc_isel_set(u8 isel);
/*
 *********************************************************************************************************
 *
 * Description: ladc isel sub_switch,only effects the specified channel
 * Arguments  : ch,0=left_ch,1=right_ch,2=mic_ch,3=left_right_ch 
 *				isel(0~3),3=max,0=min
 * Returns    : NULL 
 *
 * Note       :	void ladc_ch_isel(u8 ch,u8 isel);
 *********************************************************************************************************
 */
void ladc_ch_isel(u8 ch,u8 isel);
/*
 *********************************************************************************************************
 *
 * Description:	UnDelayEnable,adc configs take effects immediately 
 * Arguments  : 1=effect immediately
 *			    0=Delay effects until pending happen	
 *				e.g, when you want to modify the mic_gain,you can perform it as follows:
 *					ladc_adcude(1);
 *					ladc_mic_gain(30,0);
 *					ladc_adcude(0);
 * Returns    : NULL 
 *
 * Note       :	void ladc_adcude(u8 ude);
 *********************************************************************************************************
 */
void ladc_adcude(u8 ude);

#endif
