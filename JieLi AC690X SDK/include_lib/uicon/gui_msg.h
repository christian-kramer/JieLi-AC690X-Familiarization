#ifndef __GUI_MSG_H__
#define __GUI_MSG_H__

#include "typedef.h"

//typedef void (*MsgCB)(void);

typedef struct
{
	u16 msg_id;			//控件id号  [15..12]:控件类型
	void *p;			//用于传递参数
//    MsgCB cb;			//显示回调函数
    u32 result;			//返回结果
}GMSG;

typedef struct
{
	GMSG *msg_pool;		//消息池
	u16 msg_read;		//读指针
	u16 msg_write;		//写指针
	u16 msgcnt;			//消息最大容量
} GUI_MSG;

void gui_msg_init(u16 element);
void gui_flush_all_msg(void);
void gui_send_fifo_msg(u16 msg_id,u8 type,void *p);
void gui_send_lifo_msg(u16 msg_id,u8 type,void *p);
GMSG *gui_get_msg(void);

#endif
