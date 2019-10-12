/*
 *********************************************************************************************************
 *                                                AC46
 *                                      lcd ui menu display
 *                                             CODE
 *
 *                          (c) Copyright 2015-2016, ZHUHAI JIELI
 *                                           All Rights Reserved
 *
 * File : *
 * By   : jamin.li
 * DATE : 2015-10-08 build this file
 *********************************************************************************************************
 */


#ifndef _LCD_UI_MENU_H_
#define _LCD_UI_MENU_H_

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
// #include "uicon/ui_con1.h"
#include "fm/fm_radio.h"
#include "timer.h"
#include "key_drv/key.h"
#include "file_operate/file_op.h"
#include "dac/dac.h"
#include "rtc_setting.h"
#include "bluetooth/bt_ui.h"
#include "common/msg.h"
#include "rtos/task_manage.h"

#if REC_EN
#include "encode/encode.h"
#endif

#include "file_operate/file_bs_deal.h"

enum
{
    MENU_TASK_MAIN_APP = 0,
    MENU_TASK_RTC_APP,
    MENU_TASK_MUSIC_APP,
    MENU_TASK_RECORD_APP,
    MENU_TASK_RADIO_APP,
    MENU_TASK_LINEIN_APP,
    MENU_TASK_UDISK_APP,
    MENU_TASK_BTSTACK_APP,
    MENU_TASK_APP_MAX,
};


enum
{
    MENU_OPT_NOERR,
    MENU_OPT_HOTKEY,
    MENU_OPT_BACK,
    MENU_OPT_BACKMAIN, //返回主界面
    MENU_OPT_WAITING,   //继续等待操作
    MENU_OPT_CALLCBK,   // 回调函数
    MENU_OPT_ACTIVE_SCROLL, // 选中项滚动
};

typedef int (*item_cbk_f)(void *arg);
typedef void (*app_menulist_f)(u32 );

//#pragma pack (1) /*指定按1字节对齐*/

typedef struct
{
    u8    icon_a;       //选中图标
    u8    icon_b;       //未选中图标
    s8    choise_sn;    //单选菜单，选中项
    u8    reserver;
    s16   head_id;      //菜单头显示字符串id,-1 为没有标题
    u16   item_total;   //菜单项总数
    u16   item_active;  //活动菜单编号
    u16   item_cfg;     //配置菜单项，可配置回调方式，是否有菜单标题等
    u16   bs_total_dir; //浏览目录下的文件夹个数
    u16   reserver2;     //未用
    u8   *item_id_gp;      //菜单项字符串ID表
    const char *str_table; //字符串表
    void *item_cbk_gp;     //菜单选中回调函数表
} menu_list_t;

//#pragma pack () /*取消指定对齐，恢复缺省对齐*/

//item_cfg bit_map
#define ITEM_CFG_BIT_CBKGP           (1<<0) // 回调为数据标记
#define ITEM_CFG_BIT_ACTIVE_SCROLL   (1<<1) // 菜单支持选中滚动

//菜单返回项ID值
#define ITEM_BACK_FIRST_FIRST_ID     0
#define ITEM_BACK_NO_ITEM_ID         0xffff

#define MENU_LAYER_MAX      (MAX_DEEPTH+1)     //菜单控制最大层
#define MENU_ITEM_MAXLEN    14    //菜单项显示的最大长度
#define MENU_ITEM_STRLEN     16    //菜单项在字符表的长度，包含结束符
#define MENU_HEAD_STRLEN    16    //菜单头在字符表的长度，包含结束符

#define MENU_ITEMS_SCREEN        3     //每屏的菜单项个数
#define MENU_MAIN_SCREEN         4     //每屏的图标个数
#define MENU_BROWSER_SCREEN  4     //每屏的文件项个数

#define MENU_BROWSER_STRLEN  128  //每条信息显示的长度

#define MENU_TEXT_SCROLL_TIME                  1  //滚动间隔时间，单位:500MS
#define MENU_TEXT_SCROLL_CHAR                 1  //每次滚动的字符个数
#define MENU_TEXT_SCROLL_MAXCNT            (64)  //连续滚动最大次数，认为超时退出

#define MENU_WAIT_TIME_MAX                 (MENU_TEXT_SCROLL_TIME * MENU_TEXT_SCROLL_MAXCNT) //单位:500MS

extern void enter_menulist_display(u32 arg);

#endif/*_LCD_UI_MENU_H_*/
