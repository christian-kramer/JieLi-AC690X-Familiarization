#ifndef __IO_API_H__
#define __IO_API_H__

#include "typedef.h"

s32 io_read(char *name,u32 cmd);
s32 io_write(char *name,u32 cmd,u32 value);
s32 io_setbit(char *name,u32 cmd,u32 bitpos);
s32 io_clearbit(char *name,u32 cmd,u32 bitpos);



#endif
