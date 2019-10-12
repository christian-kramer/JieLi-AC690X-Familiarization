
#include "sdk_cfg.h"
#include "ui/ui_api.h"
#include "bt_ui.h"

extern REVERB_API_STRUCT * bt_reverb;
#if 1

#if LED_7_EN
void ui_open_bt_led(void *buf,u32 len)
{
    puts("ui_open_bt_led\n");

    SET_UI_MAIN(MENU_BT_MAIN);
    SET_UI_BUF_ADR(buf);
    SET_UI_BUF_LEN(len);

    UI_menu(MENU_BT_MAIN);
}
#endif

#if LCD_128X64_EN
void ui_open_bt_lcd(void *buf,u32 len)
{
    puts("ui_open_bt_lcd\n");

    SET_UI_MAIN(MENU_BT_MAIN);
    SET_UI_BUF_ADR(buf);
    SET_UI_BUF_LEN(len);
    SET_UI_ECHO_PT(&bt_reverb);
    UI_menu(MENU_BT_MAIN);
}
#endif

void ui_open_bt(void *buf,u32 len)
{

#if UI_ENABLE

#if LED_7_EN
    ui_open_bt_led(buf,len);
#endif

#if LCD_128X64_EN
    ui_open_bt_lcd(buf,len);
#endif

#endif/*UI_ENABLE*/
}

void ui_close_bt(void)
{
    SET_UI_MAIN(MENU_WAIT);
    UI_menu(MENU_WAIT);
}
#endif/*LINEIN_TASK*/
