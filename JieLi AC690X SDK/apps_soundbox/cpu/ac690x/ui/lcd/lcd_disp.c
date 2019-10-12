#include "sdk_cfg.h"
#include "dac/dac_api.h"
#include "ui/lcd/lcd_disp.h"
#include "uicon/ui_con.h"
#include "uicon/ui.h"
#include "ui/ui_api.h"
#include "dec/decoder_phy.h"
#include "music.h"
#include "music_ui.h"
#include "ui/ui_common.h"
/* #include "uicon/ui_con1.h" */
#include "fm/fm_radio.h"
#include "timer.h"
#include "key_drv/key.h"
#include "file_operate/file_op.h"
#include "dac/dac.h"
#include "rtc_setting.h"

#include "lyrics_api.h"

#if REC_EN
#include "encode/encode.h"
#endif

#if  LCD_128X64_EN
#include "ui/lcd/lcd_drv_interface.h"
#include "bluetooth/bt_ui.h"


typedef enum
{
    AB_RPT_NON = 0,
    AB_RPT_ASTA,
    AB_RPT_BSTA
}AB_RPT_STA;



static const _lcd_area_size_t time_set_backgroup[] =
{
    //left,top,right,bottom

    //DVcTime1_5
    {19,3,55,4},
    {64,3,82,4},
    {91,3,109,4},

    //DVcTime2_5
    {41,5,59,6},
    {68,5,86,6},

    //DVcRzImg1_5
    {90,5,106,6},
};

static const GUI_POINT  del_pic_point = {0,32};
static const GUI_POINT  pause_pic_point = {16,32};
static const GUI_POINT  vol_pic_point = {80,0};
//static const GUI_POINT  echo_pic_point = {80,0};
static const GUI_POINT  eq_pic_point = {110,0};
static const GUI_POINT  playmode_pic_point = {96,0};
/* static const u8 play_mode_pic[] = {DVcRzImg1_6,DVcRzImg2_6,DVcRzImg3_6,DVcRzImg4_6}; */
static const u8 play_mode_pic[] = {DVcRzImg1_6,DVcRzImg3_6,DVcRzImg3_6,DVcRzImg4_6,DVcRzImg2_6};
static const u8 eq_mode_pic[] = {DVcRzImg5_6,DVcRzImg6_6,DVcRzImg7_6,DVcRzImg8_6,DVcRzImg9_6,DVcRzImg10_6};

static const u8 rec_opt_pic[] = {DVcRzImg4_8,DVcRzImg2_8,DVcRzImg3_8,DVcRzImg4_8};
static const GUI_POINT  rec_opt_pic_point = {32,48};


#define MUSIC_UI_FILENAME_TEXTSIZE  14
#define MUSIC_UI_FOLDER_TEXTSIZE    14
u16 music_ui_filename_pt;
u16 music_ui_foldername_pt;

///------------common-api--------------

int unicode_len(u8 *str)
{
    u8 *src = str;
    while ((*str != 0) || (*(str + 1) != 0))
    {
        str += 2;
    }
    return (int) (str - src);
}

void lcd_set_cur_menu(u8 menu)
{
    UI_var.bMenuReturnCnt = 0;
    UI_var.bCurMenu = menu;
}

u16 lcd_disp_text2(const char *str, u8 dvtxt_num,u8 flags)
{
    DTEXT disp_txt;
    u16 show_bytes;

    disp_txt.buf = (u8 *)str;

    disp_txt.flags = flags;

//    puts("d-s\n");

    if((flags&ANSI) != 0)
    {
        disp_txt.len = strlen((const char *)disp_txt.buf);
    }
    else
    {
        disp_txt.len = unicode_len((u8 *)disp_txt.buf);
        //printf("\ndisp_unicode:%d\n",disp_txt.len);
        //printf_buf(disp_txt.buf,disp_txt.len);
    }

//    puts("disp_buf:\n");
//    printf_buf(str,disp_txt.len);

    show_bytes = ui_text(dvtxt_num, &disp_txt);

//    puts("d-e\n");

    return show_bytes;
}


void lcd_disp_text(const char *str, u8 dvtxt_num)
{

    lcd_disp_text2(str,dvtxt_num,ANSI);

    /*
        DTEXT disp_txt;

        disp_txt.buf = (u8 *)str;

        disp_txt.flags = ANSI;
        disp_txt.len = strlen((const char *)disp_txt.buf);
        ui_text(dvtxt_num, &disp_txt);
    */
}

