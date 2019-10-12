#ifndef _LCD128X64_H_
#define _LCD128X64_H_

#include "sdk_cfg.h"

//LCD Command Set
#define LCD_COLUMN_LOW          0x00
#define LCD_COLUMN_HIGH         0x10
#define LCD_V0_RATIO            0x20
#define LCD_POWER_MODE          0x2f
#define LCD_START_LINE          0x40
#define LCD_ELE_VOLUME          0x81
#define LCD_STATIC_IND_OFF      0xac
#define LCD_ON                  0xaf
#define LCD_OFF                 0xae
#define LCD_ALL_ON              0xa5
#define LCD_ALL_NORMAL          0xa4
#define LCD_STATIC_IND_ON       0xad
#define LCD_ADC_SEL             0xa0
#define LCD_DISP_MODE           0xa6
#define LCD_BIAS_SET            0xa2
#define LCD_PAGE_ADDR           0xb0
#define LCD_COMMON_MODE         0xc8
#define LCD_PAGE_BLINK          0xd5
#define LCD_DRIV_MODE           0xd2
#define LCD_NOP                 0xe3
#define LCD_OSC_SET             0xe4
#define LCD_END                 0xee
#define LCD_SOFT_RST            0xe2
#define LCD_DISP_BW             0xa7

#endif/*_LCD128X64_H_*/
