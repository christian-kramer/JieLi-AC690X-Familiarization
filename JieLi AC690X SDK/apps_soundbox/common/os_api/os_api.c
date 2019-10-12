#include "rtos/os_api.h"
#include "mem/malloc.h"
#include "stdarg.h"

#if OS_TASK_CREATE_EN > 0
extern u8  OSTaskCreate (void (*task)(void *p_arg), OS_TCB *task_tcb, void *p_arg
#if OS_CPU_MMU == 0
        , OS_STK *ptos
#endif
        , u8 prio
#if OS_TASKQ_EN > 0
        , void **start, QS qsize
#endif
#if OS_TIME_SLICE_EN > 0
        , u8 time_quanta
#endif
        ,s8 *name
#if OS_SET_STACK_SIZE_EN > 0
        ,u16 stack_size
#endif
);
#endif

/*----------------------------------------------------------------------------*/
/**@brief  创建一个任务
   @param  task：入口功能函数
   @param  p_arg：入口函数传入参数
   @param  prio：优先级
   @param  qsize:消息池大小，请采用双数
   @param  name：标识此任务的字符串
   @param  name：标识此任务的字符串
   @param  stack_size：堆栈大小（512*N）
   @return
   @note   由于系统靠name区分不同任务，所以请保证name的唯一
*/
/*----------------------------------------------------------------------------*/

u8 os_task_create_ext(void (*task)(void *p_arg), void *p_arg, u8 prio, QS qsize,s8 *name,u16 stack_size)
{
    OS_TCB *task_tcb;
    void **start;
    task_tcb = malloc(sizeof(OS_TCB)+qsize*sizeof(void *));
    start = (void *)task_tcb + sizeof(OS_TCB);

    //printf("create task_ext:%s\n",name);
    //printf("task_tcb =%08x \n",task_tcb);

    return OSTaskCreate(task,
                        task_tcb,
                        p_arg,
                        prio,
                        start,
                        qsize
                        ,name,
                        stack_size
                        );
}

/*----------------------------------------------------------------------------*/
/**@brief  创建一个任务
   @param  task：入口功能函数
   @param  p_arg：入口函数传入参数
   @param  prio：优先级
   @param  qsize:消息池大小，请采用双数
   @param  name：标识此任务的字符串
   @return
   @note   由于系统靠name区分不同任务，所以请保证name的唯一
*/
/*----------------------------------------------------------------------------*/

u8 os_task_create(void (*task)(void *p_arg), void *p_arg, u8 prio, QS qsize,s8 *name)
{
    OS_TCB *task_tcb;
    void **start;
    task_tcb = malloc(sizeof(OS_TCB)+qsize*sizeof(void *));
    start = (void *)task_tcb + sizeof(OS_TCB);

    //printf("create task:%s\n",name);
    //printf("task_tcb =%08x \n",task_tcb);

    return OSTaskCreate(task,
                        task_tcb,
                        p_arg,
                        prio,
                        start,
                        qsize
                        ,name,
                        STACK_TASK_MAX_SIZE
                        );
}

void OS_TASK_DEL_HOOK(OS_TCB *ptcb)
{
    vfree(ptcb->frame);
    free(ptcb);
}

#define OS_POST_EVENT_DEBUG_TARGET  0
struct task_info_db
{
	const char *name;
	void (*init)(void *priv);
	void (*exit)();
};


extern struct task_info_db *curr_task;
extern int puts(char *out);
extern void clear_wdt(void);//清看门狗
extern u8  _OSTaskQPost (char *name, int argc, va_list argptr);
u8  OSTaskQPostEvent(char *name, int argc, ...)
{
    u8 ret;

#if OS_POST_EVENT_DEBUG_TARGET  	
    u16 time_out = 0;
	u32 tmp;
    ///printf rets & reti
    asm("mov %0, RETS " : "=r" (tmp));
#endif

	va_list argptr;
    va_start(argptr, argc);
    while(1) {
        ret = _OSTaskQPost (name, argc, argptr);
        if (ret == OS_Q_FULL)
		{
			clear_wdt();
			puts("#");

#if OS_POST_EVENT_DEBUG_TARGET 
			time_out++;
			if((time_out % 120) == 0)
			{
				OS_TCB *self_task = os_get_task_tcb(OS_TASK_SELF);
				printf("PRIO = 0x%x\n",self_task->OSTCBPrio);
				printf("\n ***curr_task(%s) post targ_task(%s)--,call_addr:0x%x\n",curr_task->name,name,tmp);
			}
#endif

			OSTimeDly(5);
		}
        else
		{
		   	break;
		}
    }

    return ret;
}