void lcd_disp_string_menu(u8 menu)
{
    const char lcd_string[][17] =
    {
        "     HELLO      ",
        "    LOADING     ",
    };

	const char dev_err_string[][17] = 
	{
        "    Device      ",
        "  not exist!!!  ",
	};

#if RTC_ALM_EN
    if(menu == MENU_ALM_UP)
    {
        lcd_alarm_ring();
        return;
    }
#endif

	if(menu == MENU_DEV_ERR)
	{
    	lcd_disp_text(dev_err_string[0], DVcTxt2_1);
    	lcd_disp_text(dev_err_string[1], DVcTxt3_1);
		return;
	}

    lcd_disp_text(lcd_string[menu], DVcTxt1_2);
}

void lcd_null_dis(void)
{
//    ui_pic(DVcRzImg1_3,NULL);
}

int lcd_check_sys_vol(void)//静音显示
{
#if 1
    if((*(UI_var.sys_vol) == 0)||(is_dac_mute()))
    {
    //	printf("---mute : %d\n",is_dac_mute());
		ui_pic(DVcRzImg1_3,(void *)&vol_pic_point);
		return 1;
    }
#endif
	return 0;
}

void  lcd_check_echo_sw(void)//混响显示
{

	if(lcd_check_sys_vol())
	{
		return;
	}

#if ECHO_EN

//    printf("echo:%x\n",UI_var.echo_pt);

//    if(UI_var.echo_pt != NULL)
//    {
//        printf("ept:%x\n",UI_var.echo_pt[0]);
//    }
    if(*(UI_var.sys_vol) != 0)
    {
        if((UI_var.echo_pt != NULL)&&(UI_var.echo_pt[0] != 0))
        {
            ui_pic(DVcRzImg4_3,(void *)&vol_pic_point);
        }
    }
#endif
}

void lcd_disp_rec_error(void)
{
#if REC_EN
    const char lcd_string[] = {"  REC error!!!  "};
    lcd_clear();
    lcd_disp_text(lcd_string, DVcTxt1_2);
    lcd_set_cur_menu(MENU_RECODE_ERR);
#endif
}

void  lcd_disp_rec_opt_state(void)//静音
{
#if REC_EN
    if(UI_var.rec_opt_state >= UI_REC_OPT_ERR)
    {
        UI_var.rec_opt_state = UI_REC_OPT_STOP;
        lcd_disp_rec_error();
        return;
    }

    if(file_operate_get_total_phydev() == 0)
    {
        UI_var.rec_opt_state = UI_REC_OPT_NODEV;
    }
    else
    {
        ui_pic(DVcRzImg1_8,0);
        ui_pic(rec_opt_pic[UI_var.rec_opt_state],(GUI_POINT*)&rec_opt_pic_point);
    }
#endif
}

///------------main-display--------------
void lcd_disp_welcome(void)
{
    const char lcd_string[] = {"     HELLO      "};
    lcd_disp_text(lcd_string, DVcTxt1_2);
}

void lcd_dis_IR_number(s32 input_number)
{
    u8 i,input_cnt;
    char numb_var[] = {"      ----      "};
    DTEXT disp_txt;

    input_cnt = (input_number>>24);
    input_number &= 0xffff;
    //printf("ir-input: %d,%d\n",input_number,input_cnt);

    itoa4(input_number);
    for(i = 0; i< input_cnt; i++)
    {
        numb_var[9-i] = bcd_number[3-i];
    }
    disp_txt.buf = (u8 *)numb_var;
    disp_txt.flags = ANSI;
    disp_txt.len = strlen((char *)disp_txt.buf);
    ui_text(DVcTxt1_2, &disp_txt);
}

void lcd_dis_vol(s32 vol)
{
    char vol_var[] = {"     VOL 00     "};
    DTEXT disp_txt;

    itoa2((u8)*UI_var.sys_vol);
    vol_var[9] = bcd_number[0];
    vol_var[10] = bcd_number[1];

    disp_txt.buf = (u8 *)vol_var;

    disp_txt.flags = ANSI;
    disp_txt.len = strlen((char *)disp_txt.buf);
    ui_text(DVcTxt1_2, &disp_txt);

}

