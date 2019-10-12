/***********************************Jieli tech************************************************
  File : os_api.h
  By   : Juntham
  date : 2014-07-16 09:32
********************************************************************************************/

#ifndef   OS_API_H
#define   OS_API_H

#define  OS_VERSION                 100u                /* Version */

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned int    OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int    OS_CPU_SR;                   /* Unsigned 32 bit quantity                           */
typedef unsigned int    OS_CPU_DATA;                 /* Unsigned 32 bit quantity                           */

#include "cpu.h"
#include "typedef.h"
#include "os_cfg.h"
#include "os_cpu.h"


#if (OS_Q_EN > 0) || (OS_TASKQ_EN > 0)
typedef unsigned short QS;
#endif
    /*
     *********************************************************************************************************
     *                                             MISCELLANEOUS
     *********************************************************************************************************
     */
#define  OS_EVENT_EN       ((OS_Q_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0))

    /*
    *********************************************************************************************************
    *                              TASK STATUS (Bit definition for OSTCBStat)
    *********************************************************************************************************
    */
#define  OS_STAT_RDY               0x00u    /* Ready to run                                            */
#define  OS_STAT_SEM               0x01u    /* Pending on semaphore                                    */
#define  OS_STAT_Q                 0x02u    /* Pending on queue                                        */
#define  OS_STAT_SUSPEND           0x04u    /* Task is suspended                                       */
#define  OS_STAT_MUTEX             0x08u    /* Pending on mutual exclusion semaphore                   */
#define  OS_STAT_TASK_Q            0x10u    /* Pending on task Q                                       */
#define  OS_STAT_DELAY             0x20u    /* Task on delay                                           */

#define  OS_STAT_PEND_ANY         (OS_STAT_SEM | OS_STAT_Q | OS_STAT_MUTEX | OS_STAT_TASK_Q)

    /*
     *********************************************************************************************************
     *                                        OS_EVENT types
     *********************************************************************************************************
    */
#define  OS_EVENT_TYPE_UNUSED         0u
#define  OS_EVENT_TYPE_Q              1u
#define  OS_EVENT_TYPE_SEM            2u
#define  OS_EVENT_TYPE_MUTEX          3u

    /*
     *********************************************************************************************************
     *                                   Values for OSTickStepState
     *
     * Note(s): This feature is used by uC/OS-View.
     *********************************************************************************************************
    */

#if OS_TICK_STEP_EN > 0
#define  OS_TICK_STEP_DIS             0u    /* Stepping is disabled, tick runs as mormal               */
#define  OS_TICK_STEP_WAIT            1u    /* Waiting for uC/OS-View to set OSTickStepState to _ONCE  */
#define  OS_TICK_STEP_ONCE            2u    /* Process tick once and wait for next cmd from uC/OS-View */
#endif

    /*
     *********************************************************************************************************
     *       Possible values for 'opt' argument of OSSemDel(), OSMboxDel(), OSQDel() and OSMutexDel()
     *********************************************************************************************************
    */
#define  OS_DEL_NO_PEND               0u
#define  OS_DEL_ALWAYS                1u

#define  OS_TASK_DEL_REQ           0x01u
#define  OS_TASK_DEL_RES           0x02u
#define  OS_TASK_DEL_OK            0x03u
    /*
    *********************************************************************************************************
    *                                     OS???PostOpt() OPTIONS
    *
    * These #defines are used to establish the options for OSMboxPostOpt() and OSQPostOpt().
    *********************************************************************************************************
    */
#define  OS_POST_OPT_NONE          0x00u    /* NO option selected                                      */
#define  OS_POST_OPT_BROADCAST     0x01u    /* Broadcast message to ALL tasks waiting                  */
#define  OS_POST_OPT_FRONT         0x02u    /* Post to highest priority task waiting                   */

    /*
     *********************************************************************************************************
     *                                 TASK OPTIONS (see OSTaskCreateExt())
     *********************************************************************************************************
     */
