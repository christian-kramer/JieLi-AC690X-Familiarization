/*--------------------------------------------------------------------------*/
/**@file    LED.c
   @brief   LED 模块驱动接口函数
   @details
   @author  bingquan Cai
   @date    2012-8-30
   @note    AC319N
*/
/*----------------------------------------------------------------------------*/
#include "ui/ui_api.h"
#include "sys_detect.h"


#if UI_ENABLE

#include "ui/ui_common.h"
#include "ui/led/led7_drv.h"
#include "fm_radio.h"
#include "dec/decoder_phy.h"
#include "music_ui.h"
#include "music.h"
#include "timer.h"
#include "key_drv/key.h"
#include "file_operate/file_op.h"
//#include "play_file.h"
#include "dac/dac.h"
#include "rtc_setting.h"
#include "bluetooth/avctp_user.h"

#if REC_EN
    #include "encode/encode.h"
#endif

#if LED_7_EN
LED7_VAR led7_var;

const  u8 LED_NUMBER[10] =
{
    /*0*/
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_E | LED_F),
    /*1*/
    (u8)(LED_B | LED_C),
    /*2*/
    (u8)(LED_A | LED_B | LED_D | LED_E | LED_G),
    /*3*/
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_G),
    /*4*/
    (u8)(LED_B | LED_C | LED_F | LED_G),
    /*5*/
    (u8)(LED_A | LED_C | LED_D | LED_F | LED_G),
    /*6*/
    (u8)(LED_A | LED_C | LED_D | LED_E | LED_F | LED_G),
    /*7*/
    (u8)(LED_A | LED_B | LED_C),
    /*8*/
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_E | LED_F | LED_G),
    /*9*/
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_F | LED_G),
};

const  u8 LED_LARGE_LETTER[26] =
{
    0x77,0x40,0x39,0x3f,0x79,///<ABCDE
    0x71,0x40,0x76,0x06,0x40,///<FGHIJ
    0x40,0x38,0x40,0x37,0x3f,///<KLMNO
    0x73,0x40,0x50,0x6d,0x78,///<PQRST
    0x3e,0x3e,0x40,0x76,0x40,///<UVWXY
    0x40///<Z
};

const  u8 LED_SMALL_LETTER[26] =
{
    0x77,0x7c,0x58,0x5e,0x79,///<abcde
    0x71,0x40,0x40,0x40,0x40,///<fghij
    0x40,0x38,0x40,0x54,0x5c,///<klmno
    0x73,0x67,0x50,0x40,0x78,///<pqrst
    0x3e,0x3e,0x40,0x40,0x40,///<uvwxy
    0x40///<z
};

/* 通过测试 7断的真值表
   0    1    2     3     4     5     6
0  X    1A   1B    1E    SD   播放   X
1  1F   X    2A    2B    2E   2D     X
2  1G   2F   X     :     3B   ||     MP3
3  1C   2G   3F    X     3C   4E     X
4  1D   2C   3G    3A    X    4C     4G
5  3D   U    3E    4D    4F   X      4B
6  X    X    MHz    X    X    4A     X
*/
/*
// 7断数码管转换表
const u8 led_7[35][2] =
{
  {0,1},//1A
  {0,2},//1B
  {3,0},//1C
  {4,0},//1D
  {0,3},//1E
  {1,0},//1F
  {2,0},//1G

  {1,2},//2A
  {1,3},//2B
  {4,1},//2C
  {1,5},//2D
  {1,4},//2E
  {2,1},//2F
  {3,1},//2G

  {4,3},//3A
  {2,4},//3B
  {3,4},//3C
  {5,0},//3D
  {5,2},//3E
  {3,2},//3F
  {4,2},//3G

  {6,5},//4A
  {5,6},//4B
  {4,5},//4C
  {5,3},//4D
  {3,5},//4E
  {5,4},//4F
  {4,6},//4G

  {0,5},//LED_PLAY
  {2,5},//LED_PAUSE
  {5,1},//LED_USB
  {0,4},//LED_SD
  {2,3},//LED_2POINT
  {6,2},//LED_MHZ
  {2,6},//LED_MP3
};
*/
/***********新7段数码管真值表**************/
/* 0    1     2     3     4     5     6
0  X    2A    2B    2C    2D    2E    2F
1  1A   X     2G    :     .     MHz   MP3
2  1B   X     X     4A    4B    4C    4D
3  1C   PLAY  3A    X     4E    4F    4G
4  1D   PAUSE 3B    3E    X     X     X
5  1E   USB   3C    3F    X     X     X
6  1F   SD    3D    3G    X     X     X
*/
const u8 led_7[36][2] = ///< 新7断数码管转换表
{
  {1,0},//1A
  {2,0},//1B
  {3,0},//1C
  {4,0},//1D
  {5,0},//1E
  {6,0},//1F
  {2,1},//1G

  {0,1},//2A
  {0,2},//2B
  {0,3},//2C
  {0,4},//2D
  {0,5},//2E
  {0,6},//2F
  {1,2},//2G

  {3,2},//3A
  {4,2},//3B
  {5,2},//3C
  {6,2},//3D
  {4,3},//3E
  {5,3},//3F
  {6,3},//3G

  {2,3},//4A
  {2,4},//4B
  {2,5},//4C
  {2,6},//4D
  {3,4},//4E
  {3,5},//4F
  {3,6},//4G

  {3,1},//LED_PLAY
  {4,1},//LED_PAUSE
  {5,1},//LED_USB
  {6,1},//SD
  {1,3},//:
  {1,5},//MHz
  {1,4},//.
  {1,6},//MP3
};

//按位于 查表
const u8 bit_table[8] =
{
  0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,
};

const u8 playmodestr[][5] =
{
    " ALL",
    "Fold",
    " ONE",
    " rAn",
};

const u8 menu_string[][5] =
{
    " HI ",
    "Lod ",
    "bLUE",
    " PC ",
    " UP ",
    " dN ",
    " AUX",
    "-AL-"
};
const u8 other_string[][5]=
{
    "Eq ",
    "V ",
    "P ",
    " NOP",
    " rec",
};

