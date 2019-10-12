#ifndef __FONT_OTHERLANGUAGE_H__
#define __FONT_OTHERLANGUAGE_H__

#include "typedef.h"

typedef struct
{
	u8  width;
	u8  size;
	u16 addr;
} ASCSTRUCT;

extern tbool InitFont_ASCII(void);
extern u8  GetASCIICharacterData(u8 *buf, u16 asc);
extern tbool InitFont_OtherLanguage(u8 language);
extern u16  TextOut_OtherLanguage(u8 *str,  u16 length,  u16 x,  u16 y);
extern u16  TextOutW_OtherLanguage(u8 *str, u16 length, u16 x, u16 y, bool isBigEndian);
extern u8 GetOtherLanguageCharacterData(u8 *buf, u16 asc);
extern u8 GetASCIICharacterData(u8 *buf, u16 asc);
extern u16 ConvertUTF16toOtherLanguage(u16 utf);

#endif