#ifndef __FONT_AREA_CTL_H__
#define __FONT_AREA_CTL_H__

#include "typedef.h"

typedef struct _COORDINATE
{
    u16 x;
    u16 y;
} COORDINATE;


extern void screen_set_size(u16 width, u16 heigth);
extern u16  screen_get_width(void);
extern u16  screen_get_height(void);
extern void font_set_range(u16 x,u16 y,u16 width,u16 height);
extern u16 font_get_width(void);
extern u16 font_get_heigth(void);

extern u16 g_text_width;
extern u16 g_text_heigth;

#endif