/*----------------------------------------------------------------------------*/
/**@brief   Music模式 设备显示
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_dev(void)
*/
/*----------------------------------------------------------------------------*/
void led7_show_dev(u8 dev)
{
//    /*Music Device type*/
    /* MUSIC_DIS_VAR *music_var; */
    /* music_var = (MUSIC_DIS_VAR*)UI_var.ui_buf_adr; */

    /* if(music_var) */
    {
        if((dev == SD0_DEVICE)||(dev == SD1_DEVICE))
        {
            LED_STATUS |= LED_SD;
        }
        else if(dev == USB_DEVICE)
        {
            LED_STATUS |= LED_USB;
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   led7_drv 状态位缓存清除函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_clear_icon(void)
*/
/*----------------------------------------------------------------------------*/
void led7_clear_icon(void)
{
    led7_var.bFlashChar = 0;
    led7_var.bFlashIcon = 0;
    led7_var.bShowBuff[4] = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   led7_drv 显示坐标设置
   @param   x：显示横坐标
   @return  void
   @author  Change.tsai
   @note    void led7_setX(u8 X)
*/
/*----------------------------------------------------------------------------*/
void led7_setX(u8 X)
{
    led7_var.bCoordinateX = X;
}

/*----------------------------------------------------------------------------*/
/**@brief   LED 清屏函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_null(void)
*/
/*----------------------------------------------------------------------------*/
void led7_show_null(void)
{
    led7_clear_icon();
    led7_var.bShowBuff[0] = 0;
    led7_var.bShowBuff[1] = 0;
    led7_var.bShowBuff[2] = 0;
    led7_var.bShowBuff[3] = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   led7_drv 扫描函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_init(void)
*/
/*----------------------------------------------------------------------------*/
void led7_init(void)
{
    led7_clear();
    s32 ret;
	ret = timer_reg_isr_fun(timer0_hl,1,led7_scan,NULL);
	if(ret != TIMER_NO_ERR)
	{
		printf("led7_scan err = %x\n",ret);
	}
}

/*----------------------------------------------------------------------------*/
/**@brief   led7_drv 单个字符显示函数
   @param   chardata：显示字符
   @return  void
   @author  Change.tsai
   @note    void led7_show_char(u8 chardata)
*/
/*----------------------------------------------------------------------------*/
void led7_show_char(u8 chardata)
{
    //必须保证传入的参数符合范围，程序不作判断
    //if ((chardata < ' ') || (chardata > '~') || (led7_var.bCoordinateX > 4))
    //{
    //    return;
    //}
    if ((chardata >= '0') && (chardata <= '9'))
    {
        led7_var.bShowBuff[led7_var.bCoordinateX++] = LED_NUMBER[chardata - '0'];
    }
    else if ((chardata >= 'a') && (chardata <= 'z'))
    {
        led7_var.bShowBuff[led7_var.bCoordinateX++] = LED_SMALL_LETTER[chardata - 'a'];
    }
    else if ((chardata >= 'A') && (chardata <= 'Z'))
    {
        led7_var.bShowBuff[led7_var.bCoordinateX++] = LED_LARGE_LETTER[chardata - 'A'];
    }
    else if (chardata == ':')
    {
        LED_STATUS |= LED_2POINT;
    }
    else if (chardata == ' ')
    {
        led7_var.bShowBuff[led7_var.bCoordinateX++] = 0;
    }
    else //if (chardata == '-')     //不可显示
    {
        led7_var.bShowBuff[led7_var.bCoordinateX++] = BIT(6);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   led7_drv 字符串显示函数
   @param   *str：字符串的指针   offset：显示偏移量
   @return  void
   @author  Change.tsai
   @note    void led7_show_string(u8 *str)
*/
/*----------------------------------------------------------------------------*/
void led7_show_string(u8 *str)
{
    while (0 != *str)
    {
        led7_show_char(*str++);
    }
}

/*----------------------------------------------------------------------------*/
/** @brief:
    @param:
    @return:
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
void led7_show_string_menu(u8 menu)
{

    if(menu >= (sizeof(menu_string)/5))
    {
        printf("*strid(%d) is over!\n",menu);
    }
    else
    {
        led7_show_string((u8 *)menu_string[menu]);
    }
}

void led7_show_linin_main(u8 menu)
{
    led7_show_string_menu(menu);

#if REC_EN

    RECORD_OP_API * rec_var_p;
    REC_CTL * rec_ctl_var;

    rec_var_p = *(RECORD_OP_API **)UI_var.ui_buf_adr;
    if((UI_var.ui_buf_adr)&&(rec_var_p))
    {
        rec_ctl_var = rec_var_p->rec_ctl;

        LED_STATUS &= ~(LED_PLAY | LED_PAUSE);
        LED_STATUS &= ~(LED_SD|LED_USB);

        if((menu == MENU_AUX_MAIN)||(menu == MENU_BT_MAIN))
        {
            if((rec_ctl_var)&&(ENC_STOP !=rec_ctl_var->enable))
            {
                if (ENC_STAR == rec_ctl_var->enable)
                    LED_STATUS |= LED_PLAY;
                else if(ENC_PAUS == rec_ctl_var->enable)
                    LED_STATUS |= LED_PAUSE;

				led7_show_dev(rec_ctl_var->curr_device);
            }
        }
    }
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief   Music 播放文件号显示函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_filenumber(void)
*/
/*----------------------------------------------------------------------------*/
void led7_show_filenumber(void)
{
    MUSIC_DIS_VAR *music_var;

    music_var = (MUSIC_DIS_VAR*)UI_var.ui_buf_adr;

    if(music_var)
    {
        /*Music File Number info*/
        //printf("ui_music.ui_play_file_num = %u----\n",ui_music.ui_play_file_num);
        itoa4(music_var->ui_curr_file);
        led7_show_string((u8 *)bcd_number);
    }
}


/*----------------------------------------------------------------------------*/
/**@brief   红外输入文件号显示函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_IR_number(void)
*/
/*----------------------------------------------------------------------------*/
void led7_show_IR_number(s32 arg)
{
    u16 ir_num;
    ir_num = (u16)(arg&0xffff);
    /*IR File Number info*/
    itoa4(ir_num);
    led7_show_string((u8 *)bcd_number);
}

/*----------------------------------------------------------------------------*/
/**@brief   Music模式 显示界面
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_music_main(void)
*/
/*----------------------------------------------------------------------------*/
void led7_show_music_main(void)
{
    //led7_show_string((u8*)"MUSI");
    u32 play_time;
    MUSIC_DIS_VAR *music_var;

    music_var = (MUSIC_DIS_VAR*)UI_var.ui_buf_adr;

    if(music_var)
    {
        /*Music Play time info*/
        play_time = music_var->play_time;

        itoa2(play_time/60);
        led7_show_string((u8 *)bcd_number);

        itoa2(play_time%60);
        led7_show_string((u8 *)bcd_number);

        /* led7_show_dev(); */
		led7_show_dev(music_var->ui_curr_device);

        LED_STATUS |= LED_2POINT | LED_MP3;

        if (*music_var->ui_curr_statu == DECODER_PLAY)
        {
            LED_STATUS |= LED_PLAY;
            LED_STATUS &= ~LED_PAUSE;
            led7_var.bFlashIcon |= LED_2POINT;
        }
        else if (*music_var->ui_curr_statu == DECODER_PAUSE)
        {
            LED_STATUS &= ~LED_PLAY;
            LED_STATUS |= LED_PAUSE;
        }
    //
    //    if(mute)
    //    {
    //        led7_var.bFlashIcon |= LED_MP3;
    //        led7_var.bFlashChar = 0x0f;
    //    }
    //    else
    //    {
    //        led7_var.bFlashIcon &= ~LED_MP3;
    //        led7_var.bFlashChar = 0x00;
    //    }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   EQ显示函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_eq(s32 arg)
*/
/*----------------------------------------------------------------------------*/
void led7_show_eq(s32 arg)
{
    u8 eq_cnt;
    eq_cnt = (u8)arg;
    led7_show_string((u8 *)other_string[0]);
    led7_show_char(eq_cnt%10 + '0');
}

/*----------------------------------------------------------------------------*/
/**@brief   循环模式显示函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_playmode(s32 arg)
*/
/*----------------------------------------------------------------------------*/
void led7_show_playmode(s32 arg)
{
    u8 pm_cnt;
    pm_cnt = (u8)arg;
    led7_show_string((u8 *)&playmodestr[pm_cnt-FOP_MAX-1][0]);
}

/*----------------------------------------------------------------------------*/
/**@brief   音量显示函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_volume(s32 vol)
*/
/*----------------------------------------------------------------------------*/
void led7_show_volume(s32 vol)
{
    u8 tmp_vol;

    tmp_vol = (u8)vol;
    led7_show_string((u8 *)other_string[1]);
    itoa2(tmp_vol);
    led7_show_string((u8 *)bcd_number);
}

/*----------------------------------------------------------------------------*/
/**@brief   FM 模式主界面
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_fm_main(void)
*/
/*----------------------------------------------------------------------------*/
void led7_show_fm_main(void)
{
    /*FM - Frequency*/
 #if 1
    FM_MODE_VAR * fm_var;

    if(!UI_var.ui_buf_adr)
        return;

    fm_var = *(FM_MODE_VAR **)UI_var.ui_buf_adr;

    if(fm_var)
    {
        itoa4(fm_var->wFreq);

        if (fm_var->wFreq <= 999)
            bcd_number[0] = ' ';

        led7_show_string((u8 *)bcd_number);
        LED_STATUS |= LED_MHZ;

#if REC_EN
        RECORD_OP_API * rec_var_p;
        REC_CTL * rec_ctl_var;

        if((fm_var->fm_rec_op)&&(*(RECORD_OP_API **)(fm_var->fm_rec_op)))
        {
            rec_var_p = *(RECORD_OP_API **)fm_var->fm_rec_op;
            rec_ctl_var = rec_var_p->rec_ctl;

            if((rec_ctl_var)&&(ENC_STOP != rec_ctl_var->enable))
            {
				LED_STATUS &= ~(LED_PLAY | LED_PAUSE);
				LED_STATUS &= ~(LED_SD|LED_USB);

                /* led7_show_dev(); */
				led7_show_dev(rec_ctl_var->curr_device);
                if (ENC_STAR == rec_ctl_var->enable)
                    LED_STATUS |= LED_PLAY;
                else if(ENC_PAUS == rec_ctl_var->enable)
                    LED_STATUS |= LED_PAUSE;
            }

        }
#endif
    }

#endif // 0
}

/*----------------------------------------------------------------------------*/
/**@brief   FM 模式主界面
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_fm_station(void)
*/
/*----------------------------------------------------------------------------*/
void led7_show_fm_station(void)
{
//    /*FM - Station*/
#if 1
    FM_MODE_VAR * fm_var;

    if(!UI_var.ui_buf_adr)
        return;

    fm_var = *(FM_MODE_VAR **)UI_var.ui_buf_adr;

    if(fm_var)
    {
        led7_show_string((u8 *)other_string[2]);
        itoa2(fm_var->wFreChannel);
        led7_show_string((u8 *)bcd_number);
    }
#endif
}

#if 1 //RTC_CLK_EN
/*----------------------------------------------------------------------------*/
/**@brief   RTC 显示界面
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_RTC_main(void)
*/
/*----------------------------------------------------------------------------*/
void led7_show_RTC_main(void)
{
    RTC_SETTING * rtc_var;
    RTC_TIME *ui_time;

    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if(rtc_var)
    {
        ui_time = rtc_var->calendar_set.curr_rtc_time;

        itoa2(ui_time->bHour);
        led7_show_string((u8 *)bcd_number);
        itoa2(ui_time->bMin);
        led7_show_string((u8 *)bcd_number);

        led7_var.bFlashIcon |= LED_2POINT;
        LED_STATUS |= LED_2POINT;

        if(rtc_var->rtc_set_mode == RTC_SET_MODE)
        {
            if(rtc_var->calendar_set.coordinate == COORDINATE_MIN)
                led7_var.bFlashChar = BIT(0)|BIT(1);
            else
                led7_var.bFlashChar = BIT(2)|BIT(3);
        }
        else
            led7_var.bFlashChar = 0;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   Alarm 显示界面
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_alarm(void)
*/
/*----------------------------------------------------------------------------*/
#if 1 //RTC_ALM_EN
void led7_show_alarm(void)
{
    RTC_SETTING * rtc_var;
    RTC_TIME *ui_time;
    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if(rtc_var)
    {
        puts("led7_show_alarm\n");

        ui_time = rtc_var->alarm_set.curr_alm_time;
        itoa2(ui_time->bHour);
        led7_show_string((u8 *)bcd_number);
        itoa2(ui_time->bMin);
        led7_show_string((u8 *)bcd_number);

        LED_STATUS |= LED_2POINT;

        if(rtc_var->rtc_set_mode == ALM_SET_MODE)
        {
            if(rtc_var->alarm_set.coordinate == COORDINATE_MIN)
                led7_var.bFlashChar = BIT(0)|BIT(1);
            else
                led7_var.bFlashChar = BIT(2)|BIT(3);
        }

        /*Alarm info - Switch On/Off*/
        if (rtc_var->alarm_set.alarm_sw)
        {
            LED_STATUS |= LED_PLAY;
        }
        else
        {
            LED_STATUS |= LED_PAUSE;
        }
    }
}
#endif
#endif

#if REC_EN
/*----------------------------------------------------------------------------*/
/**@brief   REC 显示界面
   @param   void
   @return  void
   @note    void led7_show_rec_start(void)
*/
/*----------------------------------------------------------------------------*/
static void led7_show_rec_start(REC_CTL * rec_ctl_var)
{
    u32 rec_time;

//    LED_STATUS &= ~(LED_PLAY | LED_PAUSE);
//    if(rec_ctl_var)
    {
        rec_time = rec_ctl_var->file_info.enc_time_cnt;

        itoa2(rec_time/60);
        led7_show_string((u8 *)bcd_number);

        itoa2(rec_time%60);
        led7_show_string((u8 *)bcd_number);

        /* led7_show_dev(); */
		led7_show_dev(rec_ctl_var->curr_device);

        LED_STATUS |= LED_2POINT; //| LED_PLAY;

        if (ENC_STAR == rec_ctl_var->enable)
            LED_STATUS |= LED_PLAY;
        else if (ENC_PAUS == rec_ctl_var->enable)
            LED_STATUS |= LED_PAUSE;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   REC 显示界面
   @param   void
   @return  void
   @note    void led7_show_rec_main(void)
*/
/*----------------------------------------------------------------------------*/
void led7_show_rec_main(void)
{
    RECORD_OP_API * rec_var_p;
    REC_CTL * rec_ctl_var;

    LED_STATUS &= ~(LED_PLAY | LED_PAUSE);
    LED_STATUS &= ~LED_2POINT; //| LED_PLAY;
    LED_STATUS &= ~(LED_SD|LED_USB);

    if(UI_var.ui_buf_adr)
    {
        rec_var_p = *(RECORD_OP_API **)UI_var.ui_buf_adr;
        if(rec_var_p)
        {
            rec_ctl_var = rec_var_p->rec_ctl;
            if((rec_ctl_var)&&(ENC_STOP != rec_ctl_var->enable))
            {
                led7_show_rec_start(rec_ctl_var);
                return;
            }
        }
    }

    led7_show_string((u8 *)other_string[4]);
}
#endif

void LED_drive7(void)
{
    u8 k,i,j,temp;
    k=0;

    //led7_var.bShowBuff[0]=0xff;
    //led7_var.bShowBuff[1]=0xff;
    //led7_var.bShowBuff[2]=0xff;
    //led7_var.bShowBuff[3]=0xff;
    //led7_var.bShowBuff[4]=0xff;

    led7_var.bShowBuff1[0]=0;
    led7_var.bShowBuff1[1]=0;
    led7_var.bShowBuff1[2]=0;
    led7_var.bShowBuff1[3]=0;
    led7_var.bShowBuff1[4]=0;
    led7_var.bShowBuff1[5]=0;
    led7_var.bShowBuff1[6]=0;

    for(i=0;i<5;i++)
    {
        temp = led7_var.bShowBuff[i];
        if (get_sys_halfsec())
        {
            if ((led7_var.bFlashIcon) && (i == 4))
            {
                temp = LED_STATUS & (~led7_var.bFlashIcon);
            }
            else if (led7_var.bFlashChar & BIT(i))
            {
                temp = 0x0;
            }
        }

        for(j=0;j<7;j++)
        {
            if(temp & bit_table[j])
            {
                led7_var.bShowBuff1[led_7[k][0]] |= bit_table[led_7[k][1]];
            }
            k++;
        }
        if(j == 7)
        {
            if(temp & bit_table[j])
                led7_var.bShowBuff1[led_7[35][0]] |= bit_table[led_7[35][1]];
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   LED清屏函数
   @param   x：显示横坐标
   @return  void
   @author  Change.tsai
   @note    void led7_clear(void)
*/
/*----------------------------------------------------------------------------*/
void led7_clear(void)
{
    LEDN_PORT_OUT &= ~(BIT(LEDN_S0_BIT)|BIT(LEDN_S1_BIT)|BIT(LEDN_S2_BIT)|BIT(LEDN_S3_BIT)|BIT(LEDN_S6_BIT));
    LEDN_PORT_DIR |=  (BIT(LEDN_S0_BIT)|BIT(LEDN_S1_BIT)|BIT(LEDN_S2_BIT)|BIT(LEDN_S3_BIT)|BIT(LEDN_S6_BIT));
    LEDN_PORT_PU  &= ~(BIT(LEDN_S0_BIT)|BIT(LEDN_S1_BIT)|BIT(LEDN_S2_BIT)|BIT(LEDN_S3_BIT)|BIT(LEDN_S6_BIT));
    LEDN_PORT_PD  &= ~(BIT(LEDN_S0_BIT)|BIT(LEDN_S1_BIT)|BIT(LEDN_S2_BIT)|BIT(LEDN_S3_BIT)|BIT(LEDN_S6_BIT));
    LEDN_PORT_HD  &= ~(BIT(LEDN_S0_BIT)|BIT(LEDN_S1_BIT)|BIT(LEDN_S2_BIT)|BIT(LEDN_S3_BIT)|BIT(LEDN_S6_BIT));

	LEDN1_PORT_OUT &= ~(BIT(LEDN_S4_BIT)|BIT(LEDN_S5_BIT));
	LEDN1_PORT_DIR |=  (BIT(LEDN_S4_BIT)|BIT(LEDN_S5_BIT));
	LEDN1_PORT_PU  &= ~(BIT(LEDN_S4_BIT)|BIT(LEDN_S5_BIT));
	LEDN1_PORT_PD  &= ~(BIT(LEDN_S4_BIT)|BIT(LEDN_S5_BIT));
	LEDN1_PORT_HD  &= ~(BIT(LEDN_S4_BIT)|BIT(LEDN_S5_BIT));
}

/*----------------------------------------------------------------------------*/
/**@brief   LED扫描函数
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_scan(void *param)
*/
/*----------------------------------------------------------------------------*/
void led7_scan(void *param)
{
    static u8 cnt;
    u8 seg;

    LED_drive7();

    seg = led7_var.bShowBuff1[cnt];

    led7_clear();

    switch(cnt)
    {
    case 0:
        LEDN_PORT_OUT |=  BIT(LEDN_S0_BIT);
        LEDN_PORT_HD  |=  BIT(LEDN_S0_BIT);
        LEDN_PORT_DIR &= ~BIT(LEDN_S0_BIT);
        break;

    case 1:
        LEDN_PORT_OUT |=  BIT(LEDN_S1_BIT);
        LEDN_PORT_HD  |=  BIT(LEDN_S1_BIT);
        LEDN_PORT_DIR &= ~BIT(LEDN_S1_BIT);
        break;

    case 2:
        LEDN_PORT_OUT |=  BIT(LEDN_S2_BIT);
        LEDN_PORT_HD  |=  BIT(LEDN_S2_BIT);
        LEDN_PORT_DIR &= ~BIT(LEDN_S2_BIT);
        break;

    case 3:
        LEDN_PORT_OUT |=  BIT(LEDN_S3_BIT);
        LEDN_PORT_HD  |=  BIT(LEDN_S3_BIT);
        LEDN_PORT_DIR &= ~BIT(LEDN_S3_BIT);
        break;

    case 4:
        LEDN1_PORT_OUT |=  BIT(LEDN_S4_BIT);
        LEDN1_PORT_HD  |=  BIT(LEDN_S4_BIT);
        LEDN1_PORT_DIR &= ~BIT(LEDN_S4_BIT);
        break;

    case 5:
        LEDN1_PORT_OUT |=  BIT(LEDN_S5_BIT);
        LEDN1_PORT_HD  |=  BIT(LEDN_S5_BIT);
        LEDN1_PORT_DIR &= ~BIT(LEDN_S5_BIT);
        break;

    case 6:
        LEDN_PORT_OUT |=  BIT(LEDN_S6_BIT);
        LEDN_PORT_HD  |=  BIT(LEDN_S6_BIT);
        LEDN_PORT_DIR &= ~BIT(LEDN_S6_BIT);
        break;

    default :
        break;
    }

    if(seg & BIT(0))
    {
        LEDN_PORT_OUT &= ~BIT(LEDN_S0_BIT);
        LEDN_PORT_DIR &= ~BIT(LEDN_S0_BIT);
    }

    if(seg & BIT(1))
    {
        LEDN_PORT_OUT &= ~BIT(LEDN_S1_BIT);
        LEDN_PORT_DIR &= ~BIT(LEDN_S1_BIT);
    }

    if(seg & BIT(2))
    {
        LEDN_PORT_OUT &= ~BIT(LEDN_S2_BIT);
        LEDN_PORT_DIR &= ~BIT(LEDN_S2_BIT);
    }

    if(seg & BIT(3))
    {
        LEDN_PORT_OUT &= ~BIT(LEDN_S3_BIT);
        LEDN_PORT_DIR &= ~BIT(LEDN_S3_BIT);
    }

    if(seg & BIT(4))
    {
       LEDN1_PORT_OUT &= ~BIT(LEDN_S4_BIT);
       LEDN1_PORT_DIR &= ~BIT(LEDN_S4_BIT);
    }

    if(seg & BIT(5))
    {
        LEDN1_PORT_OUT &= ~BIT(LEDN_S5_BIT);
        LEDN1_PORT_DIR &= ~BIT(LEDN_S5_BIT);
    }

    if(seg & BIT(6))
    {
        LEDN_PORT_OUT &= ~BIT(LEDN_S6_BIT);
        LEDN_PORT_DIR &= ~BIT(LEDN_S6_BIT);
    }
    cnt = (cnt >= 7) ? 0 : cnt+1;
}
#endif
#if LCD_SEG_EN
LCD_SEG_VAR LCD_SEG_var;
extern tu8 power_level;
extern u16 freq;
extern u8 _xdata fm_am_sw_set;
extern bool bt_flash;
extern bool fm_mute_flag;
bool bt_flash_flag=1;
const u8  LCD_SEG_NUMBER[] =
{
    /*0*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGE | SEGF),
    /*1*/
    (u8)(SEGB | SEGC),
    /*2*/
    (u8)(SEGA | SEGB | SEGD | SEGE | SEGG),
    /*3*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGG),
    /*4*/
    (u8)(SEGB | SEGC | SEGF | SEGG),
    /*5*/
    (u8)(SEGA | SEGC | SEGD | SEGF | SEGG),
    /*6*/
    (u8)(SEGA | SEGC | SEGD | SEGE | SEGF | SEGG),
    /*7*/
    (u8)(SEGA | SEGB | SEGC),
    /*8*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGE | SEGF | SEGG),
    /*9*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
};

const u8  LCD_SEG_SMALL_NUMBER[] =
{
    /*a*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGE | SEGG),
    /*b*/
    (u8)(SEGC | SEGD | SEGE | SEGF | SEGG),
    /*c*/
    (u8)(SEGA | SEGD | SEGE | SEGF),
    /*d*/
    (u8)(SEGB | SEGC | SEGD | SEGE | SEGG),
    /*e*/
    (u8)(SEGB | SEGC | SEGD | SEGE | SEGG),
    /*f*/
    (u8)(SEGA | SEGC | SEGD | SEGF | SEGG),
    /*g*/
    (u8)(SEGA | SEGC | SEGD | SEGE | SEGF | SEGG),
    /*h*/
    (u8)(SEGA | SEGB | SEGC),
    /*i*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGE | SEGF | SEGG),
    /*j*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*k*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*l*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*m*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*n*/
    (u8)(SEGC | SEGE | SEGG),
    /*o*/
    (u8)(SEGC | SEGD | SEGE | SEGG),
    /*p*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*q*/
    (u8)(SEGA | SEGB | SEGC | SEGF | SEGG),
    /*r*/
    (u8)(SEGE | SEGG),
    /*s*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*t*/
    (u8)(SEGD | SEGE | SEGF | SEGG),
    /*u*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*v*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*w*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*x*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*y*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*z*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
};

const u8  LCD_SEG_LARGE_NUMBER[] =
{
    /*A*/
    (u8)(SEGA | SEGB | SEGC | SEGE | SEGF | SEGG),
    /*B*/
    (u8)(SEGB | SEGC),
    /*C*/
    (u8)(SEGA | SEGD | SEGE | SEGF),
    /*D*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGG),
    /*E*/
    (u8)(SEGA | SEGD | SEGE | SEGF | SEGG),
    /*F*/
    (u8)(SEGA | SEGG  | SEGF | SEGE),
    /*G*/
    (u8)(SEGA | SEGC | SEGD | SEGE | SEGF | SEGG),
    /*H*/
    (u8)(SEGB | SEGC | SEGG| SEGE | SEGF),
    /*I*/
    (u8)(SEGB | SEGC),
    /*J*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*K*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*L*/
    (u8)(SEGD | SEGE | SEGF),
    /*M*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*N*/
    (u8)(SEGA | SEGB | SEGC | SEGE | SEGF),
    /*O*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGE | SEGF),
    /*P*/
    (u8)(SEGA | SEGB | SEGE | SEGF | SEGG),
    /*Q*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*R*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*S*/
    (u8)(SEGA | SEGF | SEGG | SEGC | SEGD ),
    /*T*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*U*/
    (u8)(SEGB | SEGC | SEGD | SEGE | SEGF),
    /*V*/
    (u8)(SEGB | SEGC | SEGD | SEGE | SEGF),
    /*W*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*X*/
    (u8)(SEGB | SEGC | SEGG| SEGE | SEGF),
    /*Y*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
    /*Z*/
    (u8)(SEGA | SEGB | SEGC | SEGD | SEGF | SEGG),
};
const u8  LCD_SEG_NUM_TABLE[4][7] =
{
#if 1
//	1A--1G
	0x39,0x29,0x19,0x09,0x14,0x34,0x24,
//	2A-2G
	0x3b,0x2b,0x1b,0x0b,0x1a,0x3a,0x2a,
//	3A-3G
	0x3e,0x2e,0x1e,0x0e,0x10,0x30,0x20,
//	4A-4G
	0x31,0x21,0x11,0x01,0x1f,0x3f,0x2f,
#else
//	1A--1G
	0x39,0x29,0x19,0x09,0x14,0x34,0x24,
//	2A-2G
	0x3b,0x2b,0x1b,0x0b,0x1a,0x3a,0x2a,
//	3A-3G
	0x3e,0x2e,0x1e,0x0e,0x10,0x30,0x20,
//	4A-4G
	0x31,0x21,0x11,0x01,0x1f,0x3f,0x2f,
#endif
};
const u8  LCD_SEG_ICON_TABLE[] =
{
	LCD_ICON_USB_Y1, 
	LCD_ICON_SD_Y2,	
	LCD_ICON_P1_Y3,
	LCD_ICON_MUTE_Y4,	
	LCD_ICON_REC_Y5,	
	LCD_ICON_BT_Y6,	
	LCD_ICON_MHZ_Y7,	
	LCD_ICON_COL,	
	LCD_ICON_FM_Y8,	
};
const u8 menu_string[][5] =
{
    " HI ",
    "Lod ",
    "bLUE",
    " PC ",
    " UP ",
    " dN ",
    " AUX",
    "-AL-"
};

const u8 playmodestr[][5] =
{
    " ALL",
    "Fold",
    " ONE",
    " rAn",
};
/*----------------------------------------------------------------------------*/
void LCD_show_string_menu(u8 menu)
{

    if(menu >= (sizeof(menu_string)/5))
    {
        printf("*strid(%d) is over!\n",menu);
    }
    else
    {
        LCD_SEG_show_string((u8 *)menu_string[menu]);
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   LCD
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_init(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_icon(u8 icon)
{
	LCD_SEG_var.bHardBuff[icon>>0x04]|=1<<(icon&0x0f);
}
/*----------------------------------------------------------------------------*/
void   LCD_SEG_clear_icon(u8 icon)
{
	LCD_SEG_var.bHardBuff[icon>>0x04]&=~(1<<(icon&0x0f));
}
/*----------------------------------------------------------------------------*/
void   LCD_SEG_clear_all_icon(void)
{
	tu8 itemp;
	for(itemp=0x00;itemp<sizeof(LCD_SEG_ICON_TABLE);itemp++)
		LCD_SEG_clear_icon(LCD_SEG_ICON_TABLE[itemp]);
}
/*----------------------------------------------------------------------------*/
void   LCD_SEG_clear(void)
{
	my_memset((u8 *)&LCD_SEG_var,0x00,sizeof(LCD_SEG_VAR));
}
/*----------------------------------------------------------------------------*/
void   LCD_SEG_init(void)
{
	JL_CLOCK->CLK_CON1 &= ~(BIT(9)|BIT(8));    //[5:4]:OSC

	JL_LCDC->CON0 = 0x0007<<4;  //[6:4]:VLCDS 3.0V
	JL_LCDC->CON0 |= BIT(7);	 //[7]:32KHZ/64
	JL_LCDC->CON0 |= 0x0001<<12;  //[13:12]:CHGMOD ?????
	JL_LCDC->CON0 |= 0x0002<<2;  //[3:2]:0x01:1/2 Bias?0x02:1/3?0x03:1/4
	JL_LCDC->CON0 &= ~BIT(15);
	JL_LCDC->CON0 |= BIT(14);	//com 5 pc5-pc1
	
	JL_LCDC->SEG_IOEN0 = 0xCE18;  //PA3-PA4,PA9-PA11,PA14-PA15
	
	JL_LCDC->SEG_IOEN1 = 0x0003;  //PC0  PC1
    COM0_SEG(0x0000);
    COM1_SEG(0x0000);
    COM2_SEG(0x0000);
    COM3_SEG(0x0000);
//    COM4_SEG(0x0000);
	JL_LCDC->CON0 |= 0x01 ;  //[0] LCDCEN ??????
	LCD_SEG_clear();
	LCD_SEG_setX(0);
}
/*----------------------------------------------------------------------------*/
/**@brief   LCD_SEG 
   @param   x:
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_setX(u8 X)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_setX(u8 X)
{
    LCD_SEG_var.bCoordinateX = X;
}
/*----------------------------------------------------------------------------*/
void   LCD_SEG_clear_flash(void)
{
    LCD_SEG_var.bFlashChar = 0;
    LCD_SEG_var.bFlashIcon = 0;
}
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_null(void)
{
    LCD_SEG_clear();
}
/*----------------------------------------------------------------------------*/
/**@brief   LCD_SEG 
   @param   chardata:
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_char(u8 chardata)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_char(u8 chardata)
{
    if ((chardata < ' ') || (chardata > '~') || (LCD_SEG_var.bCoordinateX > 4))
    {
        return;
    }
    if ((chardata >= '0') && (chardata <= '9'))
    {
        LCD_SEG_var.bShowBuff[LCD_SEG_var.bCoordinateX++] = LCD_SEG_NUMBER[chardata - '0'];
    }
    else if ((chardata >= 'a') && (chardata <= 'z'))
    {
        LCD_SEG_var.bShowBuff[LCD_SEG_var.bCoordinateX++] = LCD_SEG_SMALL_NUMBER[chardata - 'a'];
		//puts("run to here \n");
        //printf("LCD_SEG_var.bShowBuff[%d]= %x\n",LCD_SEG_var.bCoordinateX-1,LCD_SEG_var.bShowBuff[LCD_SEG_var.bCoordinateX-1]);
    }
    else if ((chardata >= 'A') && (chardata <= 'Z'))
    {
        LCD_SEG_var.bShowBuff[LCD_SEG_var.bCoordinateX++] = LCD_SEG_LARGE_NUMBER[chardata - 'A'];
        //puts("run to here \n");
        //printf("LCD_SEG_var.bShowBuff[%d]= %x\n",LCD_SEG_var.bCoordinateX-1,LCD_SEG_var.bShowBuff[LCD_SEG_var.bCoordinateX-1]);
    }
  //  else if (chardata == '1BC')
  //  {
//			LCD_SEG_show_icon(LCD_ICON_1BC);
//    }
    else if (chardata == ' ')
    {
        LCD_SEG_var.bShowBuff[LCD_SEG_var.bCoordinateX++] = 0;
//        puts("--------------- kong ge----------- \n");
//        printf("LCD_SEG_var.bShowBuff[%d]= %x\n",LCD_SEG_var.bCoordinateX-1,LCD_SEG_var.bShowBuff[LCD_SEG_var.bCoordinateX-1]);
    }
    else if (chardata == '-')
    {
        LCD_SEG_var.bShowBuff[LCD_SEG_var.bCoordinateX++] = SEGG;
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   LCD_SEG
   @param   chardata:
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_number(u8 number)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_number(u8 number)
{
    LCD_SEG_show_char(number + '0');
}
/*----------------------------------------------------------------------------*/
/**@brief   LCD_SEG
   @param   *str:  offset:
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_string(u8 *str)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_string(u8 *str)
{
    while (0 != *str)
    {
        LCD_SEG_show_char(*str++);
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   LED5X7 (Hi)
   @param   *str:
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_Hi(u8 *str)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_Hi(void)
{
    LCD_SEG_show_string((u8 *)" HI ");
}
/*----------------------------------------------------------------------------*/
/**@brief   
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_waiting(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_waiting(void)
{
	LCD_SEG_clear_all_icon();
    LCD_SEG_show_string((u8 *)"----");
}
/*----------------------------------------------------------------------------*/
void  LCD_SEG_show_Idle(void)
{
    LCD_SEG_show_string((u8 *)"    ");
}
/*----------------------------------------------------------------------------*/
/**@brief
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_freq(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_freq(u16 freq)
{
#if FM_RADIO_EN

   /*FM - Frequency*/
   	LCD_SEG_clear_all_icon();
    itoa4(fm_mode_var->wFreq);
    LCD_SEG_show_string((u8 *)bcd_number);
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_freq_setting(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_freq_setting(u16 freq)
{
    LCD_SEG_var.bFlashChar = 0x0f;
    LCD_SEG_show_freq(freq);
}
/*----------------------------------------------------------------------------*/
/**@brief
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_volume(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_volume(s32 vol)
{
    LCD_SEG_clear_all_icon();
    LCD_SEG_show_char(' ');
    LCD_SEG_show_char('V');
    LCD_SEG_show_number(vol/10);
    LCD_SEG_show_number(vol%10);
}
/*----------------------------------------------------------------------------*/
/**@brief   Music
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_dev(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_dev(void)
{
    /*Music Device type*/
     LCD_SEG_clear_all_icon();
    if ((music_ui.ui_curr_device == SD0_DEVICE)||(music_ui.ui_curr_device == SD1_DEVICE))
    {
		LCD_SEG_show_icon(LCD_ICON_SD_Y2);
    }
    else if (music_ui.ui_curr_device == USB_DEVICE)
    {
		LCD_SEG_show_icon(LCD_ICON_USB_Y1);
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   EQ
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LED7_DRIVE_show_eq(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_eq(s32 arg)
{
     LCD_SEG_clear_all_icon();
	switch(eq_mode)
	{
		case 0x00:			// NORMAL
			LCD_SEG_show_string((u8 *)" NOr");
			break;
		case 0x01:			// POP
			LCD_SEG_show_string((u8 *)" POP");
			break;
		case 0x02:			// ROCK
			LCD_SEG_show_string((u8 *)" rOC");
			break;
		case 0x03:			// JAZZ
			LCD_SEG_show_string((u8 *)" JAZ");
			break;
		case 0x04:			// CLASSIC
			LCD_SEG_show_string((u8 *)" CLA");
			break;
		case 0x05:			// COUNTRY
			LCD_SEG_show_string((u8 *)" COU");
			break;
		case 0x06:			// BASS
			LCD_SEG_show_string((u8 *)" bAS");
			break;
	}
   	//LCD_SEG_show_string((u8 *)" Eq");
	//LCD_SEG_show_number(eq_mode%10);
}
/*----------------------------------------------------------------------------*/
/**@brief
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_playmode(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_playmode(s32 arg)
{
    //printf("\n------play_mode == %d\n",play_mode);
	u8 pm_cnt;
    pm_cnt = (u8)arg;
    LCD_SEG_clear_all_icon();
   // LCD_SEG_show_string((u8 *)&playmodestr[play_mode-FOP_MAX-1][0]);
    LCD_SEG_show_string((u8 *)&playmodestr[pm_cnt-FOP_MAX-1][0]);
}
extern u32 get_decode_time(void);

void LCD_SEG_show_music_main(void)
{
    u32 play_time;
	MUSIC_DIS_VAR *music_var;
	music_var = (MUSIC_DIS_VAR*)UI_var.ui_buf_adr;
	LCD_SEG_clear_all_icon();
	//puts("\n-LCD_SEG_show_music_main-\n");
    /*Music Play time info*/
    if(music_var)
    {
        /*Music Play time info*/
        if (*music_var->ui_curr_statu == DECODER_PLAY)
        {
	        play_time = music_var->play_time;

	        itoa2(play_time/60);
	        LCD_SEG_show_string((u8 *)bcd_number);

	        itoa2(play_time%60);
	        LCD_SEG_show_string((u8 *)bcd_number);

	        /* led7_show_dev(); */
			LCD_SEG_show_dev();

			LCD_SEG_show_icon(LCD_ICON_COL);
        }
        else if (*music_var->ui_curr_statu == DECODER_PAUSE)
        {
			LCD_SEG_show_pause();
        }
    }
}
/*----------------------------------------------------------------------------*/
/**@brief
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_pause(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_pause(void)
{
    LCD_SEG_clear_all_icon();
	if(get_sys_halfsec())
			LCD_SEG_show_string((u8 *)"    ");
	else
    	LCD_SEG_show_string((u8 *)"PAUS");
}
/*----------------------------------------------------------------------------*/
/**@brief   Music
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_filenumber(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_filenumber(void)
{
    /*Music File Number info*/
    LCD_SEG_clear_all_icon();
    MUSIC_DIS_VAR *music_var;
    music_var = (MUSIC_DIS_VAR*)UI_var.ui_buf_adr;
	itoa4(music_var->ui_curr_file);
   	LCD_SEG_show_string((u8 *)bcd_number);
}
/*----------------------------------------------------------------------------*/
/**@brief
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_IR_number(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_IR_number(s32 ir_num)
{
    /*IR File Number info*/
	LCD_SEG_clear_all_icon();
    itoa4(ir_num);
    LCD_SEG_show_string((u8 *)bcd_number);
}
/*----------------------------------------------------------------------------*/
/**@brief   FM
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_fm_main(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_fm_main(void)
{
#if FM_RADIO_EN
  /*FM - Frequency*/
	LCD_SEG_clear_all_icon();
	FM_MODE_VAR * fm_var;

    if(!UI_var.ui_buf_adr)
        return;

    fm_var = *(FM_MODE_VAR **)UI_var.ui_buf_adr;

    if(fm_var)
    {
        itoa4(fm_var->wFreq);

        if (fm_var->wFreq <= 999)
            bcd_number[0] = ' ';

		LCD_SEG_show_string((u8 *)bcd_number);
		LCD_SEG_show_icon(LCD_ICON_MHZ_Y7);
    }
#endif
}
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_fm_wait(void)
{
#if FM_RADIO_EN
	LCD_SEG_clear_all_icon();
	LCD_SEG_show_string((u8 *)" Lod");
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief   FM
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_fm_main(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_fm_station(void)
{
#if FM_RADIO_EN
//    /*FM - Station*/
	FM_MODE_VAR * fm_var;
	
	if(!UI_var.ui_buf_adr)
		return;
	
	fm_var = *(FM_MODE_VAR **)UI_var.ui_buf_adr;
	
	if(fm_var)
	{
		LCD_SEG_show_string((u8 *)"P ");
		itoa2(fm_var->wFreChannel);
		LCD_SEG_show_string((u8 *)bcd_number);
	}
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief   AUX 
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_fm_main(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_aux_main(void)
{
//    /*FM - Station*/
//	LCD_STATUS1 |= ICON_AUX;
	LCD_SEG_clear_all_icon();
    LCD_SEG_show_string((u8 *)" AUX");
}
/*----------------------------------------------------------------------------*/
/**@brief   pc 
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_pc_main(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_pc_main(u8 menu)
{
    LCD_SEG_clear_all_icon();
    LCD_SEG_show_string((u8 *)" PC ");
}
/*----------------------------------------------------------------------------*/
/**@brief   pc
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_pc_vol_up(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_pc_vol_up(void)
{
    LCD_SEG_show_string((u8 *)" UP ");
}
/*----------------------------------------------------------------------------*/
/**@brief   pc 
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_pc_vol_down(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_pc_vol_down(void)
{
    LCD_SEG_show_string((u8 *)" dN ");
}
/*----------------------------------------------------------------------------*/
/**@brief   bt
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_bt_main(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_bt_main(void)
{
	LCD_SEG_clear_all_icon();
	if(bt_flash_flag==1)
	{
		if(get_sys_halfsec())
		{
			LCD_SEG_clear_icon(LCD_ICON_BT_Y6);
			LCD_SEG_show_string((u8 *)"    ");
		}
		else
		{
			LCD_SEG_show_icon(LCD_ICON_BT_Y6);
			LCD_SEG_show_string((u8 *)" bt ");
		}
	}
	else
	{
		LCD_SEG_show_icon(LCD_ICON_BT_Y6);
		LCD_SEG_show_string((u8 *)" bt ");
	}
}
/*----------------------------------------------------------------------------*/
/**@brief   bt
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_bt_idle(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_bt_idle(void)
{
    LCD_SEG_show_string((u8 *)" H ");
}
/*----------------------------------------------------------------------------*/
/**@brief   bt
   @param   void
   @return  void
   @author  Change.tsai
   @note    void LCD_SEG_show_bt_call(void)
*/
/*----------------------------------------------------------------------------*/
void   LCD_SEG_show_bt_call(void)
{
    LCD_SEG_show_string((u8 *)" CAL");
}
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_batval(void)
{
#if 0
	static bool bat_disp_flag;
	switch(power_level)
	{
		case 3:
			LCD_SEG_show_icon(LCD_ICON_BAT1);
			LCD_SEG_show_icon(LCD_ICON_BAT2);
			LCD_SEG_show_icon(LCD_ICON_BAT3);
			LCD_SEG_show_icon(LCD_ICON_BAT4);
			break;
		case 2:
			LCD_SEG_show_icon(LCD_ICON_BAT1);
			LCD_SEG_show_icon(LCD_ICON_BAT3);
			LCD_SEG_show_icon(LCD_ICON_BAT4);
			break;
		case 1:
			LCD_SEG_show_icon(LCD_ICON_BAT1);
			LCD_SEG_show_icon(LCD_ICON_BAT4);
			break;
		case 0:
			LCD_SEG_show_icon(LCD_ICON_BAT1);
			break;
		default:
			break;
	}
#endif
}
#if RTC_CLK_EN
/*----------------------------------------------------------------------------*/
/**@brief   RTC
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_RTC_main(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_RTC_main(void)
{
    RTC_SETTING * rtc_var;
    RTC_TIME *ui_time;

    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if(rtc_var)
    {
        ui_time = rtc_var->calendar_set.curr_rtc_time;

        itoa2(ui_time->bHour);
        LCD_SEG_show_string((u8 *)bcd_number);
        itoa2(ui_time->bMin);
        LCD_SEG_show_string((u8 *)bcd_number);

		LCD_SEG_show_icon(LCD_ICON_COL);

        if(rtc_var->rtc_set_mode == RTC_SET_MODE)
        {
            if(rtc_var->calendar_set.coordinate == COORDINATE_MIN)
        	{
               // led7_var.bFlashChar = BIT(0)|BIT(1);
        	}
            else
        	{
                //led7_var.bFlashChar = BIT(2)|BIT(3);
        	}
        }
        else
            led7_var.bFlashChar = 0;
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   Alarm
   @param   void
   @return  void
   @author  Change.tsai
   @note    void led7_show_alarm(void)
*/
/*----------------------------------------------------------------------------*/
#if RTC_ALM_EN
void LCD_SEG_show_alarm(void)
{
    RTC_SETTING * rtc_var;
    RTC_TIME *ui_time;
    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if(rtc_var)
    {
        puts("led7_show_alarm\n");

        ui_time = rtc_var->alarm_set.curr_alm_time;
        itoa2(ui_time->bHour);
        LCD_SEG_show_string((u8 *)bcd_number);
        itoa2(ui_time->bMin);
        LCD_SEG_show_string((u8 *)bcd_number);

		LCD_SEG_show_icon(LCD_ICON_COL);

        if(rtc_var->rtc_set_mode == ALM_SET_MODE)
        {
            if(rtc_var->alarm_set.coordinate == COORDINATE_MIN)
        	{
             //   led7_var.bFlashChar = BIT(0)|BIT(1);
        	}
            else
        	{
             //   led7_var.bFlashChar = BIT(2)|BIT(3);
        	}
        }
        /*Alarm info - Switch On/Off*/
        if (rtc_var->alarm_set.alarm_sw)
        {
			//LCD_SEG_show_icon(LCD_ICON_COL);
        }
        else
        {
			//LCD_SEG_show_icon(LCD_ICON_COL);
        }
    }
}
#endif
#endif
#if REC_EN
/*----------------------------------------------------------------------------*/
/**@brief   REC
   @param   void
   @return  void
   @note    void led7_show_rec_start(void)
*/
/*----------------------------------------------------------------------------*/
static void LCD_SEG_show_rec_start(REC_CTL * rec_ctl_var)
{
    u32 rec_time;

//    LED_STATUS &= ~(LED_PLAY | LED_PAUSE);
//    if(rec_ctl_var)
    {
        rec_time = rec_ctl_var->file_info.enc_time_cnt;

        itoa2(rec_time/60);
        LCD_SEG_show_string((u8 *)bcd_number);

        itoa2(rec_time%60);
        LCD_SEG_show_string((u8 *)bcd_number);
		LCD_SEG_show_dev();
		LCD_SEG_show_icon(LCD_ICON_COL);

        if (ENC_STAR == rec_ctl_var->enable)
    	{
		//	LCD_SEG_show_icon(LCD_ICON_REP);
    	}
        else if (ENC_PAUS == rec_ctl_var->enable)
    	{
			//LCD_SEG_show_icon(LCD_ICON_REP);
    	}
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   REC
   @param   void
   @return  void
   @note    void led7_show_rec_main(void)
*/
/*----------------------------------------------------------------------------*/
void LCD_SEG_show_rec_main(void)
{
    RECORD_OP_API * rec_var_p;
    REC_CTL * rec_ctl_var;
    LCD_SEG_clear_all_icon();   
    if(UI_var.ui_buf_adr)
    {
        rec_var_p = *(RECORD_OP_API **)UI_var.ui_buf_adr;
        if(rec_var_p)
        {
            rec_ctl_var = rec_var_p->rec_ctl;
            if((rec_ctl_var)&&(ENC_STOP != rec_ctl_var->enable))
            {
                LCD_SEG_show_rec_start(rec_ctl_var);
                return;
            }
        }
    }
    LCD_SEG_show_string((u8 *)" rEC");
}
#endif
/*----------------------------------------------------------------------------*/
void  LCD_SEG_scan(void *p)
{
    u8 i;
    u8 j;
	u8 temp;
   // u8 bufdat[7];
#if 0
//	COM0_SEG(0xCE1B);
	COM0_SEG(LCD_SEG_var.bHardBuff[0]);
	//COM0_SEG(0x0003);

	//COM1_SEG(0xCE1C);
	COM1_SEG(LCD_SEG_var.bHardBuff[3]);

//	COM2_SEG(0xCE1C);
//	COM3_SEG(0xCE1C);
//	COM4_SEG(0xCE1C);
#else
	for(i=0;i<4;i++)
	{
		temp = LCD_SEG_var.bShowBuff[i];
		for(j=0;j<7;j++)
		{
			if(LCD_SEG_NUM_TABLE[i][j]==0xff)
				continue;
			if(temp &(1<<j))
			{
				LCD_SEG_var.bHardBuff[LCD_SEG_NUM_TABLE[i][j]>>4]|= 1<<(LCD_SEG_NUM_TABLE[i][j]&0x0f);
			}
			else
			{
				LCD_SEG_var.bHardBuff[LCD_SEG_NUM_TABLE[i][j]>>4]&= ~(1<<(LCD_SEG_NUM_TABLE[i][j]&0x0f));
			}
		}
	}
      COM0_SEG(LCD_SEG_var.bHardBuff[0]);
	  COM1_SEG(LCD_SEG_var.bHardBuff[1]);
	  COM2_SEG(LCD_SEG_var.bHardBuff[2]);
	  COM3_SEG(LCD_SEG_var.bHardBuff[3]);
#endif
}
#endif

#endif
