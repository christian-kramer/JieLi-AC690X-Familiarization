#ifndef __TIMER_PARAM_H__
#define __TIMER_PARAM_H__

#include "typedef.h"

/*------------------------------------------------------------------------
 *  IRFLT IO SEL：
 *  0x00--0x0f   PA[0:15]
 *  0x10--0x1f   PB[0:15]
 *  0x20--0x2a   Reserved
 *  0x2b--0x2f   PC[11:15]
 *  0x30--0x37   PD[0:7]
 *  0x38--0x3d   Reserved
 *  0x3e         USBDP
 *  0x3f         USBDM
 * ----------------------------------------------------------------------*/


#define  TIMER0 "timer0"
#define  TIMER1 "timer1"
#define  TIMER2 "timer2"
#define  TIMER3 "timer3"
#define  TIMER_MAX_ISR_FUN  20
enum
{
    TIMER_NO_ERR        =0,
    TIMER_DRV_MODE_ERR       =-1000 ,
    TIMER_DRV_OPEN_ERR        ,
    TIMER_DRV_USED_ERR        ,
    TIMER_DRV_NAME_ERR         ,
    TIMER_DRV_PARA_ERR        ,
    TIMER_DRV_ISRFUN_RDEF_ERR ,
    TIMER_MEM_FULL_ERR        ,
    TIMER_DRV_IRFLT_IO_ERR    ,
};

enum
{
    TIMER_WORK_MODE_COUNTER       =0,
    TIMER_WORK_MODE_CAPTURE_IOUP    ,
    TIMER_WORK_MODE_CAPTURE_IODN    ,
};

enum
{
    TIMER_CLK_SRC_SYSCLK          =0,
    TIMER_CLK_SRC_IOSIGN            ,
    TIMER_CLK_SRC_OSC               ,
    TIMER_CLK_SRC_RC                ,
};

enum
{
    TIMER_CMD_GET_CUR_WORK_MODE    =0,
    TIMER_CMD_GET_TOTAL_WORK_MODE    ,
    TIMER_CMD_SET_ISR_STOP           ,
    TIMER_CMD_SET_ISR_START          ,
    TIMER_CMD_SET_CLK_SRC            ,   
    TIMER_CMD_CLEAR_ISR_REG          ,
    TIMER_CMD_IRFLT_SET              ,
    TIMER_CMD_GET_START_PRDNUM       ,
    TIMER_CMD_GET_PRDNUM             ,
    TIMER_CMD_SET_CNT                ,
    TIMER_CMD_IRFLT_CLK_SET          ,
};

typedef struct timer_param
{
    u32 tick_time;
    u8 work_mode;
}__timer_param;

#endif
