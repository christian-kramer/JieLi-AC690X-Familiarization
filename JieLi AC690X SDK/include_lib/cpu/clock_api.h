#ifndef _CLOCK_API_H_
#define _CLOCK_API_H_

#include "typedef.h"
#include "clock_interface.h"

/********************************************************************************/
/*
 *      API
 */
void clock_init(SYS_CLOCK_INPUT sys_in, u32 input_freq, u32 out_freq);

void clock_set_sys_freq(u32 freq);

u32 clock_get_sys_freq(void);

void clock_set_lsb_freq(u32 freq);

u32 clock_get_lsb_freq(void);

u32 clock_get_sfc_freq(void);

void clock_set_sys_div(u8 div);

#define UART_CLK        clock_get_lsb_freq()
#define SYS_CLK         clock_get_sys_freq()
#define SFC_CLK         clock_get_sfc_freq()

void set_spi_speed_auto(void);
#endif
