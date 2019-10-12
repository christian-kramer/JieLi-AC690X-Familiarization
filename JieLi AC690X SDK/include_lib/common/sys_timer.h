#ifndef SYS_TIMER_H
#define SYS_TIMER_H


#include "typedef.h"
#include "common/list.h"




struct sys_timer{
	struct list_head entry;
	void (*fun)(struct sys_timer *);
	u32 jiffies;
    u32 user;
    u32 delay_do;
    u32 loop;
};



bool __timer_find(struct sys_timer *timer);
void sys_timer_register(struct sys_timer *timer, u32 msec,
		void (*fun)(struct sys_timer *timer),u8 delay_do);
void sys_timer_remove(struct sys_timer *timer);
void sys_timer_reset(struct sys_timer *timer, u32 msec);

void sys_timer_init(struct sys_timer *timer);
void sys_timer_schedule();
void sys_timer_delay_schedule();
void sys_timer_set_user(struct sys_timer *timer, u32 user);
u32 sys_timer_get_user(struct sys_timer *timer);
void sys_timer_delay_handler_register(void(*handler)());
void sys_timer_del_schedule();
void loop_timer_schedule();

#endif

