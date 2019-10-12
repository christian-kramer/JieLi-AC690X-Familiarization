#ifndef _INCLUDES_
#define _INCLUDES_

#include "typedef.h"
#include "common/printf.h"
#include "uart.h"
#include "string.h"
#include "mem/malloc.h"
#include "common/msg.h"
#include "hw_cpu.h"
#include "rtos/os_api.h"


#define MAIN_TASK_NAME        "MainTask"
#define RTC_TASK_NAME         "RTCTask"
#define MUSIC_TASK_NAME       "MusicTask"
#define RECORD_TASK_NAME      "RECTask"
#define FM_TASK_NAME		  "FMRadioTask"
#define LINEIN_TASK_NAME      "LineinTask"
#define UDISK_TASK_NAME       "USBdevTask"
#define BTMSG_TASK_NAME       "btmsg"
#define BTSTACK_TASK_NAME     "BtStackTask"
#define IDLE_TASK_NAME        "IdleTask"
#define RCSP_TASK_NAME 		  "RCSP_Task"
#define RDEF_LIGHT_TASK_NAME       "LightTask"

#endif

