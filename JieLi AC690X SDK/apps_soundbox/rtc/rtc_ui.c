/*
*********************************************************************************************************
*                                             BC51
*
*                                             CODE
*
*                          (c) Copyright 2015-2016, ZHUHAI JIELI
*                                           All Rights Reserved
*
* File : *
* By   : jamin.li
* DATE : 11/11/2015 build this file
*********************************************************************************************************
*/

#include "rtc.h"

#if RTC_CLK_EN

#if LED_7_EN
void ui_open_rtc_led(void *buf,u32 len)
{
    rtc_puts("ui_open_aux_led\n");

    SET_UI_MAIN(MENU_RTC_MAIN);
    SET_UI_BUF_ADR(&rtc_set);
    SET_UI_BUF_LEN(sizeof(RTC_SETTING));

    UI_menu(MENU_RTC_MAIN);
}
#endif
#if LCD_SEG_EN
void ui_open_rtc_lcd_seg(void *buf,u32 len)
{
    rtc_puts("ui_open_rtc_lcd_seg\n");

    SET_UI_MAIN(MENU_RTC_MAIN);
    SET_UI_BUF_ADR(&rtc_set);
    SET_UI_BUF_LEN(sizeof(RTC_SETTING));

    UI_menu(MENU_RTC_MAIN);
}
#endif

#if LCD_128X64_EN
void ui_open_rtc_lcd(void *buf,u32 len)
{
    rtc_puts("ui_open_aux_lcd\n");

    SET_UI_BUF_ADR(&rtc_set);
    SET_UI_BUF_LEN(sizeof(RTC_SETTING));

    if(rtc_set.alarm_set.alarm_flag)
    {
        SET_UI_MAIN(MENU_ALM_UP);
        UI_menu(MENU_ALM_UP);
    }
    else
    {
        SET_UI_MAIN(MENU_RTC_MAIN);
        UI_menu(MENU_RTC_MAIN);
    }


}
#endif

void ui_open_rtc(void *buf,u32 len)
{
#if UI_ENABLE

#if LED_7_EN
    ui_open_rtc_led(buf,len);
#endif
#if LCD_SEG_EN
    ui_open_rtc_lcd_seg(buf,len);
#endif
#if UI_SEG_ENABLE
    ui_open_rtc_seg(buf,len);
#endif

#if LCD_128X64_EN
    ui_open_rtc_lcd(buf,len);
#endif

#if UI_TFT_ENABLE
    ui_open_rtc_tft(buf,len);
#endif

#endif/*UI_ENABLE*/
}

void ui_close_rtc(void)
{
    SET_UI_MAIN(MENU_WAIT);
    UI_menu(MENU_WAIT);

    SET_UI_BUF_ADR(NULL);
    SET_UI_BUF_LEN(0);
}

#endif
