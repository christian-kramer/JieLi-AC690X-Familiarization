#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__


#define DAC_MUX      2
#define INDATA_MUX   3

#define  TASK_STK_SIZE                0x300       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        5       /* Number of identical tasks                          */


//保留2-10号优先级给客户开发
/*模式任务*/

#define TaskIdlePrio                    12  //Idle任务
#define TaskLineinPrio                  12  //LINE IN任务
#define TaskLineinEqPrio                13  //LINE IN sofe eq任务
#define TaskPcPrio                      12  //PC任务
#define TaskRTCPrio                     12  //RTC任务
#define TaskPcCardPrio                  11  //PC读卡器
#define TaskFMradioPrio                 12	//FM
#define TaskFMsrcPrio                   13	//FM_DAC
#define TaskMusicPrio                   12
#define TaskMusicPhyDecPrio             11
#define TaskEFOutPrio                   13

#define TaskEncodePrio                  12  //ENCODE APP
#define TaskEncRunPrio                  11  //ENCODE RUN
#define TaskEncWFPrio                   10  //ENCODE WRITE FILE

#define TaskEchoMicPrio                 12  //混响任务
#define TaskEchoRunPrio                 9  //混响处理


/*不可相同优先级的 高优先级任务*/
typedef enum
{
   SecondPrio   =  14,
   TaskUIPrio   =  15,
   TaskRcspPrio      ,
   TaskDevMountPrio = 18,
   TaskSbcPrio      ,
   TaskBtEscoPrio   ,
   TaskBtAecPrio    ,
   TaskBtStackPrio  ,
   TaskBTMSGPrio    ,
   TaskBtLmpPrio    ,
   TaskResoursrPrio ,
   TaskPselPhyDecPrio,
   TaskPselPrio     ,
   TaskBtAecSDPrio  ,
   TaskDispPrio     ,
   TaskMainPrio     ,
   TaskDevCheckPrio , //31   31为最大值
}SYS_TASK_PRIO;

/************ÉèÖÃÕ»´óÐ¡£¨µ¥Î»Îª OS_STK £©************/
#define START_TASK_STK_SIZE   1024
#define SECOND_TASK_STK_SIZE   512
#define MP3_TASK_STK_SIZE   0x4000
#define DAC_TASK_STK_SIZE   0x400

#define FILL_TASK_STK_SIZE   0x2000

#define THIRD_TASK_STK_SIZE   0x200
#define UI_TASK_STK_SIZE     0x1000
/*
struct task_req;

struct task_info {
	const char *name;
	void (*init)(void *priv);
	void (*exit)();
};

struct task_req {
	void (*exit_req)(const struct task_info *task);
	int (*switch_req)(const struct task_info *task, const char *name);
};

#define  TASK_REGISTER(task) \
	const struct task_info task __attribute__((section(".task_info")))
*/


#endif