//typedef enum
//{
//    SD0_DEVICE = 0x00,
//    SD1_DEVICE,
//    USB_DEVICE,
//    NO_DEVICE = 0xff,
//}UI_DEVICE;
void lcd_dis_dev(u32 dev, u8 dvtxt_num)
{
//    DTEXT disp_txt;
    char lcd_dev_str[][4] =
    {
        "SD0",
        "SD1",
        "USB",
        "NON",
    };

    lcd_disp_text(lcd_dev_str[dev],dvtxt_num);//目录

//    ui_text(, &disp_txt);
}


///-------------app-display---------------

//返回显示字符偏移位置
int text_scroll_deal(u16 disp_len,u16 text_size,int pt)
{
    if((disp_len <= text_size) || (disp_len < pt))
    {
        return 0;
    }

    disp_len = disp_len - pt;

    if(text_size >= disp_len)
    {
        return 0;
    }

    return pt+1;

}

int disp_unicode_len(u8 *str)
{
    int char_cnt = 0;
    u8 *src = str;
    while ((*str != 0) || (*(str + 1) != 0))
    {
        if(*(str + 1) == 0)
        {
            char_cnt++;
        }
        str+=2;
    }
    return (int) (str - src) - char_cnt;
}

///------------music-display--------------
void lcd_music_main(void)
{

    char lcd_file_num[] = {"    0000/0000"};
    u8 flags;
    u32 tmp;
    u8 *point;


    MUSIC_DIS_VAR *music_var;
    char lcd_string[] = {"音乐      "};

    music_var = (MUSIC_DIS_VAR*)UI_var.ui_buf_adr;

    if(music_var)
    {
#if DECODE_AB_REPEAT
        //printf("m_ab:%x\n",*(music_var->ab_statu));
        if(AB_RPT_NON != *(music_var->ab_statu))
        {
            lcd_string[5] = 'A';
            lcd_string[6] = '-';
            if(AB_RPT_BSTA == *(music_var->ab_statu))
            {
                lcd_string[7] = 'B';
            }
        }
        else
#endif
        {
            //display time
            itoa2_api(music_var->play_time/60,(u8 *)&lcd_string[5]);
            itoa2_api(music_var->play_time%60,(u8 *)&lcd_string[8]);
            lcd_string[7] = ':';
        }

        lcd_disp_text(lcd_string,DVcTxt1_1);//音乐模式

        lcd_check_sys_vol();
        lcd_check_echo_sw();
//EQ
        ui_pic(eq_mode_pic[*(music_var->eq_mode)],(GUI_POINT*)&eq_pic_point);

//playmode
        ui_pic(play_mode_pic[*(music_var->play_mode) - REPEAT_ALL],(GUI_POINT*)&playmode_pic_point);

        if(music_var->ui_file_info->update_flag != 0)
        {
            //start play music,init parm
            music_ui_filename_pt = 0;
            music_ui_foldername_pt = 0;
            music_var->ui_file_info->update_flag = 0;
            //puts("file_ui_update\n");
        }

//显示文件名
        point = (u8 *)music_var->ui_file_info->file_name->lfn;
        tmp = music_var->ui_file_info->file_name->lfn_cnt;
        if(tmp > 0)
        {
            flags = UNICODE_LE;

            if(UI_var.param_menu == MENU_HALF_SEC_REFRESH)
            {
                //半秒才刷
                if(disp_unicode_len(point) > MUSIC_UI_FILENAME_TEXTSIZE)
                {
                    music_ui_filename_pt = text_scroll_deal(tmp,MUSIC_UI_FILENAME_TEXTSIZE,music_ui_filename_pt);
                }
            }
            else
            {
                music_ui_filename_pt = 0;
            }

        }
        else
        {
            flags = ANSI;
        }

        ui_pic(DVcRzImg3_3,0);//folder
        lcd_disp_text2((const char*)point + (music_ui_filename_pt&0xfffe),DVcTxt2_3,flags);//文件

//del file
        if((music_var->opt_state & MUSIC_OPT_BIT_DEL) != 0)
        {
            puts("del\n");
            ui_pic(DVcRzImg4_7,(GUI_POINT*)&del_pic_point);
            return;
        }

//pasue
        if((music_var->opt_state & MUSIC_OPT_BIT_PAUSE) != 0)
        {
            ui_pic(DVcRzImg3_7,(GUI_POINT*)&pause_pic_point);
            return;
        }

//FF
        if((music_var->opt_state & MUSIC_OPT_BIT_FF) != 0)
        {
            ui_pic(DVcRzImg2_7,(GUI_POINT*)&pause_pic_point);
            return;
        }

//FR
        if((music_var->opt_state & MUSIC_OPT_BIT_FR) != 0)
        {
            ui_pic(DVcRzImg1_7,(GUI_POINT*)&pause_pic_point);
            return;
        }

//LRC
#if LRC_LYRICS_EN
    if((music_ui.opt_state == MUSIC_OPT_BIT_PLAY)&&
        (music_ui.lrc_flag== TRUE))
    {
        if(0 == lrc_show(music_var->play_time, 0))
        {
            return;
        }
    }
#endif


//显示目录文件夹

        point = (u8 *)music_var->ui_file_info->dir_name->lfn;
        tmp = music_var->ui_file_info->dir_name->lfn_cnt;
        if(tmp > 0)
        {
            flags = UNICODE_LE;
            if(UI_var.param_menu == MENU_HALF_SEC_REFRESH)
            {
                //半秒才刷
                if(disp_unicode_len(point) > MUSIC_UI_FOLDER_TEXTSIZE)
                {
                    music_ui_foldername_pt = text_scroll_deal(tmp,MUSIC_UI_FOLDER_TEXTSIZE,music_ui_foldername_pt);
                }
            }
            else
            {
                music_ui_foldername_pt = 0;
            }

        }
        else
        {
            flags = ANSI;
        }

        ui_pic(DVcRzImg2_3,0);//folder
        lcd_disp_text2((const char*)point + (music_ui_foldername_pt&0xfffe),DVcTxt3_3,flags);//文件夹

        itoa4_api(music_var->ui_curr_file,(u8*)&lcd_file_num[4]);
        itoa4_api(music_var->ui_total_file,(u8*)&lcd_file_num[9]);
        lcd_disp_text(lcd_file_num,DVcTxt4_3);//目录

        lcd_dis_dev(music_var->ui_curr_device, DVcTxt4_3);

    }
}


