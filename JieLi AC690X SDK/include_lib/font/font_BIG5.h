#ifndef __FONT_BIG5_H__
#define __FONT_BIG5_H__

#include "typedef.h"

extern tbool InitFont_BIG5(void);
extern u16  TextOut_BIG5(u8 *str,  u16 length,  u16 x,  u16 y);
extern u16  TextOutW_BIG5(u8 *str, u16 length, u16 x, u16 y, bool isBigEndian);
extern u16 ConvertUTF16toBIG5(u16 utf);

#endif