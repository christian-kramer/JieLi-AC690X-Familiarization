#ifndef __KEY_DRV_IO_H__
#define __KEY_DRV_IO_H__

#include "sdk_cfg.h"
#include "sys_detect.h"
#include "rtc/rtc_api.h"

#if KEY_IO_EN
   //set usb_io to key_io
   #define IS_KEY0_DOWN()    (!PORTR_IN(PORTR2))  //PP PR2
   #define IS_KEY1_DOWN()    (!USB_IO_DP_IN())       //V+
   #define IS_KEY2_DOWN()    (!USB_IO_DM_IN())       //V-
   #define IS_KEY3_DOWN()    0
   #define KEY_INIT()        do{PORTR_PU(PORTR2,1);PORTR_PD(PORTR2,0);PORTR_DIR(PORTR2,1);\
	   							USB_DM_PU(1);USB_DM_PD(0);USB_DM_DIR(1);\
	   							USB_DP_PU(1);USB_DP_PD(0);USB_DP_DIR(1);\
  							   }while(0)


tu8 get_iokey_value(void);
extern void io_key_init(void);
extern  u8  io_key_get_value(void);
extern  u8  io_key_pressed_to_msg(u8 key_style, u8 key_num);

#else
   #define IS_KEY0_DOWN()   0 
   #define IS_KEY1_DOWN()   0 
   #define IS_KEY2_DOWN()   0 
   #define IS_KEY3_DOWN()   0
   #define KEY_INIT()        
#endif/*KEY_IO_EN*/

#endif/*__KEY_DRV_IO_H__*/