///------------bt-display--------------
void lcd_bt_main(u8 menu)
{
    const char lcd_string[] = {"蓝牙"};
    lcd_disp_text(lcd_string, DVcTxt1_1);

#if 1
    BT_DIS_VAR * bt_var;

    bt_var = (BT_DIS_VAR *)UI_var.ui_buf_adr;

    if(bt_var)
    {
        if((BT_STATUS_CONNECTING == bt_var->ui_bt_connect_sta)||
                (BT_STATUS_TAKEING_PHONE == bt_var->ui_bt_connect_sta)||
                (BT_STATUS_PLAYING_MUSIC == bt_var->ui_bt_connect_sta))
        {
            const char connecting_str[] = {"    连接成功    "};
            lcd_disp_text(connecting_str, DVcTxt2_1);

            if (BT_STATUS_TAKEING_PHONE == bt_var->ui_bt_connect_sta)
            {
                const char taking_phone_str[] = {"    正在通话    "};
                lcd_disp_text(taking_phone_str, DVcTxt3_1);
                lcd_disp_rec_opt_state();
            }
            else
            {
                if(BT_STATUS_PLAYING_MUSIC == bt_var->ui_bt_connect_sta)
                {
                    ///playing
                    const char music_play_str[] = {"    音乐播放    "};
                    lcd_disp_text(music_play_str, DVcTxt3_1);
                }
                else
                {
                    ///pause
                    const char music_pause_str[] = {"    音乐暂停    "};
                    lcd_disp_text(music_pause_str, DVcTxt3_1);
                }
                lcd_check_echo_sw();
            }
        }
        else
        {
            const char connecting_str[] = {"    等待连接    "};
            lcd_disp_text(connecting_str, DVcTxt2_1);
            lcd_check_echo_sw();
        }

        lcd_check_sys_vol();

    }
#endif
//    puts("---bt-display---\n");
}

///------------pc-display--------------
void lcd_pc_main(u8 menu)
{
    const char lcd_string[] = {" PC "};
    lcd_disp_text(lcd_string, DVcTxt1_1);
    ui_pic(DVcRzImg1_1, 0);
	/* lcd_check_sys_vol(); */
}