#define  OS_TASK_OPT_NONE        0x0000u    /* NO option selected                                      */
#define  OS_TASK_OPT_STK_CHK     0x0001u    /* Enable stack checking for the task                      */
#define  OS_TASK_OPT_STK_CLR     0x0002u    /* Clear the stack when the task is create                 */
#define  OS_TASK_OPT_SAVE_FP     0x0004u    /* Save the contents of any floating-point registers       */

#define  OS_TASK_SELF        (char *)0x1
#define  OS_TASK_FATHER      (char *)0x2

    /*
     *********************************************************************************************************
     *                                             ERROR CODES
     *********************************************************************************************************
    */
        enum
        {
            OS_NO_ERR = 0,
            OS_TRUE,
            OS_ERR_EVENT_TYPE,
            OS_ERR_PEND_ISR,
            OS_ERR_POST_NULL_PTR,
            OS_ERR_PEVENT_NULL,
            OS_ERR_POST_ISR,
            OS_ERR_QUERY_ISR,
            OS_ERR_INVALID_OPT,
            OS_ERR_TASK_WAITING,
            OS_ERR_PDATA_NULL,
            OS_TIMEOUT,
            OS_TASK_NOT_EXIST,
            OS_ERR_EVENT_NAME_TOO_LONG,
            OS_ERR_FLAG_NAME_TOO_LONG,
            OS_ERR_TASK_NAME_TOO_LONG,
            OS_ERR_PNAME_NULL,
            OS_ERR_TASK_CREATE_ISR,
            OS_MBOX_FULL,
            OS_Q_FULL,
            OS_Q_EMPTY,
            OS_Q_ERR,
            OS_ERR_NO_QBUF,
            OS_PRIO_EXIST,
            OS_NAME_EXIST,
            OS_STACK_TOO_LARGE,
            OS_PRIO_ERR,
            OS_PRIO_INVALID,
            OS_SEM_OVF,
            OS_TASK_DEL_ERR,
            OS_TASK_DEL_IDLE,
            OS_TASK_DEL_ISR,
            OS_NO_MORE_TCB,
            OS_TIME_NOT_DLY,
            OS_TIME_INVALID_MINUTES,
            OS_TIME_INVALID_SECONDS,
            OS_TIME_INVALID_MILLI,
            OS_TIME_ZERO_DLY,
            OS_TASK_SUSPEND_PRIO,
            OS_TASK_SUSPEND_IDLE,
            OS_TASK_RESUME_PRIO,
            OS_TASK_NOT_SUSPENDED,
            OS_MEM_INVALID_PART,
            OS_MEM_INVALID_BLKS,
            OS_MEM_INVALID_SIZE,
            OS_MEM_NO_FREE_BLKS,
            OS_MEM_FULL,
            OS_MEM_INVALID_PBLK,
            OS_MEM_INVALID_PMEM,
            OS_MEM_INVALID_PDATA,
            OS_MEM_INVALID_ADDR,
            OS_MEM_NAME_TOO_LONG,
            OS_ERR_MEM_NO_MEM,
            OS_ERR_NOT_MUTEX_OWNER,
            OS_TASK_OPT_ERR,
            OS_ERR_DEL_ISR,
            OS_ERR_CREATE_ISR,
            OS_FLAG_INVALID_PGRP,
            OS_FLAG_ERR_WAIT_TYPE,
            OS_FLAG_ERR_NOT_RDY,
            OS_FLAG_INVALID_OPT,
            OS_FLAG_GRP_DEPLETED,
            OS_ERR_PIP_LOWER,
            OS_ERR_MSG_POOL_LEN,
            OS_ERR_MSG_POOL_EMPTY,
            OS_ERR_MSG_POOL_NULL_PTR,
            OS_ERR_MSG_POOL_FULL,


        };
    /*$PAGE*/
    /*
    *********************************************************************************************************
    *                                          EVENT CONTROL BLOCK
    *********************************************************************************************************
    */
