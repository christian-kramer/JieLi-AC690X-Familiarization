#ifndef __LYRICS_API_H__
#define __LYRICS_API_H__

#include "includes.h"
#include "dec/music_api.h"
#include "uicon/lyrics.h"
#include "uicon/ui.h"

//宏定义区
#define LRC_DISPLAY_TEXT_ID   DVcTxt1_11
#define LRC_DISPLAY_TEXT_LEN   32

#define  ONCE_READ_LENGTH     ALIGN_4BYTE(128)   ///<涉及内存对齐问题，值最好是4的倍数(最大允许值为255)
#define  ONCE_DIS_LENGTH      ALIGN_4BYTE(64)    ///显示歌词的缓存长度
#define  LABEL_TEMP_BUF_LEN   ALIGN_4BYTE(2048)  ///暂用2K缓存时间标签

extern int lrc_init(void);
extern void lrc_exit(void);
extern int lrc_find(MUSIC_OP_API *m_op_api);
#endif//__LYRICS_API_H__


