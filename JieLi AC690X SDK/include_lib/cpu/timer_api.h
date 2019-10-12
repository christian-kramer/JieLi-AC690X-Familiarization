#ifndef __TIMER_API_H__
#define __TIMER_API_H__

#include "typedef.h"
#include "common/list.h"
#include "timer_param.h"

typedef struct timer_list
{
    struct list_head list;
    char *name;
    u32 time;
    void *param;
    void (*handler)(void *param);
    u32 cnt;
    u32 max_item;
}__timer_handle;

//-------------------API function---------------------------------------------
void timer_module_on();
void timer_module_off();
__timer_handle  *timer_open(char *name,u32 max_isr_fun);
s32 timer_close(__timer_handle *handle);
s32 timer_init_api(__timer_handle *handle,__timer_param *param);
s32 timer_start(__timer_handle *handle);
s32 timer_stop(__timer_handle *handle);
s32 timer_reg_isr_callback_fun(__timer_handle *handle,u32 priority,void (*callback_fun)());
s32 timer_unreg_isr_callback_fun(__timer_handle *handle,u32 priority,void (*callback_fun)());
s32 timer_reg_isr_fun(__timer_handle *handle , u32 time , void (*fun)(void *param) , void *param);
s32 timer_unreg_isr_fun(__timer_handle *handle,void (*fun)(void *param));
s32 timer_ctrl(__timer_handle *handle,u32 cmd,u32 para,void *p);


#endif