#ifndef OS_CREATE_LIB
#if OS_EVENT_EN
        struct event_cnt
        {
            u16 cnt;
        };
        struct event_mutex
        {
            u8 value;
            u8 prio;
        };

        struct os_tcb;
        typedef struct os_event
        {
            u8    OSEventType;
#if OS_TIME_SLICE_EN > 0
            struct   os_tcb  *OSTCBList;
#else
            OS_CPU_DATA   OSTCBList;
#endif
            void    *OSEventPtr;
            union
            {
                struct event_cnt OSEvent;
                struct event_mutex OSMutex;
            }u;
        } OS_EVENT;
#endif


    /*$PAGE*/

    /*
     *********************************************************************************************************
     *                                          MESSAGE QUEUE DATA
     *********************************************************************************************************
     */

#if (OS_Q_EN > 0) || (OS_TASKQ_EN > 0)
    typedef struct os_q                     /* QUEUE CONTROL BLOCK                                         */
    {
        QS         a;
        QS         b;
        QS         c;
        QS         d;
        void       **e;
    } OS_Q;
#endif

    /*$PAGE*/

    /*
     *********************************************************************************************************
     *                                          TASK CONTROL BLOCK
     *********************************************************************************************************
    */
        typedef struct os_tcb
        {
            OS_STK        *OSTCBStkPtr;      /* Pointer to current top of stack                              */

#if OS_CPU_MMU > 0
            u8            *frame;
#endif

#if OS_TIME_SLICE_EN > 0
            u8             slice_quanta;         /* 时间片初始值 */
            u8             slice_cnt;     /* 时间片模式下的计数值 */
            u16            OSTCBDly;         /* Nbr ticks to delay task or, timeout waiting for event        */
#endif

#if OS_TASKQ_EN > 0
            OS_Q           task_q;
#endif

#if OS_PARENT_TCB > 0
            struct os_tcb   *OSTCBParent;
#endif

#if OS_CHILD_TCB > 0
            struct os_tcb   *OSTCBChildNext;
#endif

#if OS_TIME_SLICE_EN > 0
            struct os_tcb   *OSTCBEventNext; /* Pointer to next     TCB in the event waiting TCB list     */
            struct os_tcb   *OSTCBEventPrev; /* Pointer to previous TCB in the event waiting TCB list     */
            struct os_tcb   *OSTCBSliceNext; /* Pointer to next     TCB in the time slice TCB list        */
            struct os_tcb   *OSTCBSlicePrev; /* Pointer to previous TCB in the time slice TCB list        */
#endif

#if OS_EVENT_EN
            OS_EVENT        *OSTCBEventPtr;    /* Pointer to event control block                               */
#endif

#if (OS_Q_EN > 0)
            void            *OSTCBMsg;         /* Message received from OSMboxPost() or OSQPost()              */
#endif

#if OS_TIME_SLICE_EN == 0
            u16           OSTCBDly;         /* Nbr ticks to delay task or, timeout waiting for event        */
#endif
            u8            OSTCBPrio;        /* Task priority (0 == highest)                                 */
            u8            OSTCBStat;        /* Task status                                                  */
            bit1          OSTCBPendTO;      /* Flag indicating PEND timed out (TRUE == timed out)           */

#if OS_TASK_DEL_EN > 0
            u8            OSTCBDelReq;      /* Indicates whether a task needs to delete itself              */
#endif

            char          *name;
            u32           name_crc32;
#if OS_SET_STACK_SIZE_EN > 0
            u16           stack_size;
#endif
        } OS_TCB;

typedef OS_EVENT OS_SEM,OS_MUTEX;

    /*
     *********************************************************************************************************
     *                                MUTUAL EXCLUSION SEMAPHORE MANAGEMENT
     *********************************************************************************************************
     */

#if OS_MUTEX_EN > 0

#if OS_MUTEX_ACCEPT_EN > 0
    u8         OSMutexAccept(OS_EVENT *pevent);
#define os_mutex_accept OSMutexAccept
#endif

    u8     OSMutexCreate(OS_EVENT *pevent);
#define os_mutex_create OSMutexCreate

#if OS_MUTEX_DEL_EN > 0
    u8     OSMutexDel(OS_EVENT *pevent, u8 opt);
#define os_mutex_del OSMutexDel
#endif

    u8         OSMutexPend(OS_EVENT *pevent, u16 timeout);
