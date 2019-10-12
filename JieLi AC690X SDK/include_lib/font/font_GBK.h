#ifndef __FONT_GBK_H__
#define __FONT_GBK_H__

#include "typedef.h"

extern tbool InitFont_GBK(void);
extern u16  TextOut_GBK(u8 *str,  u16 length,  u16 x,  u16 y);
extern u16  TextOutW_GBK(u8 *str, u16 length, u16 x, u16 y, bool isBigEndian);
extern u16 ConvertUTF16toGB2312(u16 utf);
extern u16 ConvertUTF16toGBK(u16 utf);

#endif