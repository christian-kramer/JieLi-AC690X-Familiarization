#ifndef __FONT_KSC_H__
#define __FONT_KSC_H__

#include "typedef.h"

extern tbool InitFont_KSC(void);
extern u16  TextOut_KSC(u8 *str,  u16 length,  u16 x,  u16 y);
extern u16  TextOutW_KSC(u8 *str, u16 length, u16 x, u16 y, bool isBigEndian);
extern u16 ConvertUTF16toKSC(u16 utf);

#endif