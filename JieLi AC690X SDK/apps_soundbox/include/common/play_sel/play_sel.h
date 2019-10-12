#ifndef _PLAY_SEL_H_
#define _PLAY_SEL_H_

#include "includes.h"
#include "sdk_cfg.h"
#include "bluetooth/avctp_user.h"
#include "dec/music_api.h"

//#define DEC_SEL_DEBUG
#ifdef DEC_SEL_DEBUG
	#define dsel_printf printf
	#define dsel_puts puts
#else
	#define dsel_printf(...)
	#define dsel_puts(...)
#endif


#define BT_CFG_FILE             "bt_cfg.bin"  /*蓝牙配置文件默认放在第一个文件，与库对应，不可修改成其它值*/
#define EQ_CFG_FILE             "eq_cfg.bin"


#define BPF_BT_MP3              "/bt.***"
#define BPF_PC_MP3              "/pc.***"
#define BPF_RADIO_MP3           "/radio.***"
#define BPF_LINEIN_MP3          "/linein.***"
#define BPF_MUSIC_MP3           "/music.***"
#define BPF_RTC_MP3             "/rtc.***"
#define BPF_PC_MP3              "/pc.***"
#define BPF_ECHO_MP3            "/echo.***"
#define BPF_REC_MP3             "/record.***"

#define BPF_WAIT_MP3            "/wait.***"
#define BPF_POWER_OFF_MP3       "/power_off.***"
#define BPF_POWER_ON_MP3       "/power_up.***"
#define BPF_CONNECT_MP3         "/connect.***"
#define BPF_CONNECT_LEFT_MP3   	"/connect.***" /// "conleft.***"
#define BPF_CONNECT_RIGHT_MP3  	"/connect.***" /// "conrigh.***"
#define BPF_CONNECT_ONLY_MP3   	"/connect.***" /// "cononly.***"
#define BPF_DISCONNECT_MP3      "/disconnect.***"
#define BPF_RING_MP3            "/ring.***"

#define BPF_0_MP3               "/0.***"
#define BPF_1_MP3               "/1.***"
#define BPF_2_MP3               "/2.***"
#define BPF_3_MP3               "/3.***"
#define BPF_4_MP3               "/4.***"
#define BPF_5_MP3               "/5.***"
#define BPF_6_MP3               "/6.***"
#define BPF_7_MP3               "/7.***"
#define BPF_8_MP3               "/8.***"
#define BPF_9_MP3               "/9.***"
#define BPF_SD_MP3       		"/sd.***"
#define BPF_USB_MP3       		"/usb.***"
#define BPF_MAX_VOL_MP3         "/max_vol.***"

#define BPF_LOW_POWER_MP3       "/low_bat.***"
#define BPF_MUSIC_PLAY_MP3      "/test.***"
#define BPF_MUSIC_PAUSE_MP3     "/test.***"
#define BPF_TEST_MP3       		"/test.***"

typedef struct __PLAY_SEL_PARA //para
{
    u32 max_file_number;
    u32 rpt_time;
    u32 delay;
    u32 *file_number;       //文件号播放
    void **file_name;       //文件名播放
    char let;       //文件名播放
} _PLAY_SEL_PARA;
/////*/

typedef struct __PLAY_SEL_API
{
    u32 device_sel;
    _PLAY_SEL_PARA *file_par;
    tbool busy;
    tbool alive;
    void *father_name;
} _PLAY_SEL_API;

tbool play_sel(void *name,u32 device_sel,_PLAY_SEL_PARA *file_par);
tbool play_sel_busy(void);
tbool play_sel_stop(void);
void play_sel_play_decoder(_MUSIC_API *mapi);
void play_sel_pause_decoder(_MUSIC_API *mapi);
tbool tone_play(void *task_name, u32 file_num);
// tbool tone_play_by_name(void *task_name, void * file_name);
tbool tone_play_by_name(void *task_name, u32 total_file, ...);
void * tone_number_get_name(u32 number);
#endif
