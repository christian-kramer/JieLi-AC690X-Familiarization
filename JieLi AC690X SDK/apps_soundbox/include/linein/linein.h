#ifndef LINEIN_TASK_H
#define LINEIN_TASK_H

#include "includes.h"
#include "echo/echo_api.h"
#include "echo/echo_deal.h"

#define AUX_DEBUG
#ifdef AUX_DEBUG
    #define aux_puts     puts
    #define aux_printf   printf
#else
    #define aux_puts(...)
    #define aux_printf(...)
#endif

#define AUX_DETECT_EN       1
#define LINEIN_TASK_NAME   "LineinTask"
#define AUX_AD_ENABLE		0

//typedef struct _LININ_UI
//{
//    u32 disp_main;
//    u8  linin_statu;
//}LININ_UI;

extern const struct task_info linein_task_info;
extern REVERB_API_STRUCT *aux_reverb;
extern void aux_dac_channel_on(void);
extern void aux_dac_channel_off(void);

#endif