#define os_mutex_pend OSMutexPend
    u8         OSMutexPost(OS_EVENT *pevent);
#define os_mutex_post OSMutexPost

#if OS_MUTEX_QUERY_EN > 0
    u8         OSMutexQuery(OS_EVENT *pevent, OS_MUTEX_DATA *p_mutex_data);
#define os_mutex_query OSMutexQuery
#endif

#endif/*OS_MUTEX_EN > 0*/

    /*
     *********************************************************************************************************
     *                                         MESSAGE QUEUE MANAGEMENT
     *********************************************************************************************************
     */
#if (OS_Q_EN > 0)

#if OS_Q_ACCEPT_EN > 0
    u8         OSQAccept(OS_EVENT *pevent, void *msg);
#define os_q_accept OSQAccept
#endif

    u8         OSQCreate(OS_EVENT *pevent, void **start, u8 size);
#define os_q_create OSQCreate

#if OS_Q_DEL_EN > 0
    u8         OSQDel(OS_EVENT *pevent, u8 opt);
#define os_q_del OSQDel
#endif

#if OS_Q_FLUSH_EN > 0
    u8         OSQFlush(OS_EVENT *pevent);
#define os_q_flush OSQFlush
#endif

    u8         OSQPend(OS_EVENT *pevent, u16 timeout, void *msg);
#define os_q_pend OSQPend

#if OS_Q_POST_EN > 0
    u8         OSQPost(OS_EVENT *pevent, void *msg);
#define os_q_post OSQPost
#endif

#if OS_Q_POST_FRONT_EN > 0
    u8         OSQPostFront(OS_EVENT *pevent, void *msg);
#define os_q_postfront OSQPostFront
#endif

#if OS_Q_POST_OPT_EN > 0
    u8         OSQPostOpt(OS_EVENT *pevent, void *msg, u8 opt);
#define os_q_postOpt OSQPostOpt
#endif

#if OS_Q_QUERY_EN > 0
    u8         OSQQuery(OS_EVENT *pevent, OS_Q_DATA *p_q_data);
#define os_q_query OSQQuery
#endif

#endif/*(OS_Q_EN > 0)*/

    /*$PAGE*/
    /*
    *********************************************************************************************************
    *                                          SEMAPHORE MANAGEMENT
    *********************************************************************************************************
    */
#if OS_SEM_EN > 0

#if OS_SEM_ACCEPT_EN > 0
    u16    OSSemAccept(OS_EVENT *pevent);
#define os_sem_accept OSSemAccept
#endif

    u8     OSSemCreate(OS_EVENT *pevent, u16 cnt);
#define os_sem_create OSSemCreate

#if OS_SEM_DEL_EN > 0
    u8     OSSemDel(OS_EVENT *pevent, u8 opt);
#define os_sem_del OSSemDel
#endif

    u8     OSSemPend(OS_EVENT *pevent, u16 timeout);
#define os_sem_pend OSSemPend
    u8     OSSemPost(OS_EVENT *pevent);
#define os_sem_post OSSemPost

#if OS_SEM_QUERY_EN > 0
    u8     OSSemQuery(OS_EVENT *pevent, OS_SEM_DATA *p_sem_data);
#define os_sem_query OSSemQuery
#endif

#if OS_SEM_SET_EN > 0
    u8     OSSemSet(OS_EVENT *pevent, u16 cnt);
#define os_sem_set OSSemSet
#endif

#endif/*OS_SEM_EN > 0*/

    /*$PAGE*/
    /*
     *********************************************************************************************************
     *                                            TASK MANAGEMENT
     *********************************************************************************************************
     */
#if OS_TASK_CHANGE_PRIO_EN > 0
        u8         OSTaskChangePrio(char *name, u8 newprio);
#define os_task_change_prio OSTaskChangePrio
#endif

u8 os_task_create(void (*task)(void *p_arg)
#if MEM_MALLOC == 0
        , OS_TCB *task_tcb
#endif
        , void *p_arg

#if OS_CPU_MMU == 0
         , OS_STK *ptos
#endif
         , u8 prio
#if OS_TASKQ_EN > 0
#if MEM_MALLOC == 0
                  , void **start
#endif
                  , QS qsize
#endif
                  ,s8 *name
                 );

