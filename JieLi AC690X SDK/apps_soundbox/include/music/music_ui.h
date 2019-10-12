#ifndef _MUSIC_UI_H_
#define _MUSIC_UI_H_

#include "fat/tff.h"
#include "dec/music_api.h"
#include "sdk_cfg.h"

#if MUSIC_AB_RPT_EN
#define DECODE_AB_REPEAT 1
#else
#define DECODE_AB_REPEAT 0
#endif


typedef enum
{
    SD0_DEVICE = 0x00,
    SD1_DEVICE,
    USB_DEVICE,
    NO_DEVICE = 0xff,
}UI_DEVICE;

#define   MUSIC_OPT_BIT_PLAY       (0<<0)
#define   MUSIC_OPT_BIT_DEL        (1<<0)
#define   MUSIC_OPT_BIT_PAUSE      (1<<1)
#define   MUSIC_OPT_BIT_FF         (1<<2)
#define   MUSIC_OPT_BIT_FR         (1<<3)
#define   MUSIC_OPT_BIT_SEL        (1<<4)

typedef struct _MUSIC_DIS_VAR
{
    UI_DEVICE ui_curr_device;
    u32 ui_curr_file;
    u32 ui_total_file;
    u32 play_time;
    u32 *ui_curr_statu;
    FS_DISP_NAME *ui_file_info;
    u8  *eq_mode;
    u8  *play_mode;
    MUSIC_OP_API *mapi;
    u8  *ab_statu;
    u8 opt_state;
    u8 lrc_flag;

}MUSIC_DIS_VAR;

extern MUSIC_DIS_VAR music_ui;

extern void ui_open_music(void *buf,u32 len);
extern void ui_close_music(void);

#endif/*_MUSIC_UI_H_*/
