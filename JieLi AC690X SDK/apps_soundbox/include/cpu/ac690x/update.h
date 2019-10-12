#ifndef _UPDATE_H_
#define _UPDATE_H_

#include "typedef.h"

#define UPDATE_OK_RUN	0x01	//reset run directly when update successfully
#define UPDATE_OK_WHILE	0x02	//stop and remainding when update successfully

void led_update_start(void);
void led_update_finish(void);
void update_check(u32 res);
void update_deal(u8 option);
bool device_is_first_start();
bool device_is_updata_over();

#endif
