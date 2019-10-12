#ifndef __FONT_OUT_H__
#define __FONT_OUT_H__

#include "typedef.h"
#include "string.h"

typedef void (*FONT_HOOK)(u8* pixbuf, u16 width, u16 height, u16 x, u16 y);
extern FONT_HOOK font_putchar;

//×ÖºÅ´óÐ¡
typedef enum
{
	SMALL_FONT,
	MIDDLE_FONT,
	BIG_FONT,
}FONT_SIZE;


//extern bool font_init(FONT_HOOK hook, u8 language);
bool font_init(u8 language);
u16  font_textout(u8 *buf, u16 len, u16 x, u16 y, u8 fontsize);
u16  font_textoutw(u8 *buf, u16 len, u16 x, u16 y, bool isBigEndian, u8 fontsize);
u16  font_utf16toansi(u16 utf);
tbool font_get_file_addr_bypath(u8 *path,u32 *addr);

void font_flash_read(u8 _xdata *buf,u32 addr,u32 length);

#endif
