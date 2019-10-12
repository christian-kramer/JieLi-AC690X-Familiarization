/*
********************************************************************************
*
*                                   UI UDISK MODULE
*
* Filename      : ui_udisk.c
* Version       : V1.0
* Programmer(s) : GZR
********************************************************************************
*/
#include "sdk_cfg.h"
#include "pc_ui.h"
#include "ui/ui_api.h"

#if 1

#if LED_7_EN
void ui_open_pc_led(void *buf,u32 len)
{
    puts("ui_open_pc_led\n");
//    g_ui_string = UI_STRING_AUX;
//    msg_main_ui.type = MSG_UI_STRING_MAIN;
//    msg_main_ui.ptr = &g_ui_string;
//    msg_main_ui.ptr_len = sizeof(u8);
//    ui_display(MSG_UI_STRING_MAIN,&g_ui_string,sizeof(u8),1);

    SET_UI_MAIN(MENU_PC_MAIN);
    SET_UI_BUF_ADR(NULL);
    SET_UI_BUF_LEN(0);
    UI_menu(MENU_PC_MAIN);
}
#endif

#if LCD_128X64_EN
void ui_open_pc_lcd(void *buf,u32 len)
{
    puts("ui_open_pc_lcd\n");
    SET_UI_MAIN(MENU_PC_MAIN);
    SET_UI_BUF_ADR(NULL);
    SET_UI_BUF_LEN(0);
    UI_menu(MENU_PC_MAIN);
}
#endif

void ui_open_pc(void *buf,u32 len)
{
    //puts("open aux ui\n");
#if UI_ENABLE

#if LED_7_EN
    ui_open_pc_led(buf, len);
#endif

#if UI_SEG_ENABLE
    ui_open_pc_seg(buf, len);
#endif

#if LCD_128X64_EN
    ui_open_pc_lcd(buf, len);
#endif

#if UI_TFT_ENABLE
    ui_open_pc_tft(buf, len);
#endif

#endif/*UI_ENABLE*/
}

void ui_close_pc(void)
{
    SET_UI_MAIN(MENU_WAIT);
    UI_menu(MENU_WAIT);

    SET_UI_BUF_ADR(NULL);
    SET_UI_BUF_LEN(0);
}
#endif/*LINEIN_TASK*/
