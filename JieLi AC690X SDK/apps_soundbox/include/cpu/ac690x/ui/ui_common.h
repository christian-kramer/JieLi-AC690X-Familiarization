#ifndef _UI_COMMON_H_
#define _UI_COMMON_H_

#include "includes.h"

void itoa1(u8 i);
void itoa2(u8 i);
void itoa3(u16 i);
void itoa4(u16 i);

void itoa2_api(u8 i,u8 *buf);
void itoa4_api(u16 i,u8 *buf);

extern const u8 asc_number[];

extern u8 bcd_number[5];

#endif
