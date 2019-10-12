#ifndef _UI_API_H_
#define _UI_API_H_

#include "sdk_cfg.h"
#include "dec/decoder_phy.h"

#define UI_TASK_NAME    "UI_TASK_HSECOND"

#define _MENU_LIST_SUPPORT_      (1)

#define UI_POST_MSG_FLAG   (0xF123)

//宏定义
//1、打印控制

#define UI_DEBUG
#ifdef UI_DEBUG
    #define ui_puts         puts
    #define ui_printf       printf
#else
    #define ui_puts(...)
    #define ui_printf(...)
#endif

#define UI_NO_ARG   (-1)

//2、显示控制
#define UI_RETURN       5

#if UI_ENABLE//0//
    #define UI_menu(x)              ui_menu_reflash(x)//ui_menu_api(x)
    #define UI_menu_arg(x,y)    ui_menu_arg_do(x,y)
    #define UI_menu_mux(x,y)    ui_menu_spec(x,y)
    #define SET_UI_MAIN(x)       {UI_var.bMainMenu = x;UI_var.rec_opt_state = 0;}//ui_menu_main(x)//
    #define SET_UI_LOCK(x)       UI_var.bLockMenu = x
    #define SET_UI_UNLOCK(x)    UI_var.bLockMenu = 0
    #define SET_UI_BUF_ADR(x)   UI_var.ui_buf_adr = x
    #define SET_UI_BUF_LEN(x)   UI_var.ui_buf_len = x
    #define SET_UI_SYS_VOL(x)   UI_var.sys_vol= x
    #define SET_UI_REC_OPT(x)   UI_var.rec_opt_state = x
    #define SET_UI_ECHO_PT(x)   UI_var.echo_pt = (void*)x
	#define UI_GET_CUR_MENU()   ui_menu_get_arg(1)

#else
    #define UI_menu(...)
    #define UI_menu_arg(...)
    #define UI_menu_mux(...)
    #define SET_UI_MAIN(...)
    #define SET_UI_LOCK(...)
    #define SET_UI_UNLOCK(...)
    #define SET_UI_BUF_ADR(...)
    #define SET_UI_BUF_LEN(...)
    #define SET_UI_SYS_VOL(...)
    #define SET_UI_REC_OPT(...)
    #define SET_UI_ECHO_PT(...)
	#define UI_GET_CUR_MENU()   

#endif/*UI_ENABLE*/


enum
{
    MENU_POWER_UP = 0,
    MENU_WAIT,
    MENU_BT_MAIN,
    MENU_PC_MAIN,
    MENU_PC_VOL_UP,
    MENU_PC_VOL_DOWN,
    MENU_AUX_MAIN,
    MENU_ALM_UP,
    MENU_DEV_ERR,
	/***以上数据对应menu_string数组，顺序不能修改***/

//    MENU_PLAY,
//    MENU_PLAYMODE,
//    MENU_MAIN_VOL,
//    MENU_EQ,

    MENU_MAIN_VOL,
    MENU_SET_EQ,
    MENU_SET_PLAY_MODE,

    MENU_NOFILE,
    MENU_NODEVICE,
    MENU_PLAY_TIME,
    MENU_FILENUM,
    MENU_INPUT_NUMBER,
    MENU_MUSIC_MAIN,
    MENU_MUSIC_PAUSE,

    MENU_FM_MAIN,
    MENU_FM_DISP_FRE,
    MENU_FM_FIND_STATION,
    MENU_FM_CHANNEL,

    MENU_USBREMOVE,
    MENU_SDREMOVE,
    MENU_SCAN_DISK,

    MENU_NOP,
    MENU_NUM,

    MENU_RTC_MAIN,
    MENU_RTC_SET,
    MENU_RTC_PWD,
    MENU_ALM_SET,

    MENU_BT_SEARCH_DEVICE,
    MENU_BT_CONNECT_DEVICE,
    MENU_BT_DEVICE_ADD,
    MENU_BT_DEVICE_NAME,

    BLUETOOTH_SHOW_DEVICE_ADDR_NAME_DEFAULT,
    BLUETOOTH_CLEAR_LCD,
    BLUETOOTH_TEST_A2DP,
    BLUETOOTH_TEST_HFP,
    BLUETOOTH_TEST_HID,
    BLUETOOTH_TEST_OFFSET,
    BLUETOOTH_VALUE_OFFSET,

    MENU_RECODE_MAIN,
    MENU_RECODE_ERR,

    MENU_LIST_DISPLAY,

