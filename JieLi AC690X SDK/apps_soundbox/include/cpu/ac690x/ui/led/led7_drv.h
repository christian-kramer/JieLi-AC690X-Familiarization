#ifndef	_LED7_DRV_H_
#define _LED7_DRV_H_

#include "includes.h"

#if LED_7_EN
typedef struct _led7_VAR
{
    u8  bCoordinateX;
    u8  bFlashChar;
    u8  bFlashIcon;
    u8  bShowBuff[5];
    u8  bBrightness;
    u8  bShowBuff1[9];
}LED7_VAR;


//void set_LED_fade_out(void);
//void set_LED_all_on(void);

void led7_init(void);
void led7_scan(void *param);
void led7_clear_icon(void);
void led7_show_char(u8 chardata);
void led7_show_number(u8 number);
void led7_show_Hi(void);
void led7_show_music_main(void);
void led7_show_RTC_main(void);
void led7_show_filenumber(void);
void led7_show_volume(s32 vol);
void led7_show_fm_main(void);
void led7_show_IR_number(s32);
void led7_show_pc_main(void);
void led7_show_pc_vol_up(void);
void led7_show_pc_vol_down(void);
void led7_show_aux_main(void);
void led7_show_eq(s32 arg);
void led7_show_playmode(s32 arg);
void led7_show_pause(void);
void led7_show_fm_station(void);
void led7_show_waiting(void);
void led7_show_alarm(void);
//void led7_show_nop(void);
void led7_show_rec_main(void);
void led7_show_linin_main(u8 menu);
void led7_clear(void);
void led7_show_string_menu(u8 menu);
void led7_setX(u8 X);
#define LED_STATUS  led7_var.bShowBuff[4]

#define LED_A   BIT(0)
#define LED_B	BIT(1)
#define LED_C	BIT(2)
#define LED_D	BIT(3)
#define LED_E	BIT(4)
#define LED_F	BIT(5)
#define LED_G	BIT(6)
#define LED_H	BIT(7)

//for LED0
#define LED_PLAY	LED_A
#define LED_PAUSE	LED_B
#define LED_USB		LED_C
#define LED_SD		LED_D
#define LED_2POINT	LED_E
#define LED_MHZ	    LED_F
#define LED_DOT		LED_G
#define LED_MP3     LED_H

//Õ¼ÓÃPB3~PB9
#define  LEDN_PORT_OUT     JL_PORTC->OUT
#define  LEDN_PORT_HD      JL_PORTC->HD
#define  LEDN_PORT_DIR     JL_PORTC->DIR
#define  LEDN_PORT_PD      JL_PORTC->PD
#define  LEDN_PORT_PU      JL_PORTC->PU

#define  LEDN1_PORT_OUT     JL_PORTA->OUT
#define  LEDN1_PORT_HD      JL_PORTA->HD
#define  LEDN1_PORT_DIR     JL_PORTA->DIR
#define  LEDN1_PORT_PD      JL_PORTA->PD
#define  LEDN1_PORT_PU      JL_PORTA->PU

#define  LEDN_S0_BIT   4 
#define  LEDN_S1_BIT   3
#define  LEDN_S2_BIT   2
#define  LEDN_S3_BIT   1
#define  LEDN_S4_BIT   15
#define  LEDN_S5_BIT   14
#define  LEDN_S6_BIT   0
#endif
#if LCD_SEG_EN
typedef struct _LCD_SEG_VAR
{
    u8  bCoordinateX;       //<X ×ø±ê
    u8  bFlashChar;         //×Ö·ûÎ»ÉÁË¸
    u8  bFlashIcon;         //<Í¼±êÉÁË¸
 //   u8  bBrightness;
    u8  bShowBuff[5];       //<ÏÔÊ¾»º´æ
    u16 bHardBuff[4];       //<ÏÔÊ¾»º´æ
}LCD_SEG_VAR;

void LCD_SEG_clear(void);
void LCD_SEG_init(void);
void LCD_show_string_menu(u8 menu);

