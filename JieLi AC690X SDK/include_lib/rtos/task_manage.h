#ifndef __TASK_MANAGE__
#define __TASK_MANAGE__

#include "typedef.h"

typedef enum
{
    RUN_TASK_SUCC = 0,          ///<运行任务成功
    RUN_TASK_ERR_NO_TAG,        ///<没有指定运行什么任务
    RUN_TASK_ERR_HAS_RPT,       ///<指定的任务已经处于运行状态
}RUN_TASK_STATUS;


typedef enum
{
    SWITCH_SPEC_TASK =0,        ///<指定task
    SWITCH_NEXT_TASK ,          ///<下一task
    SWITCH_PRE_TASK,            ///<上一task
    SWITCH_LAST_TASK,            ///<前一个一task
}TASK_SWITCH_MODE;

struct task_info
{
	const char *name;
	void (*init)(void *priv);
	void (*exit)();
};

struct task_dev_info
{
	const char *name;
	u32 (*dev_check)(void);
};

// #define  TASK_REGISTER(task) \
	// const struct task_info task __attribute__((section(".task_info")))
#define  TASK_REGISTER(task) \
	const struct task_info task sec(.task_info)


extern struct task_info *curr_task;

RUN_TASK_STATUS task_switch(char *task_name,void *priv,TASK_SWITCH_MODE cur_mode);
u32 compare_task_name(char *task_name);
int msg_mask_off_in_bt();

#endif