/* u8 os_task_create(void (*task)(void *p_arg), void *p_arg, u8 prio, QS qsize ,s8 *name); */
u8 os_task_create_ext(void (*task)(void *p_arg), void *p_arg, u8 prio, QS qsize,s8 *name,u16 stack_size);

u8 OSTaskQAccept (int argc, void *msg);
#define os_taskq_accept OSTaskQAccept

u8 OSTaskQPend (u16 timeout, int argc, void *msg);
#define os_taskq_pend OSTaskQPend

u8  OSTaskQPostWait(char *name, u16 time_out, int argc, ...);
u8  OSTaskQPostWaitConst(char *name, int argc, ...);
#define os_taskq_post OSTaskQPost

u8  OSTaskQPost(char *name, int argc, ...);
#define os_taskq_post_wait OSTaskQPostWait

//#define os_taskq_post_event OSTaskQPostWaitConst
#define os_taskq_post_event OSTaskQPostEvent
#define os_taskq_post_msg   OSTaskQPost

u8  OSTaskQCount (char *name, u32 *task_q_cnt);
#define os_taskq_count OSTaskQCount

u8  OSTaskQFlush (char *name);
#define os_taskq_flush OSTaskQFlush
#define os_taskq_flush_name OSTaskQFlush

u8  OSTaskQPostFront (char *name, int argc, ...);
#define os_taskq_post_front OSTaskQPostFront

u8  OSTaskQPostFrontWait(char *name, u16 time_out, int argc, ...);
#define os_taskq_post_front_wait OSTaskQPostFrontWait

u8  OSTaskQQuery (char *name, u8 *task_q_entries);
#define os_taskq_query OSTaskQQuery
#define os_taskq_query_name OSTaskQQuery

OS_TCB *OSGetTaskTcb (char *name);
#define os_get_task_tcb OSGetTaskTcb


#if OS_TASK_DEL_EN > 0
    u8   OSTaskDel(char *name);
#define os_task_del(a) OSTaskDel(a)
#define os_task_del_name(a) OSTaskDel(a)
    u8   OSTaskDelReq(const char *name);
#define os_task_del_req(a) OSTaskDelReq(a)
#define os_task_del_req_name(a) OSTaskDelReq(a)
    void OSTaskDelRes(char *name);
#define os_task_del_res(a) OSTaskDelRes(a)
#define os_task_del_res_name(a) OSTaskDelRes(a)
#endif


#if OS_TASK_SUSPEND_EN > 0
    u8   OSTaskResume(char *name);
#define os_task_resume OSTaskResume
#define os_task_resume_name OSTaskResume
    u8   OSTaskSuspend(char *name);
#define os_task_suspend OSTaskSuspend
#define os_task_suspend_name OSTaskSuspend
#endif

#if OS_TASK_QUERY_EN > 0
    u8   OSTaskQuery(char *name, OS_TCB *p_task_data);
#define os_task_query OSTaskQuery
#define os_task_query_name OSTaskQuery
#endif

    /*$PAGE*/
       /*
        *********************************************************************************************************
        *                                            TIME MANAGEMENT
        *********************************************************************************************************
        */

        void          OSTimeDly(u16 ticks);
#define os_time_dly OSTimeDly


#if OS_TIME_GET_SET_EN > 0
    u32  OSTimeGet(void);
#define os_time_get OSTimeGet
    void OSTimeSet(u32 ticks);
#define os_time_set OSTimeSet

#endif

    void OSTimeTick(void);
#define os_time_tick OSTimeTick

    /*
     *********************************************************************************************************
     *                                             MISCELLANEOUS
     *********************************************************************************************************
     */

    void OSInit(void);
#define os_init OSInit

    void OSStart();
#define os_start OSStart

    u16  OSVersion(void);
#define os_version OSVersion

#if OS_SCHED_LOCK_EN > 0
    void          OSSchedLock(void);
    void          OSSchedUnlock(void);

