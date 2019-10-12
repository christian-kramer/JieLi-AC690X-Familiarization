#include "sdk_cfg.h"
#include "rtos/task_manage.h"
#include "rtos/os_api.h"
#include "common/msg.h"
#include "linein/linein.h"
#include "usb_device/usb_device.h"
#include "fm/fm_radio.h"
#include "music/music.h"
#include "file_operate/file_op.h"
#include "dev_pc.h"
#include "linein/dev_linein.h"
#include "idle/idle.h"
#include "common/common.h"
#include "echo/echo.h"
#include "vm/vm.h"
#include "rcsp/rcsp_interface.h"

#if REC_EN
#include "encode/record.h"
#include "encode/encode.h"
#endif
#include "sys_detect.h"

struct task_info *curr_task; ///<当前处于前台运行的任务指针
struct task_info *last_task_info;

extern const struct task_info rtc_task_info;
int check_return_false(void);
extern TASK_REGISTER(btstack_task_info);
extern TASK_REGISTER(bt_hid_task_info);

///任务切换
const struct task_info *task_list[] AT(.task_info);
const struct task_info *task_list[] =
{
    &btstack_task_info,

    #if BT_HID_INDEPENDENT_MODE
        &bt_hid_task_info,
    #endif

    &music_task_info,

#if FM_RADIO_EN
    &fm_radio_task_info,
#endif
    //
    &linein_task_info,
    //
#if USB_PC_EN
    &pc_task_info,
#endif
    //
#if RTC_CLK_EN
    &rtc_task_info, ///<RTC
#endif
    //
#if REC_EN
    &rec_task_info, ///<REC
#endif

#if ECHO_TASK_EN
    &echo_task_info, ///<ECHO
#endif
    //

#if RCSP_LIGHT_HW    
    &light_task_info,
#endif

    &idle_task_info,
};

const struct task_dev_info music_task_2_dev =
{
    .name = "MusicTask",
    .dev_check = file_operate_get_total_phydev,
};

const struct task_dev_info record_task_2_dev =
{
    .name = "RECTask",
    .dev_check = file_operate_get_total_phydev,
};

#if AUX_DETECT_EN
const struct task_dev_info linein_task_2_dev =
{
    .name = "LineinTask",
    .dev_check = aux_is_online,
};
#endif/*AUX_DETECT_EN*/


u32 app_usb_slave_online_status_null(void)
{
   return 0;
}



#if USB_PC_EN
const struct task_dev_info pc_task_2_dev =
{
    .name = "USBdevTask",
    .dev_check = app_usb_slave_online_status,
};
#endif/*USB_PC_EN*/

const struct task_dev_info *task_connect_dev[] AT(.task_info);
const struct task_dev_info *task_connect_dev[] =
{
    &music_task_2_dev,

#if AUX_DETECT_EN
    &linein_task_2_dev,
#endif/*AUX_DETECT_EN*/

#if USB_PC_EN
    &pc_task_2_dev,
#endif/*USB_PC_EN*/

#if REC_EN
    &record_task_2_dev,
#endif/*REC_EN*/

};
const int task_connect_dev_valsize = sizeof(task_connect_dev);

/*----------------------------------------------------------------------------*/
/**@brief  根据任务名字，获取任务指针
 @param  name：任务名字
 @return 指向传入任务名字的指针或者NULL
 @note
 */
/*----------------------------------------------------------------------------*/
static u32 __get_task(const char *name)
{
    u32 max_cnt;
    u32 task_cnt;

    max_cnt = sizeof(task_list) / sizeof(int);

    for (task_cnt = 0; task_cnt < max_cnt; task_cnt++)
    {
        if (0 == strcmp(task_list[task_cnt]->name, name))
        {
            printf("get_task_cnt = %d\n", task_cnt);
            return task_cnt;
        }
    }
    return -1;
}

/*----------------------------------------------------------------------------*/
/**@brief  返回 0
 @param
 @return
 @note   for debug
 */
/*----------------------------------------------------------------------------*/
int check_return_false(void)
{
    return 0;
}
/*----------------------------------------------------------------------------*/
/**@brief  过滤外设检测失败的任务
 @param  : task_cnt--将要切换的任务id
 @return  确定运行的任务id
 @note  如果当前任务外设检测失败会自动切换到下一个任务
 */