    MENU_200MS_REFRESH = 0x80,
    MENU_100MS_REFRESH,
    MENU_HALF_SEC_REFRESH,
    MENU_REFRESH,
    MENU_POWER_DOWN,
    MENU_SPEC,
    MENU_MAIN = 0xFF,
};

enum
{
    UI_MSG_NON = 0,
    UI_MSG_DIS_MAIN,
    UI_MSG_REFLASH,
    UI_MSG_OTHER,
    UI_MSG_MENULIST,

    UI_MSG_MENULIST_KEYBACK = 0x10,
    UI_MSG_MENULIST_KEYMODE,
    UI_MSG_MENULIST_KEYPREV,
    UI_MSG_MENULIST_KEYNEXT,
    UI_MSG_MENULIST_KEYEXIT,

    UI_MSG_NULL = 0xff,
};

enum
{
    UI_REC_OPT_STOP = 0,
    UI_REC_OPT_START,
    UI_REC_OPT_PAUSE,
    UI_REC_OPT_NODEV,
    UI_REC_OPT_ERR,
};

enum
{
    UI_MENU_LIST_MAIN = 0,
    UI_MENU_LIST_ITEM,
    UI_MENU_LIST_BROWSER,
};

typedef struct _UI_VAR
{
    u8  bCurMenu;
    u8  bMainMenu;
    u8  bMenuReturnCnt;
    u8  bLockMenu;
    void * ui_buf_adr;//UI_显示buf，第一个u32 必须是显示类型
    u32 ui_buf_len;
    u8  *sys_vol;
    int *echo_pt;
    u8  rec_opt_state;
    u8  param_menu;
}UI_VAR;
#if LCD_SEG_EN
	typedef struct _UI_DISP_API
	{
	    ///--------common------///
		void (*ui_string_menu)(void);
	    void (*ui_IR_number)(s32);
		void (*ui_vol_set)(s32 vol);
		void (*ui_clear)(void);
		void (*ui_wait)(void);

	    ///--------music------///
		void (*ui_music_main)(void);
		void (*ui_eq_set)(s32);
		void (*ui_file_num)(void);
		void (*ui_play_mode)(s32);

	    ///--------fm------///
		void (*ui_FM_main)(void);
		void (*ui_FM_channel)(void);

	    ///--------rtc------///
		void (*ui_RTC_main)(void);
		void (*ui_RTC_set)(void);
		void (*ui_ALM_set)(void);

	    ///--------rec------///
		void (*ui_REC_main)(void);

		///--------aux------///
		void (*ui_AUX_main)(void);
	    ///--------bt------///
	    void (*ui_BT_main)(void);
	    ///--------pc------///
	    void (*ui_PC_main)(u8);
	    void (*ui_menu_list)(u32);
	}UI_DISP_API;
#else
	typedef struct _UI_DISP_API
	{
	    ///--------common------///
	    void (*ui_string_menu)(u8);
	    void (*ui_IR_number)(s32);
		void (*ui_vol_set)(s32 vol);
		void (*ui_clear)(void);

    ///--------music------///
	void (*ui_music_main)(void);
	void (*ui_eq_set)(s32);
	void (*ui_file_num)(void);
	void (*ui_play_mode)(s32);

    ///--------fm------///
	void (*ui_FM_main)(void);
	void (*ui_FM_channel)(void);

    ///--------rtc------///
	void (*ui_RTC_main)(void);
	void (*ui_RTC_set)(void);
	void (*ui_ALM_set)(void);

    ///--------rec------///
	void (*ui_REC_main)(void);

	///--------aux------///
    void (*ui_AUX_main)(u8);

    ///--------bt------///
    void (*ui_BT_main)(u8);

    ///--------pc------///
    void (*ui_PC_main)(u8);

    void (*ui_menu_list)(u32);

	}UI_DISP_API;
#endif
//全局变量声明
extern UI_VAR _data UI_var;

//函数声明
extern void set_LED_fade_out(void);
extern void set_LED_all_on(void);
extern void ui_init_api(void);
extern void ui_menu_api(u8 menu,s32 arg);
extern u32 ui_menu_reflash(u8 menu);
extern u32 ui_menu_arg_do(u8 menu,u32 arg);
extern u32 ui_menu_spec(u8 NewMenu,u32 CurMenu);
extern u32 ui_menu_get_arg(u8 type);

extern int ui_menu_check_msg(int *msg);
extern void ui_keymenu_register(char *task_name,u8 *ad_key_t,u8 *io_key_t, u8 *ir_key_t,u8 *touch_key_t);

#endif  /*  _LED_UI_API_H_  */