void LCD_SEG_setX(u8 X);
//void LCD_SEG_clear_icon(void);
void LCD_SEG_clear_all_icon(void);
void LCD_SEG_show_null(void);
void LCD_SEG_show_char(u8 chardata);
void LCD_SEG_show_number(u8 number);
void LCD_SEG_show_string(u8 *str);
void LCD_SEG_show_Hi(void);
void LCD_SEG_show_waiting(void);
void LCD_SEG_show_pause(void);
void LCD_SEG_show_freq(u16 freq);
void LCD_SEG_show_freq_setting(u16 freq);
void LCD_SEG_show_volume(s32 vol);
void LCD_SEG_show_IR_number(s32 ir_num);
void LCD_SEG_show_dev(void);
void LCD_SEG_show_filenumber(void);
void LCD_SEG_show_eq(s32 arg);
void LCD_SEG_show_playmode(s32 arg);
void LCD_SEG_show_pc_main(u8 menu);
void LCD_SEG_show_pc_vol_up(void);
void LCD_SEG_show_pc_vol_down(void);
void LCD_SEG_show_bt_main(void);
void LCD_SEG_show_bt_idle(void);
void LCD_SEG_show_bt_call(void);
void  LCD_SEG_show_music_main(void);
void LCD_SEG_show_fm_main(void);
void LCD_SEG_show_fm_station(void);
void LCD_SEG_show_aux_main(void);
void LCD_SEG_show_batval(void);
void LCD_SEG_show_fm_wait(void);
void LCD_SEG_show_Idle(void);
void LCD_SEG_scan(void *p);

extern LCD_SEG_VAR LCD_SEG_var;

#define COM0_SEG(x)	JL_PORTA->DIE &= ~0xCE18;  JL_PORTA->DIE |= x;JL_PORTC->DIE &= ~0x0003;  JL_PORTC->DIE |= x;
#define COM1_SEG(x) JL_PORTA->HD &= ~0xCE18; JL_PORTA->HD |= x;   JL_PORTC->HD &= ~0x0003; JL_PORTC->HD |= x;
#define COM2_SEG(x)	JL_PORTA->PD &= ~0xCE18; JL_PORTA->PD |= x;  JL_PORTC->PD &= ~0x0003; JL_PORTC->PD |= x;
#define COM3_SEG(x)	JL_PORTA->PU &= ~0xCE18; JL_PORTA->PU |= x;  JL_PORTC->PU &= ~0x0003; JL_PORTC->PU |= x;
//#define COM4_SEG(x)	JL_PORTA->OUT &= ~0xCE18; JL_PORTA->OUT |= x;//JL_PORTC->OUT &= ~0x0001; JL_PORTC->OUT |= x;
//#define COM5_SEG(x)	JL_PORTA->DIR &= ~0xCE1C;  JL_PORTA->DIR |= x;

/*------------------LCD 3 X 9 1/2 Bias--------------*/
/*
			SEG BIT MAP

  7  |   6	|	5	|	4	|	3	|	2	|	1 	|	0
   F     A      B       E       G       D       C
*/
/*-------------------------------------------------*/
#define SEGA    BIT(0)
#define SEGB    BIT(1)
#define SEGC	BIT(2)
#define SEGD	BIT(3)
#define SEGE	BIT(4)
#define SEGF	BIT(5)
#define SEGG	BIT(6)

#define LCD_ICON_USB_Y1		0x33
#define LCD_ICON_SD_Y2		0x23
#define LCD_ICON_P1_Y3		0x13
#define LCD_ICON_MUTE_Y4	0x04
#define LCD_ICON_REC_Y5		0x0b
#define LCD_ICON_BT_Y6		0x03
#define LCD_ICON_MHZ_Y7		0x0f
#define LCD_ICON_COL		0x00
#define LCD_ICON_FM_Y8		0x0f



#define LCD_STATUS1   LCD_SEG_var.bShowBuff[4]
#endif

#endif	/*	_LED_H_	*/
