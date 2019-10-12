#ifndef __DEV_CTL_H__
#define __DEV_CTL_H__

#include "drv_manage.h"
enum
{
    UPDATA_SD0 = 0,
    UPDATA_SD1,
    UPDATA_USB,
    UPDATA_PC,
    UPDATA_UART,
	UPDATA_BT,
};

typedef enum
{
    DEV_UPDATA = 1,
    PC_UPDATA,
    UART_UPDATA,
    BT_UPDATA,
}UPDATA_WAY;

void  dev_updata_mode(void * parm, UPDATA_WAY updata_way);

void dev_ctrl_sys_init(void *parm);

#endif
