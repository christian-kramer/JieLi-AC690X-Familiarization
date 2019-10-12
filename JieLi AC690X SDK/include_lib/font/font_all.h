#ifndef __FONT_ALL_H__
#define __FONT_ALL_H__

#include "typedef.h"
#include "sydf/syd_file.h"
#include "font_GBK.h"
#include "font_BIG5.h"
#include "font_SJIS.h"
#include "font_KSC.h"
#include "font_OtherLanguage.h"
#include "font_utf16toansi.h"
//#include "font_out.h"

#define ntohl(x)	(u32)((x>>24)|((x>>8)&0xff00)|(x<<24)|((x&0xff00)<<8))
#define ntoh(x)		(u16)((x>>8&0x00ff)|x<<8&0xff00)

#define  Chinese_Simplified       1   //¼òÌåÖÐÎÄ
#define  Chinese_Traditional      2   //·±ÌåÖÐÎÄ
#define  Japanese                 3   //ÈÕÓï
#define  Korean                   4   //º«Óï
#define  English                  5   //Ó¢Óï
#define  French                   6   //·¨Óï
#define  German                   7   //µÂÓï
#define  Italian                  8   //Òâ´óÀûÓï
#define  Dutch                    9   //ºÉÀ¼Óï
#define  Portuguese               10  //ÆÏÌÑÑÀÓï
#define  Spanish                  11  //Î÷°àÑÀÓï
#define  Swedish                  12  //ÈðµäÓï
#define  Czech                    13  //½Ý¿ËÓï
#define  Danish                   14  //µ¤ÂóÓï
#define  Polish                   15  //²¨À¼Óï
#define  Russian                  16  //¶íÓï
#define  Turkey                   17  //ÍÁ¶úÆäÓï
#define  Hebrew                   18  //Ï£²®À´Óï
#define  Thai                     19  //Ì©Óï
#define  Hungarian                20  //ÐÙÑÀÀûÓï
#define  Romanian                 21  //ÂÞÂíÄáÑÇÓï
#define  Arabic                   22  //°¢À­²®Óï

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	u32 addr;
	u8 flag;
	u8 font_size;
	u8 font_nbytes;
}FONT_MATRIX;
#ifdef WIN32
#pragma pack(pop)
#endif

extern FONT_MATRIX fontinfo[3];
extern FONT_MATRIX ascfontinfo[3];
extern FONT_MATRIX *font;
extern FONT_MATRIX *ascfont;
extern u8 g_amplify;
extern u32 g_ConvertTableFileAddr;
extern u8 g_language_mode;
extern bool IsGB2312;
extern u8  pixBuf[100];

//extern u32 g_AsciiPixDataFileAddr;
//extern u32 g_PixDataFileAddr;
extern u32 g_ConvertTableFileAddr;


#endif
