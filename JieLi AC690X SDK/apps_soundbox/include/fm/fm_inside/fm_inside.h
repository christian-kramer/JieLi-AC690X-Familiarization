/*--------------------------------------------------------------------------*/
/**@file     fm_inside.c
   @brief    内部收音底层驱动
   @details
   @author
   @date   2011-3-30
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef _FM_INSIDE_H_
#define _FM_INSIDE_H_


#include "sdk_cfg.h"
// #include "fm_driver_lib/fm_set.h"


#define FM_DAC_OUT_SAMPLERATE  32000L

void fm_inside_init(void);
bool fm_inside_set_fre(u16 fre);
bool fm_inside_read_id(void);
void fm_inside_powerdown(void);
void fm_inside_mute(u8 flag);
void fm_msg_printf(void);
u32 get_fm_stereo(void);

// void set_fm_printf(u8);
// void set_fm_osc(u32 osc_fre);

// void fm_auto_set(void);

#endif // _FM_INSIDE_H_
