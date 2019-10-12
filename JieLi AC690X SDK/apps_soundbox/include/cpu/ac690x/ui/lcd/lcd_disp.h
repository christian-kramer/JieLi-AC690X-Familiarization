#ifndef _LCD_DISP_H_
#define _LCD_DISP_H_

#include "sdk_cfg.h"

typedef struct
{
    u8 left;
    u8 top;
    u8 right;
    u8 bottom;
}_lcd_area_size_t;

///------------common-api--------------
void lcd_disp_text(const char *str,u8 dvtxt_num);
u16 lcd_disp_text2(const char *str, u8 dvtxt_num,u8 flags);
void lcd_disp_string_menu(u8 menu);
void lcd_null_dis(void);



///------------main-display--------------
void lcd_disp_welcome(void);
void lcd_dis_IR_number(s32 input_number);
void lcd_dis_vol(s32 vol);

void lcd_clear(void);


///-------------app-display---------------


///------------music-display--------------
void lcd_music_main(void);

///------------bt-display--------------
void lcd_bt_main(u8 menu);

///------------pc-display--------------
void lcd_pc_main(u8 menu);

///------------fm-display--------------
void lcd_fm_main(u8 menu);
void lcd_fm_channel(void);

///------------rtc-display--------------
void lcd_rtc_main(void);

///------------rec-display--------------
void lcd_rec_main(void);

///------------aux-display--------------
void lcd_aux_main(u8 menu);

extern void lcd_rtc_set(void);
extern void lcd_alarm_set(void);
extern void lcd_alarm_ring(void);

#endif/*_LCD_DISP_H_*/