#define os_sched_lock() 	OSSchedLock()
#define os_sched_unlock()   OSSchedUnlock()
#endif

/*----------------------------------------------------------------------------*/
/**@brief  发送消息通过先入先出方式，，如果消息池满，会一直尝试10次
   @param  name：线程的字符串表示，表示接受此消息的线程
   @param  time_out:重试次数
   @param  argc：消息数量
   @param  ...：列举出各个消息
   @return  0发送成功
   @note
*/
/*----------------------------------------------------------------------------*/
extern u8  os_taskq_post_event (char *name,int argc, ...);

/*----------------------------------------------------------------------------*/
/**@brief  发送消息通过先入先出方式，设置time_out次数，如果消息池满，会尝试time_out次一直发送
   @param  name：线程的字符串表示，表示接受此消息的线程
   @param  time_out:重试次数
   @param  argc：消息数量
   @param  ...：列举出各个消息
   @return  0发送成功
   @note
*/
/*----------------------------------------------------------------------------*/
extern u8  os_taskq_post_wait (char *name,u16 time_out,int argc, ...);
/*----------------------------------------------------------------------------*/
/**@brief  发送消息通过先入先出方式，不检查当前消息buffer是否满，不保证发送成功
   @param  name：线程的字符串表示，表示接受此消息的线程
   @param  argc：消息数量
   @param  ...：列举出各个消息
   @return  0发送成功
   @note
*/
/*----------------------------------------------------------------------------*/
extern u8 os_taskq_post_msg(char *name, int argc, ...);
/*----------------------------------------------------------------------------*/
/**@brief  获取消息
   @param  Timeout： 获取消息等待（默认请使用0）
   @param  Arge:  用于获取消息的buffer的大小
   @param  msg：  用于获取消息的buffer的指针
   @return 0：没有错误  ；其它值错误状态
   @note
*/
/*----------------------------------------------------------------------------*/
extern u8 os_taskq_pend(u16 timeout, int argc, void *msg);
/*----------------------------------------------------------------------------*/
/**@brief  创建信号量
   @param  p_sem： 信号量指针
   @param  cnt：信号量初始值
   @return 0：没有错误  ；其它值错误状态
   @note
*/
/*----------------------------------------------------------------------------*/
extern u8 os_sem_create (OS_EVENT *p_sem,u16   cnt);
/*----------------------------------------------------------------------------*/
/**@brief  发送信号量
   @param  p_sem： 信号量指针
   @return 0：没有错误  ；其它值错误状态
   @note
*/
/*----------------------------------------------------------------------------*/
extern u8  os_sem_post (OS_EVENT  *p_sem);
/*----------------------------------------------------------------------------*/
/**@brief  等待信号量
   @param  p_sem： 信号量指针
   @param  timeout： 超时时间（0为不超时）
   @return 0：没有错误  ；其它值错误状态
   @note
*/
/*----------------------------------------------------------------------------*/
extern u8 os_sem_pend (OS_EVENT   *p_sem, u16   timeout);
/*----------------------------------------------------------------------------*/
/**@brief  设置信号量的值
   @param  p_sem： 信号量指针
   @param  cnt： 信号量的值
   @return 0：没有错误  ；其它值错误状态
   @note
*/
/*----------------------------------------------------------------------------*/
extern u8  os_sem_set (OS_EVENT   *p_sem, u16   cnt);

#define OS_MUTEX_INIT() \
	{ \
		.OSEventType = OS_EVENT_TYPE_MUTEX, \
		.u.OSMutex.value = 1, \
		.u.OSMutex.prio = 0xff, \
		.OSEventPtr = (void *)0, \
		.OSTCBList = 0, \
	}

#define OS_SEM_INIT(c) \
	{ \
		.OSEventType = OS_EVENT_TYPE_SEM, \
		.u.OSEvent.cnt = c, \
		.OSEventPtr = (void *)0, \
		.OSTCBList = 0, \
	}














#endif          /*end of CREATE_LIB*/

#ifdef __cplusplus
}
#endif
#endif
