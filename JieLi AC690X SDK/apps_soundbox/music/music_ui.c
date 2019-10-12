#include "sdk_cfg.h"
#include "music_ui.h"
#include "ui/ui_api.h"
#include "echo/echo_api.h"
#include "music.h"
#include "music_info.h"
#include "music_ab_rpt.h"
#if 1


extern A_B_REPEAT_OBJ ab_rept;

//extern REVERB_API_STRUCT * music_reverb;

#if LCD_SEG_EN
void ui_open_music_lcd_seg(void *buf,u32 len)
{
    puts("ui_open_music_lcd_seg\n");
    SET_UI_MAIN(MENU_MUSIC_MAIN);
    SET_UI_BUF_ADR(buf);
    SET_UI_BUF_LEN(len);
    UI_menu(MENU_REFRESH);
}
#endif
#if LED_7_EN
void ui_open_music_led(void *buf,u32 len)
{
    puts("ui_open_music_led\n");
    SET_UI_MAIN(MENU_MUSIC_MAIN);
    SET_UI_BUF_ADR(buf);
    SET_UI_BUF_LEN(len);
    UI_menu(MENU_REFRESH);
}
#endif

#if LCD_128X64_EN
void ui_open_music_lcd(void *buf,u32 len)
{
    puts("ui_open_music_lcd\n");

	if(music_name_buf.tpath == NULL)
	{
    	music_name_buf.tpath= malloc(128 + ALIGN_4BYTE(sizeof(LONG_FILE_NAME)) *2);
		if(music_name_buf.tpath ==NULL)
		{
	    	puts("***disp malloc fail00****\n");
			return;
		}
		else
		{
			music_name_buf.file_name = (LONG_FILE_NAME*) (music_name_buf.tpath + 128);
			music_name_buf.dir_name = (LONG_FILE_NAME*) (music_name_buf.tpath + 128 + ALIGN_4BYTE(sizeof(LONG_FILE_NAME)));	
		}	
	}
	else
	{
	    puts("***disp not null000****\n");
	}

    SET_UI_MAIN(MENU_MUSIC_MAIN);
    SET_UI_BUF_ADR(buf);
    SET_UI_BUF_LEN(len);
    SET_UI_ECHO_PT(&music_reverb);
    UI_menu(MENU_WAIT);
}
#endif

void ui_open_music(void *buf, u32 len)
{
    //puts("open aux ui\n");
    
   
#if UI_ENABLE

    music_ui.ui_file_info = &music_name_buf;
    music_ui.eq_mode = &eq_mode;
    music_ui.play_mode= &play_mode;
    music_ui.opt_state = MUSIC_OPT_BIT_PLAY;
    music_ui.ui_curr_device = NO_DEVICE;
    music_ui.play_time = 0;
    music_ui.lrc_flag= FALSE;

#if DECODE_AB_REPEAT
    music_ui.ab_statu = (void*)&ab_rept.statu;
#endif

#if LED_7_EN
    ui_open_music_led(buf, len);
#endif
#if LCD_SEG_EN
	ui_open_music_lcd_seg(buf, len);
#endif
#if UI_SEG_ENABLE
    ui_open_music_seg(buf, len);
#endif

#if LCD_128X64_EN
    ui_open_music_lcd(buf, len);
#endif

#if UI_TFT_ENABLE
    ui_open_music_tft(buf, len);
#endif

#endif/*UI_ENABLE*/

}

void ui_close_music(void)
{
    SET_UI_MAIN(MENU_WAIT);
    UI_menu(MENU_WAIT);

    SET_UI_BUF_ADR(NULL);
    SET_UI_BUF_LEN(0);
    SET_UI_ECHO_PT(NULL);

    music_ui.ui_curr_device = NO_DEVICE;
    if(music_name_buf.tpath != NULL)
   {
        free(music_name_buf.tpath);
        music_name_buf.tpath = NULL;
   }
}

#endif/*LINEIN_TASK*/
