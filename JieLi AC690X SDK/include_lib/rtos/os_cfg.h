/***********************************Jieli tech************************************************
File : os_cfg.h
By   : Juntham
date : 2014-07-03 09:09
 ********************************************************************************************/

#ifndef OS_CFG_H
#define OS_CFG_H
#include "cpu.h"

#define OS_TIME_SLICE_EN          0

#define OS_SET_STACK_SIZE_EN      1
#if OS_SET_STACK_SIZE_EN   >   0
#define IDLE_TASK_STACK_SIZE    0x1000
#else
#define IDLE_TASK_STACK_SIZE    STACK_TASK_SIZE
#endif

#define OS_PRIORITY_INVERSION     1     /*是否处理优先级翻转*/

/* ---------------------- MISCELLANEOUS ----------------------- */
#define OS_ARG_CHK_EN             0    /* Enable (1) or Disable (0) argument checking                  */
#define OS_CPU_HOOKS_EN           1    /* hooks are found in the processor port files         */

#if OS_TIME_SLICE_EN > 0
#define OS_LOWEST_PRIO            (0)    /* Defines the lowest priority that can be assigned ...         */
#else
#define OS_LOWEST_PRIO            (0+OS_CPU_CORE-1)/* Defines the lowest priority that can be assigned   */
#endif

#define  OS_IDLE_PRIO            (OS_LOWEST_PRIO)     /* IDLE      task priority                     */

#define OS_MAX_TASKS              31    /* Max. number of tasks in your application, MUST be >= 2       */

#define OS_SCHED_LOCK_EN          1    /*  Include code for OSSchedLock() and OSSchedUnlock()       */

#define OS_TICKS_PER_SEC          100    /* Set the number of ticks in one second                        */

#define OS_PARENT_TCB             1    /* 是否记录父任务的TCB */

#define OS_CHILD_TCB              0    /* 是否记录子任务的TCB */

/* ----------------------TASK MESSAGE QUEUES ---------------------- */
#define OS_TASKQ_EN               1   /* Enable (1) or Disable (0) code generation for QUEUES         */
#define OS_TASKQ_ACCEPT_EN        1    /*     Include code for OSTaskQAccept()                             */
#define OS_TASKQ_PEND_EN          1    /*     Include code for OSTaskQAccept()                             */
#define OS_TASKQ_FLUSH_EN         1    /*     Include code for OSTaskQFlush()                              */
#define OS_TASKQ_POST_EN          1    /*     Include code for OSTaskQPost()                               */
#define OS_TASKQ_POST_FRONT_EN    1    /*     Include code for OSTaskQPostFront()                          */
#define OS_TASKQ_QUERY_EN         1    /*     Include code for OSTaskQQuery()                              */

/* ---------------- MUTUAL EXCLUSION SEMAPHORES --------------- */
#define OS_MUTEX_EN               1    /* Enable (1) or Disable (0) code generation for MUTEX          */
#define OS_MUTEX_ACCEPT_EN        1    /*     Include code for OSMutexAccept()                         */
#define OS_MUTEX_DEL_EN           1    /*     Include code for OSMutexDel()                            */
#define OS_MUTEX_QUERY_EN         0    /*     Include code for OSMutexQuery()                          */

/* ------------------------ SEMAPHORES ------------------------ */
#define OS_SEM_EN                 1    /* Enable (1) or Disable (0) code generation for SEMAPHORES     */
#define OS_SEM_ACCEPT_EN          1    /*    Include code for OSSemAccept()                            */
#define OS_SEM_DEL_EN             1    /*    Include code for OSSemDel()                               */
#define OS_SEM_QUERY_EN           0    /*    Include code for OSSemQuery()                             */
#define OS_SEM_SET_EN             1    /*    Include code for OSSemSet()                               */

/* ---------------------- MESSAGE QUEUES ---------------------- */
#define OS_Q_EN                   1    /* Enable (1) or Disable (0) code generation for QUEUES         */
#define OS_Q_ACCEPT_EN            1    /*     Include code for OSQAccept()                             */
#define OS_Q_DEL_EN               1    /*     Include code for OSQDel()                                */
#define OS_Q_FLUSH_EN             1    /*     Include code for OSQFlush()                              */
#define OS_Q_POST_EN              1    /*     Include code for OSQPost()                               */
#define OS_Q_POST_FRONT_EN        1    /*     Include code for OSQPostFront()                          */
#define OS_Q_POST_OPT_EN          1    /*     Include code for OSQPostOpt()                            */
#define OS_Q_QUERY_EN             0    /*     Include code for OSQQuery()                              */

/* --------------------- TASK MANAGEMENT ---------------------- */
#define OS_TASK_CHANGE_PRIO_EN    1    /*     Include code for OSTaskChangePrio()                      */
#define OS_TASK_CREATE_EN         1    /*     Include code for OSTaskCreate()                          */
#define OS_TASK_DEL_EN            1    /*     Include code for OSTaskDel()                             */
#define OS_TASK_QUERY_EN          0    /*     Include code for OSTaskQuery()                           */
#define OS_TASK_SUSPEND_EN        1    /*     Include code for OSTaskSuspend() and OSTaskResume()      */
#define OS_TASK_SW_HOOK_EN        0    /*     Include code for OSTaskSwHook()                          */


/* --------------------- TIME MANAGEMENT ---------------------- */
#define OS_TIME_DLY_HMSM_EN       0    /*     Include code for OSTimeDlyHMSM()                         */
#define OS_TIME_DLY_RESUME_EN     1    /*     Include code for OSTimeDlyResume()                       */
#define OS_TIME_GET_SET_EN        1    /*     Include code for OSTimeGet() and OSTimeSet()             */
#define OS_TIME_TICK_HOOK_EN      0    /*     Include code for OSTimeTickHook()                        */


#endif