///------------fm-display--------------
void lcd_fm_main(u8 menu)
{
    const char lcd_string[] = {"收音"};
    char lcd_freq_str[] = {"        87.5MHz "};

    lcd_disp_text(lcd_string, DVcTxt1_1);
#if FM_RADIO_EN
    FM_MODE_VAR * fm_var;

    if(!UI_var.ui_buf_adr)
        return;

    fm_var = *(FM_MODE_VAR **)UI_var.ui_buf_adr;

    if(fm_var)
    {
        if(fm_var->wFreChannel > 0)
        {
            itoa2((u8)fm_var->wFreChannel);
            lcd_freq_str[3] = 'C';
            lcd_freq_str[4] = bcd_number[0];
            lcd_freq_str[5] = bcd_number[1];
        }

        itoa4(fm_var->wFreq);

        if (fm_var->wFreq <= 999)
        {
            bcd_number[0] = ' ';
        }
        lcd_freq_str[7] = bcd_number[0];
        lcd_freq_str[8] = bcd_number[1];
        lcd_freq_str[9] = bcd_number[2];
        lcd_freq_str[11] = bcd_number[3];

        lcd_disp_text(lcd_freq_str, DVcTxt3_1);

        lcd_check_sys_vol();
        lcd_disp_rec_opt_state();
		if(fm_mode_var && ((fm_mode_var->scan_mode >= FM_SCAN_BUSY)|| (fm_mode_var->fm_mute)))   ///FM正在搜台，只响应部分按键 //MUTE
		{
			printf("no_show_echo %d %d\n",fm_mode_var->scan_mode, fm_mode_var->fm_mute);
		}
		else
		{
			printf("show_echo\n");
			lcd_check_echo_sw();
		}
	}
#endif
}

void lcd_fm_channel(void)
{
//    lcd_disp_text(9, DVcTxt1_3);
}

///------------rtc-display--------------
void lcd_rtc_main(void)
{
    RTC_SETTING * rtc_var;
    RTC_TIME *ui_time_var;

    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if(rtc_var == 0)
    {
        return;
    }

    if(rtc_var->rtc_set_mode == RTC_SET_MODE)
    {
        lcd_rtc_set();
        return;
    }
    else
    {
        if(rtc_var->rtc_set_mode == ALM_SET_MODE)
        {
            //超时后界面也要显示闹钟设置，让上层来决定什么时候更新显示
            lcd_alarm_set();
            return;
        }
    }

    const char lcd_string[] = {"时钟"};
    lcd_disp_text(lcd_string, DVcTxt1_1);

    if(rtc_var)
    {
        ui_time_var = rtc_var->calendar_set.curr_rtc_time;
        ui_time(DVcTime1_4,(TIME *)ui_time_var,-1,-1,0);
        ui_time(DVcTime2_4,(TIME *)ui_time_var,-1,-1,0);
    }
}

///------------rec-display--------------
void lcd_rec_main(void)
{
#if REC_EN
    RECORD_OP_API * rec_var_p;
    REC_CTL * rec_ctl_var;
    u32 rec_time;

//    TIME time_tmp;
    char lcd_time_str[] = {"     00:00:00  "};
    const char lcd_string[] = {"录音"};
    char rec_nodevice_str[] = {"   no device!!!"};
    const char lcd_sta_str[][17] =
    {
        "    ENC_STOP    ",
        "    ENC_START   ",
        "    ENC_PAUSE   ",
    };

    lcd_disp_text(lcd_string, DVcTxt1_1);

    if(UI_var.ui_buf_adr)
    {
        rec_var_p = *(RECORD_OP_API **)UI_var.ui_buf_adr;
        if(rec_var_p)
        {
            rec_ctl_var = rec_var_p->rec_ctl;
            if(rec_ctl_var)
            {
                lcd_disp_text((const char *)(lcd_sta_str[rec_ctl_var->enable]), DVcTxt2_1);

                rec_time = rec_ctl_var->file_info.enc_time_cnt;

                if(rec_ctl_var->enable != ENC_STOP)
                {
                    lcd_dis_dev(rec_ctl_var->curr_device,DVcTxt3_1);
                    /* printf("rec_time = %d\n",rec_time); */
                    itoa2_api(rec_time/3600,(u8 *)&lcd_time_str[5]);
                    itoa2_api(rec_time%3600/60,(u8 *)&lcd_time_str[8]);
                    itoa2_api(rec_time%60,(u8 *)&lcd_time_str[11]);
					/* printf("\n%s\n",lcd_time_str); */
                    lcd_disp_text((const char *)(lcd_time_str), DVcTxt3_1);//TIME
                }
                //return;
            }
        }
        else
        {
            lcd_disp_text((const char *)(lcd_sta_str[ENC_STOP]), DVcTxt2_1);//STOP
            if(file_operate_get_total_phydev() == 0)
            {
                lcd_disp_text(rec_nodevice_str, DVcTxt3_1);
                return;
            }
        }

        lcd_disp_rec_opt_state();
		lcd_check_sys_vol();
    }

#endif

}

