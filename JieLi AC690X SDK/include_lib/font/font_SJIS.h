#ifndef __FONT_SJIS_H__
#define __FONT_SJIS_H__

#include "typedef.h"

extern tbool InitFont_SJIS(void);
extern u16  TextOut_SJIS(u8 *str,  u16 length,  u16 x,  u16 y);
extern u16  TextOutW_SJIS(u8 *str, u16 length, u16 x, u16 y, bool isBigEndian);
extern u8 GetSJISASCCharacterData(u8 *buf, u16 asc);
extern u16 ConvertUTF16toSJIS(u16 utf);

#endif