/*----------------------------------------------------------------------------*/
static u32 skip_task_without_dev(u32 *task_cnt)
{
    u8 i;
    u32 max_cnt;
    u32 dev_cnt;

    max_cnt = (sizeof(task_list) / sizeof(int)) - 1;
    dev_cnt = sizeof(task_connect_dev) / sizeof(int);
    for (i = 0; i < dev_cnt; i++)
    {
        if (!strcmp(task_list[*task_cnt]->name, task_connect_dev[i]->name))
        {
            if (!task_connect_dev[i]->dev_check())
            {
                (*task_cnt)++;
                if (*task_cnt >= max_cnt)
                {
                    *task_cnt = 0;
                }
                if (skip_task_without_dev(task_cnt))
                    return TRUE;
                else
                    return FALSE;
            }
            else
                return TRUE;
        }
    }

    if (i == dev_cnt)
        return TRUE;

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/**@brief  任务执行
 @param  task_cnt:     任务idle
 @param  priv:     给启动任务的参数
 @param  cur_mode：切换方式
 @return  成功或者错误信息
 @note
 */
/*----------------------------------------------------------------------------*/
static RUN_TASK_STATUS run_task(u32 task_cnt, void *priv, TASK_SWITCH_MODE cur_mode)
{
    u32 max_cnt;

    max_cnt = (sizeof(task_list) / sizeof(int)) - 1;
    if (cur_mode == SWITCH_SPEC_TASK)
    {
        if (task_list[task_cnt] == curr_task) ///<指定运行的任务，与当前正在运行的任务一致
            return RUN_TASK_ERR_HAS_RPT;

    }
    else if (cur_mode == SWITCH_NEXT_TASK)
    {
        task_cnt++;
        if (task_cnt >= max_cnt)
        {
            task_cnt = 0;
        }
    }
    else if (cur_mode == SWITCH_PRE_TASK)
    {
        task_cnt--;
        if (task_cnt == 0)
        {
            task_cnt = max_cnt - 1;
        }
    }
    else if (cur_mode == SWITCH_LAST_TASK)
    {

    }

    skip_task_without_dev(&task_cnt); //忽略设备不在线的任务
    printf("*****Next_Task_Name2:%s\n", task_list[task_cnt]->name);

    task_list[task_cnt]->init((void *) (task_list[task_cnt]->name));
    curr_task = (struct task_info *) task_list[task_cnt];

    return RUN_TASK_SUCC;
}

/*----------------------------------------------------------------------------*/
/**@brief  任务切换
 @param  task_name: 任务的名字
 @param  priv:     给启动任务的参数
 @param  cur_mode：切换方式
 @return  成功或者错误信息
 @note
 */
/*----------------------------------------------------------------------------*/
RUN_TASK_STATUS task_switch(char *task_name, void *priv, TASK_SWITCH_MODE cur_mode)
{
    RUN_TASK_STATUS status;
    struct task_info *task;
    u32 task_cnt;

    //    GET_SYS_MEM_STAT();

    if (task_name != NULL)
    {
        task_cnt = __get_task(task_name);
        if (-1 == task_cnt)
        {
            puts("t_sw_1\n");
            return RUN_TASK_ERR_NO_TAG;
        }
        task = (struct task_info *) task_list[task_cnt];
        if (task == curr_task)
        {
            puts("t_sw_2\n");
            return RUN_TASK_ERR_HAS_RPT;
        }
    }
    else
    {
        if (curr_task != NULL)
        {
            task_cnt = __get_task(curr_task->name);
            if (-1 == task_cnt)
            {
                puts("t_sw_3\n");
                return RUN_TASK_ERR_NO_TAG;
            }
        }
        else
        {
            ///<当前没有运行任务，也没指定需要运行的任务
            return RUN_TASK_ERR_NO_TAG;
        }

    }
    puts("t_sw_4\n");

#if REC_EN
    //stop_rec(rec_op_api); //启动录音
#endif

    if (curr_task != NULL)
    {
        curr_task->exit(); //退出相应任务
        puts("\n\ncurr_task exit!!!\n\n");
        GET_SYS_MEM_STAT();
    }

    if (SWITCH_LAST_TASK == cur_mode)
    {
        puts("last mode\n");
        task_cnt = __get_task(last_task_info->name);
    }
    else
    {
        last_task_info = curr_task;
    }
    ///dac_mute(0,1);    //要完全退出之前的模式再解mute，不然会上个模式的尾音
    status = run_task(task_cnt, priv, cur_mode);

	vm_check_all(1);

#if SUPPORT_APP_RCSP_EN
	rcsp_app_start(curr_task->name);
#endif

	return status;
}
/*----------------------------------------------------------------------------*/
/**@brief  删除一个任务
 @param  task_name：删除任务的名字
 @return
 @note
 */
/*----------------------------------------------------------------------------*/
void os_task_delete(char *task_name)
{
    if (os_task_del_req(task_name) != OS_TASK_NOT_EXIST)
    {
        os_taskq_post_event(task_name, 1, SYS_EVENT_DEL_TASK);
        do
        {
            OSTimeDly(1);
        }
        while (os_task_del_req(task_name) != OS_TASK_NOT_EXIST);
        printf("delete task succ =%s \n", task_name);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  把任务设置为可删除状态
 @return
 @note   此函数由被删除的任务自己调用设置，在释放完占用的资源后，调用此
 函数，进入此函数后，不再返回，整个任务从这里结束
 */
/*----------------------------------------------------------------------------*/
void os_task_ready_delete()
{
    os_task_del_res(OS_TASK_SELF);
}

/*----------------------------------------------------------------------------*/
/**@brief  比较是否为当前运行的任务
 @param    task_name: 比较任务的名字
 @return   1-yes，0-no
 @note
 */
/*----------------------------------------------------------------------------*/
u32 compare_task_name(char *task_name)
{
    if (task_name != NULL)
    {
        if(0 == strcmp(task_name,curr_task->name))
        {
            puts("is curr_task\n");
            return 1;
        }
    }
    puts("not curr_task\n");
    return 0;
}
