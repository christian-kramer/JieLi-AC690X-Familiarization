#ifndef ASCII_LIB_H
#define ASCII_LIB_H


#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"


void ASCII_ToUpper(void   *buf, u32 len);

u32 ASCII_StrCmp(const u8  *src, const u8  *dst, u32 len);

void ASCII_IntToStr(void   *pStr , u32 intNum , u32 strLen);

u32 ASCII_StrToInt(const void   *pStr , u32   *pRint, u32 strLen);

u32 ASCII_StrLen(void   *str , u32 len);

u32 ASCII_WStrLen(void   *str , u32 len);













#ifdef __cplusplus
}
#endif

#endif
