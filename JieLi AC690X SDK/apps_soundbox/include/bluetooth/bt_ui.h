#ifndef _BT_UI_H_
#define _BT_UI_H_

#include "bluetooth/avctp_user.h"
#include "echo/echo_api.h"
#include "echo/echo_deal.h"

typedef struct _BT_DIS_VAR
{
    u8 ui_bt_connect_sta;
    u8 ui_bt_a2dp_sta;
    u8 reserver[2];
    u8 *bt_eq_mode;
}BT_DIS_VAR;

BT_DIS_VAR bt_ui_var;

void ui_open_bt(void *buf,u32 len);
void ui_close_bt(void);

#endif/*_BT_UI_H_*/
