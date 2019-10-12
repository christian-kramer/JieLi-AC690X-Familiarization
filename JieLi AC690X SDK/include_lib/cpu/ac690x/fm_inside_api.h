#ifndef __FM_INSIDE_API__
#define __FM_INSIDE_API__


#include "typedef.h"

enum
{
	SET_FM_INSIDE_MODULE_OPEN,
	SET_FM_INSIDE_SCAN_ARG1,
	SET_FM_INSIDE_SCAN_ARG2,
	SET_FM_INSIDE_OSC_CLK,
	SET_FM_INSIDE_OSC_SEL,
	SET_FM_INSIDE_PRINTF,
};

void fm_inside_on(void * parm);
void fm_inside_off(void);
u8 set_fm_inside_freq(u32 freq);
void set_fm_inside_mute(u8 mute);
bool read_fm_inside_id(void);
void fm_inside_io_ctrl(int ctrl,...);
void fm_inside_module_open(void);

#endif
