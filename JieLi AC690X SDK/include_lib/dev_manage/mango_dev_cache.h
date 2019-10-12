#ifndef _MANGO_DEV_CACHE_H_
#define _MANGO_DEV_CACHE_H_

#include "typedef.h"

enum
{
    CACHE_SUCC = 0x0,
    CACHE_OFFLINE,
};

void set_cache_base(u32 base);
extern const struct DEV_IO * dev_reg_cache(void *parm);
extern void cache_read(u8 *buf, u32 addr, u32 len);

#endif
