/*--------------------------------------------------------------------------*/
/**@file     bk1080.h
   @brief    BK1080 ’“Ù
   @details
   @author
   @date   2011-3-30
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef _BK_1080_H_
#define _BK_1080_H_

#include "sdk_cfg.h"


#define XTAL_CLOCK			0
#define CHIP_DEV_ID 		0x80

#define delay_n10ms(x) os_time_dly(x)

void bk1080_init(void);
void bk1080_setfreq(u16 curFreq);
bool bk1080_set_fre(u16 freq);
void bk1080_powerdown(void);
void bk1080_mute(u8 flag);
bool bk1080_read_id(void);
void bk1080_setch(u8 db);


#endif		/*  _BK_1080_H_ */
