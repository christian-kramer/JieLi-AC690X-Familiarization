/*--------------------------------------------------------------------------*/
/**@file    fm_api.c
   @brief   FM 模式功能接口函数
   @details
   @author  guowei
   @date    2014-9-15
   @note    BC51
*/
/*----------------------------------------------------------------------------*/
#include "sdk_cfg.h"

#if FM_RADIO_EN

#include "fm/fm_radio.h"
#include "fm/fm_api.h"
#include "common/msg.h"
#include "dac/dac.h"
#include "rtos/os_api.h"
#include "vm/vm_api.h"
#include "fm_prompt.h"
#include "dac/ladc.h"
#include "fm_ui.h"
#include "clock_api.h"
#include "vm/vm.h"
/* #include "sys_cfg_api.h" */
#include "common/common.h"
#include "record.h"
#include "encode/encode.h"
#include "dac/src_api.h"
#include "ui/ui_api.h"


#if	RDA5807
    #include "RDA5807.h"
#endif

#if	BK1080
    #include "BK1080.h"
#endif

#if	QN8035
    #include "QN8035.h"
#endif

#if	FM_INSIDE
    #include "fm_inside.h"
#endif

extern RECORD_OP_API * rec_fm_api;
/**************2016-12-13***********************/
#ifdef SD0_IIC_MULT_EN
extern void close_sd0_controller(void);
#endif
/**********************************************/


void (* const fm_arr_init[])(void)  =
{

#if	RDA5807
    rda5807_init,
#endif

#if	BK1080
    bk1080_init,
#endif

#if	QN8035
    qn8035_init,
#endif

#if	FM_INSIDE
    fm_inside_init,
#endif

};

bool (* const fm_arr_set_fre[])(u16) =
{

#if	RDA5807
    rda5807_set_fre,
#endif

#if	BK1080
    bk1080_set_fre,
#endif

#if	QN8035
    qn8035_set_fre,
#endif

#if	FM_INSIDE
    fm_inside_set_fre,
#endif

};


void (* const fm_arr_powerdown[])(void)=
{

#if	RDA5807
    rda5807_powerdown,
#endif

#if	 BK1080
    bk1080_powerdown,
#endif

#if	QN8035
    qn8035_powerdown,
#endif

#if	FM_INSIDE
    fm_inside_powerdown,
#endif

};


bool (* const fm_arr_read_id[])(void)=
{

#if	RDA5807
    rda5807_read_id,
#endif

#if	 BK1080
    bk1080_read_id,
#endif

#if	QN8035
    qn8035_read_id,
#endif

#if	FM_INSIDE
    fm_inside_read_id,
#endif

};


void (* const fm_arr_mute[])(u8)=
{

#if	RDA5807
    rda5807_mute,
#endif

#if	BK1080
    bk1080_mute,
#endif

#if	QN8035
    qn8035_mute,
#endif

#if	FM_INSIDE
    fm_inside_mute,
#endif

};

/*----------------------------------------------------------------------------*/
/**@brief  FM变量初始化函数
   @param  无
   @return 无
   @note   void fm_drv_open(void)
*/
/*----------------------------------------------------------------------------*/
void fm_arg_open(void)
{
    fm_mode_var = (FM_MODE_VAR *)malloc(sizeof(FM_MODE_VAR));
    if(!fm_mode_var)
    {
        fm_puts("fm_open_err-1\n");
        while(1);
    }
    memset(fm_mode_var, 0x00, sizeof(FM_MODE_VAR));

    fm_info = (FM_INFO *)malloc(sizeof(FM_INFO));
    if(!fm_info)
    {
        fm_puts("fm_open_err-2\n");
        while(1);
    }
    memset(fm_info, 0x00, sizeof(FM_INFO));

    fm_mode_var->fm_rec_op = &rec_fm_api;
	fm_mode_var->fm_mute = 1;
}

/*----------------------------------------------------------------------------*/
/**@brief  FM变量初始化函数
   @param  无
   @return 无
   @note   void fm_open(void)
*/
/*----------------------------------------------------------------------------*/
void fm_arg_close(void)
{
    free_fun((void **)&fm_mode_var);
    free_fun((void **)&fm_info);
}

