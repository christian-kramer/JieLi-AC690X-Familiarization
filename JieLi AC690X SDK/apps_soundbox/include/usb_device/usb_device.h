#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__

#include "sdk_cfg.h"

#define PC_DEBUG
#ifdef PC_DEBUG
    #define pc_puts     puts
    #define pc_printf   printf
#else
    #define pc_puts(...)
    #define pc_printf(...)
#endif

#define PC_TASK_NAME        "USBdevTask"
#define PC_CARD_TASK_NAME   "PcCardTask"

extern const struct task_info pc_task_info;

#endif/*__USB_DEVICE_H__*/
