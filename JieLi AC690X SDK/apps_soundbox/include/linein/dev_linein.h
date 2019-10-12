#ifndef __DEV_LINEIN_H__
#define __DEV_LINEIN_H__

#include "typedef.h"

#define AUX_CHECK_ENABLE   //aux模式是否被AUX在线离线消息影响

#define AUX_IO_BIT    BIT(0)
#define AUX_DIR_SET   (JL_PORTA->DIR |= AUX_IO_BIT)
#define AUX_PU_SET    (JL_PORTA->PU  |= AUX_IO_BIT)
#define AUX_IN_CHECK  (JL_PORTA->IN  &  AUX_IO_BIT)

typedef enum __AUX_STATUS
{
    AUX_OFF = 0,
    AUX_ON,
    NULL_AUX,
}AUX_STATUS;

typedef struct __AUX_VAR
{
    u16 pre_status;
    u8  status_cnt;
    u8  bDevOnline;
    u32 last_sta;
}AUX_VAR;


void aux_init_api(void);
void aux_check_api(u32 info);
u32 aux_is_online(void);


#endif/*__DEV_LINEIN_H__*/