/*----------------------------------------------------------------------------*/
/**@brief   FM模块初始化接口函数
   @param
   @return
   @note    void init_fm_rev(void)
*/
/*----------------------------------------------------------------------------*/
bool fm_mode_init(void)
{

    for (fm_mode_var->bAddr = 0; fm_mode_var->bAddr < (sizeof(fm_arr_read_id)/sizeof(fm_arr_read_id[0])); fm_mode_var->bAddr++)
    {
        if ((*fm_arr_read_id[fm_mode_var->bAddr])())
        {
            (* fm_arr_init[fm_mode_var->bAddr])();
            return TRUE;
        }
    }

    return FALSE;
}

/*----------------------------------------------------------------------------*/
/**@brief   关闭FM模块电源
   @param
   @return
   @note    void fm_mode_powerdown(void
*/
/*----------------------------------------------------------------------------*/
void fm_mode_powerdown(void)
{
    (* fm_arr_powerdown[fm_mode_var->bAddr])();
}

/*----------------------------------------------------------------------------*/
/**@brief   FM模块Mute开关
   @param   flag：Mute使能位 0：开声音  1：关声音
   @return  无
   @note    void fm_module_mute(u8 flag)
*/
/*----------------------------------------------------------------------------*/
void fm_module_mute(u8 flag)
{
    fm_mode_var->fm_mute = flag;
    (* fm_arr_mute[fm_mode_var->bAddr])(flag);
}

/*----------------------------------------------------------------------------*/
/**@brief   设置一个FM频点的接口函数
   @param   mode = 0: 使用frequency中的值，= 1:频点加1， = 2:频点减1
   @return  1：有台；0：无台
   @note    bool fm_module_set_fre(u16 fre, u8 mode)
*/
/*----------------------------------------------------------------------------*/
bool fm_module_set_fre(u8 mode)
{
    fm_module_mute(1);

	vm_check_all(0);

    if (mode == FM_FRE_INC)
    {
        fm_mode_var->wFreq++;
    }
    else if (mode == FM_FRE_DEC)
    {
        fm_mode_var->wFreq--;
    }
    if (fm_mode_var->wFreq > MAX_FRE)
        fm_mode_var->wFreq = MIN_FRE;
    if (fm_mode_var->wFreq < MIN_FRE)
        fm_mode_var->wFreq = MAX_FRE;

    return (* fm_arr_set_fre[fm_mode_var->bAddr])(fm_mode_var->wFreq);
}

/*----------------------------------------------------------------------------*/
/**@brief   设置一个FM频点的接口函数
   @param   freq:875-1080
   @return  NULL
   @note    void fm_test_set_freq(u16 freq)
*/
/*----------------------------------------------------------------------------*/
void fm_test_set_freq(u16 freq)
{
	u8 ret;
    ret =  (* fm_arr_set_fre[fm_mode_var->bAddr])(freq);
	printf("fm_test_set_freq:%d = %d\n",freq, ret);
}

/*----------------------------------------------------------------------------*/
/**@brief    全频段搜索
   @param    mode: 0全频段, 1:上一个有效频点， 2:下一个有效频点
   @return   0:未找到频点 1:搜索完成 2:退出FM模式
   @note     bool fm_radio_scan(u8 mode)
*/
/*----------------------------------------------------------------------------*/
bool fm_radio_scan(u8 mode)
{
    bool tres;

    if (mode == FM_SCAN_PREV)
        tres = fm_module_set_fre(FM_FRE_DEC);
    else if((mode == FM_SCAN_NEXT)||(mode == FM_SCAN_ALL))
        tres = fm_module_set_fre(FM_FRE_INC);
    else    //搜台状态错误
        return FALSE;

    fm_mode_var->wFreChannel = 0;

    //全频搜索频点显示
    SET_UI_LOCK(MENU_FM_DISP_FRE);
#if LED_7_EN
    UI_menu(MENU_FM_DISP_FRE);
#endif // LED_7_EN
#if LCD_128X64_EN
	if(MENU_FM_MAIN == UI_GET_CUR_MENU())
	{
		UI_menu(MENU_REFRESH);
	}
#endif // LCD_128X64_EN
    if (tres)						//找到一个台
    {
        if(fm_mode_var->fm_mute)
        {
            fm_module_mute(0);
        }
        fm_info->dat[FM_FRE] = fm_mode_var->wFreq - MIN_FRE;
        //fm_save_info();
        save_fm_point(fm_mode_var->wFreq - MIN_FRE);
        fm_mode_var->wFreChannel = get_channel_via_fre(fm_mode_var->wFreq - MIN_FRE);
        fm_mode_var->wTotalChannel = get_total_mem_channel();

        //显示搜到的频道
        SET_UI_LOCK(MENU_FM_FIND_STATION);
#if LED_7_EN
        UI_menu(MENU_FM_FIND_STATION);
#endif // LED_7_EN
#if LCD_128X64_EN
        UI_menu(MENU_REFRESH);
#endif // LCD_128X64_EN
        return TRUE;
    }

    return FALSE;
}

