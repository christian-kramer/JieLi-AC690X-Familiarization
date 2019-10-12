#include "sdk_cfg.h"
#include "ui/ui_api.h"
#include "rec_ui.h"
#include "encode/encode.h"

#if REC_EN

#if LED_7_EN
void ui_open_rec_led(void *buf,u32 len)
{
    puts("ui_open_rec_led\n");
//    g_ui_string = UI_STRING_AUX;
//    msg_main_ui.type = MSG_UI_STRING_MAIN;
//    msg_main_ui.ptr = &g_ui_string;
//    msg_main_ui.ptr_len = sizeof(u8);
//    ui_display(MSG_UI_STRING_MAIN,&g_ui_string,sizeof(u8),1);

    SET_UI_MAIN(MENU_RECODE_MAIN);
    SET_UI_BUF_ADR(buf);
    SET_UI_BUF_LEN(len);

    UI_menu(MENU_RECODE_MAIN);
}
#endif
#if LCD_SEG_EN
void ui_open_rec_lcd_seg(void *buf,u32 len)
{
    puts("ui_open_rec_lcd_seg\n");
//    g_ui_string = UI_STRING_AUX;
//    msg_main_ui.type = MSG_UI_STRING_MAIN;
//    msg_main_ui.ptr = &g_ui_string;
//    msg_main_ui.ptr_len = sizeof(u8);
//    ui_display(MSG_UI_STRING_MAIN,&g_ui_string,sizeof(u8),1);

    SET_UI_MAIN(MENU_RECODE_MAIN);
    SET_UI_BUF_ADR(buf);
    SET_UI_BUF_LEN(len);

    UI_menu(MENU_RECODE_MAIN);
}
#endif

#if LCD_128X64_EN
void ui_open_rec_lcd(void *buf,u32 len)
{
    puts("ui_open_rec_lcd\n");

    SET_UI_MAIN(MENU_RECODE_MAIN);
    SET_UI_BUF_ADR(buf);
    SET_UI_BUF_LEN(len);
    UI_menu(MENU_RECODE_MAIN);
}
#endif

void ui_open_rec(void *buf,u32 len)
{
    //puts("open rec ui\n");
#if UI_ENABLE

#if LED_7_EN
    ui_open_rec_led(buf, len);
#endif

#if LCD_SEG_EN
	ui_open_rec_lcd_seg(buf, len);
#endif

#if UI_SEG_ENABLE
    ui_open_rec_seg(buf, len);
#endif

#if LCD_128X64_EN
    ui_open_rec_lcd(buf, len);
#endif

#if UI_TFT_ENABLE
    ui_open_rec_tft(buf, len);
#endif

#endif/*UI_ENABLE*/
}

void ui_close_rec(void)
{
    SET_UI_MAIN(MENU_WAIT);
    UI_menu(MENU_WAIT);

    SET_UI_BUF_ADR(NULL);
    SET_UI_BUF_LEN(0);
}

#endif/*LINEIN_TASK*/
