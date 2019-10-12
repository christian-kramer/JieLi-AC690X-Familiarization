#ifndef __KEY_DRV_AD_H__
#define __KEY_DRV_AD_H__

#include "key_drv/key.h"
#define ADC_KEY_NUMBER 10

#if KEY_AD_EN
#define  USE_AD_KEY0   1
#define  USE_AD_KEY1   0
#endif

//AD channel define
#define AD_KEY_PA3    (0x0<<8)
#define AD_KEY_PA4    (0x1<<8)
#define AD_KEY_PA5    (0x2<<8)
#define AD_KEY_PA6    (0x3<<8)
#define AD_KEY_PA9    (0x4<<8)
#define AD_KEY_PA10   (0x5<<8)
#define AD_KEY_PB7    (0x6<<8)
#define AD_KEY_PB8    (0x7<<8)
#define AD_KEY_PB9    (0x8<<8)
#define AD_KEY_PB10   (0x9<<8)
#define AD_KEY_PB11   (0xA<<8)
#define AD_KEY_PB12   (0xB<<8)
#define AD_KEY_PR1    (0xC<<8)
#define AD_KEY_PR2    (0xD<<8)
#define AD_KEY_LDOIN  (0xE<<8)
#define AD_KEY_LDOREF (0xF<<8)

/*AD通道定义*/
enum
{
#if KEY_AD_EN
    AD_CH_KEY = 0,
#endif
    AD_CH_LDOIN,
    AD_CH_LDOREF,
    MAX_AD_CHANNEL,
};

//#if USE_AD_KEY0
extern u16 adc_value[3];
extern void ad_key0_init(void);
extern tu8 get_adkey_value();
extern u8  ad_key0_get_value(void);
extern u8  ad_key0_pressed_to_msg(u8 key_style, u8 key_num);
#if AUX_AD_DET
void aux_Ad_check_api(u32 info);
#endif
//#endif/*USE_AD_KEY0*/

#if USE_AD_KEY1
extern void ad_key1_init(void);
extern  u8  ad_key1_get_value(void);
extern  u8  ad_key1_pressed_to_msg(u8 key_style, u8 key_num);
#endif /*USE_AD_KEY1*/

void adc_scan(void *param);
u16 get_battery_level(void);
u32 get_next_adc_ch(void);

extern volatile u8 adc_busy_flag;

#endif/*__KEY_DRV_AD_H__*/