/*----------------------------------------------------------------------------*/
/**@brief  FM 模式信息初始化
   @param  无
   @return 无
   @note   void fm_info_init(void)
*/
/*----------------------------------------------------------------------------*/
static void fm_info_init(void)
{
    fm_mode_var->scan_mode = FM_ACTIVE;

    fm_read_info();

    fm_mode_var->wFreq = fm_info->dat[FM_FRE];
    if(fm_mode_var->wFreq > (MAX_FRE - MIN_FRE))
    {
        fm_mode_var->wFreq = MIN_FRE;
    }
    else
    {
        fm_mode_var->wFreq += MIN_FRE;
    }

    fm_mode_var->wTotalChannel = get_total_mem_channel();
    if(!fm_mode_var->wTotalChannel)
    {
        fm_mode_var->wFreq = 875;
        fm_mode_var->wFreChannel = 0;
        fm_mode_var->wTotalChannel = 0;
        fm_module_set_fre(FM_CUR_FRE);
        fm_module_mute(0);
        fm_mode_var->scan_mode = FM_SCAN_STOP;
		return;
    }

    fm_mode_var->wFreChannel = fm_info->dat[FM_CHAN];
    if(fm_mode_var->wFreChannel > MAX_CHANNL)					//台号为1;总台数为1
    {
        fm_mode_var->wFreChannel = 1;
        fm_mode_var->wTotalChannel = 1;
    }
    else if(!fm_mode_var->wFreChannel)
    {
        fm_mode_var->wFreChannel = 1;
    }

    fm_mode_var->wFreChannel = get_channel_via_fre(fm_mode_var->wFreq - MIN_FRE);
    if(0x00 == fm_mode_var->wFreChannel)
    {
        puts("no channel\n");
        /* fm_mode_var->wFreChannel = 1; */
        /* save_fm_point(fm_mode_var->wFreq - MIN_FRE); */
        /* fm_save_info(); */
    }

    fm_module_set_fre(FM_CUR_FRE);
    fm_module_mute(0);
    fm_mode_var->scan_mode = FM_SCAN_STOP;

    /*---------FM MAIN UI--------------*/
//    set_ui_main(MENU_FM_MAIN);
//    ui_menu_api(MENU_FM_MAIN);
}

/*--------------------------------2016-11-29--------------------------------------------*/
void FmVolume ( u8 volume )
{
#ifdef SD0_IIC_MULT_EN
    u8 vol_temp = 0;
    if ( volume == 0 )
    {
        fm_module_mute(1);
        return;
    }
    else if(volume<=2)
        fm_module_mute(0);

    vol_temp = ( volume / 2 ) & 0x0F;
#if	QN8035
        QND_SetVol ( vol_temp );
#endif
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief  FM 模式关闭总接口
   @param  无
   @return 无
   @note   void fm_radio_powerdown(void)
*/
/*----------------------------------------------------------------------------*/
void fm_radio_powerdown(void)
{
    fm_mode_powerdown();
    fm_arg_close();
}


/*----------------------------------------------------------------------------*/
/**@brief  FM 模式初始化总接口
   @param  无
   @return 无
   @note   void fm_radio_init(void)
*/
/*----------------------------------------------------------------------------*/
u8 fm_radio_init(void)
{
/****************2016-12-13********************/
#ifdef SD0_IIC_MULT_EN
		close_sd0_controller();
		os_time_dly(5);
#endif
/**********************************************/
    if(fm_mode_init())
    {
        fm_info_init();
        return TRUE;
    }
    else
    {
        fm_radio_powerdown();
        return FALSE;
    }
}

#endif