///------------aux-display--------------
void lcd_aux_main(u8 menu)
{
    const char lcd_string[] = {"AUX"};
    lcd_disp_text(lcd_string, DVcTxt1_1);
    ui_pic(DVcRzImg2_1, 0);
    lcd_check_sys_vol();
    lcd_disp_rec_opt_state();
    lcd_check_echo_sw();
}


///------------rtc,alarm set-display--------------
void lcd_rtc_set(void)
{
    RTC_SETTING *rtc_var;
    RTC_TIME *ui_time_var;
    _lcd_area_size_t *area_pt;
    const char lcd_string[] = {"时钟设置"};

//    puts("lcd_rtc_set start\n");
    lcd_disp_text(lcd_string, DVcTxt1_5);
    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if(rtc_var)
    {
        ui_time_var = rtc_var->calendar_set.curr_rtc_time;
        ui_time(DVcTime1_5,(TIME *)ui_time_var,-1,-1,0);
        ui_time(DVcTime2_5,(TIME *)ui_time_var,-1,-1,0);

        if(rtc_var->calendar_set.coordinate <= COORDINATE_MAX)
        {
            area_pt = (_lcd_area_size_t *)&time_set_backgroup[rtc_var->calendar_set.coordinate];
            //        printf("coordinate,%d\n",rtc_var->calendar_set.coordinate);
            lcd_TurnPixelReverse_Rect(area_pt->left,area_pt->top,area_pt->right,area_pt->bottom);
        }
    }
//    puts("lcd_rtc_set end\n");

}

///------------rtc,alarm set-display--------------
void lcd_alarm_set(void)
{

#if RTC_ALM_EN

    RTC_SETTING *rtc_var;
    RTC_TIME *ui_time_var;
    const char lcd_string[] = {"闹钟设置"};

//    puts("lcd_alarm_set start\n");

    lcd_disp_text(lcd_string, DVcTxt1_5);
    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if(rtc_var)
    {
        ui_time_var = rtc_var->alarm_set.curr_alm_time;
        ui_time(DVcTime1_5,(TIME *)ui_time_var,-1,-1,0);
        ui_time(DVcTime2_5,(TIME *)ui_time_var,-1,-1,0);


        if(rtc_var->alarm_set.alarm_sw != 0)
        {
            ui_pic(DVcRzImg1_5, 0);
        }

        if(rtc_var->alarm_set.coordinate<= COORDINATE_MAX)
        {
            _lcd_area_size_t *area_pt = (void*)&time_set_backgroup[rtc_var->alarm_set.coordinate];
            //        printf("coordinate,%d\n",rtc_var->alarm_set.coordinate);
            lcd_TurnPixelReverse_Rect(area_pt->left,area_pt->top,area_pt->right,area_pt->bottom);
        }
    }
//    puts("lcd_alarm_set end\n");
#endif

}


///------------rtc,alarm set-display--------------
void lcd_alarm_ring(void)
{
#if RTC_ALM_EN
    RTC_SETTING *rtc_var;
    RTC_TIME *ui_time_var;
    const char lcd_string[] = {"闹钟响铃"};
    const GUI_POINT  ring_pic_point = {21,40};

    //puts("lcd_alarm ring start\n");

    lcd_disp_text(lcd_string, DVcTxt1_5);
    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if(rtc_var)
    {
        ui_time_var = rtc_var->alarm_set.curr_alm_time;
        //printf("ui_alarm:%d-%d-%d,%d:%d:%d\n",ui_time_var->dYear,ui_time_var->bMonth,ui_time_var->bDay,\
        //ui_time_var->bHour,ui_time_var->bMin,ui_time_var->bSec);

        ui_time(DVcTime2_5,(TIME *)ui_time_var,-1,-1,0);
        ui_pic(DVcRzImg1_5, 0);
        ui_pic(DVcRzImg1_5, (GUI_POINT*)&ring_pic_point);
        lcd_check_sys_vol();
    }
#endif
}


#endif/*LCD_128x64_EN*/



