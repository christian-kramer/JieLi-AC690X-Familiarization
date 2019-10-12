
#include "common/app_cfg.h"
#include "rtos/os_api.h"
#include "rtos/os_cfg.h"
#include "common/error.h"
#include "common/msg.h"
#include "rtos/task_manage.h"
#include "sys_detect.h"
#include "dac/dac_api.h"
#include "play_sel/play_sel.h"
#include "key_drv/key.h"
#include "idle_key.h"
#include "idle.h"
#include "rcsp/rcsp_interface.h"
#include "rcsp/light_head.h"


extern void *malloc_fun(void *ptr,u32 len,char *info);
extern void free_fun(void **ptr);
extern void test_fw_led_mode(void);
extern void light_scene_frame_deal_test(void);
/*----------------------------------------------------------------------------*/
/**@brief  Idle 任务
   @param  p：任务间参数传递指针
   @return 无
   @note   static void idle_task(void *p)
*/
/*----------------------------------------------------------------------------*/
static void light_task(void *p)
{
    int msg[3];

    puts("\n************************light TASK********************\n");
    //led_fre_set(15,0);
    //tone_play(RDEF_LIGHT_TASK_NAME, BPF_RING_MP3); //Idle提示音播放
    tone_play_by_name(RDEF_LIGHT_TASK_NAME,1, BPF_RING_MP3); //line in提示音播放
    os_taskq_post_msg(RDEF_LIGHT_TASK_NAME, 1, MSG_IDLE_MODE); //触发idle模块初始化

    while(1)
    {
        memset(msg,0x00,sizeof(msg));
        os_taskq_pend(0, ARRAY_SIZE(msg), msg);
        clear_wdt();

#if SUPPORT_APP_RCSP_EN
		rcsp_light_task_msg_deal_before(msg);
#endif
		switch(msg[0])
        {
        case SYS_EVENT_DEL_TASK:
            if (os_task_del_req_name(OS_TASK_SELF) == OS_TASK_DEL_REQ)
            {
                puts("IDLE_SYS_EVENT_DEL_TASK\n");
                //play_sel_stop(); //关闭提示音
                os_task_del_res_name(OS_TASK_SELF);
            }
            break;

        case SYS_EVENT_PLAY_SEL_END: //提示音结束
            puts("IDLE_SYS_EVENT_PLAY_SEL_END\n");
            //play_sel_stop(); //关闭提示音
            puts("IDEL_play_tone_end\n");
            os_taskq_post_msg(RDEF_LIGHT_TASK_NAME, 1, MSG_IDLE_MODE);
            break;

        case MSG_IDLE_MODE:
            puts("MSG_IDLE_MODE\n");
//                ui_open_idle(NULL,0);
            //low_power_mode(WAKEUP_13,WAKEUP_DOWN_EDGE);
            break;

        case MSG_IDLE_CTL:
            puts("MSG_IDLE_CTL\n");
            os_taskq_post_msg("MainTask", 1, MSG_EXIT_IDLE_TASK);
            break;

        case MSG_HALF_SECOND:
            ///puts(" light_H ");
//            test_fw_led_mode();
//            light_scene_frame_deal_test();
            break;

        default:
            break;
        }
#if SUPPORT_APP_RCSP_EN
		rcsp_light_task_msg_deal_after(msg);
#endif
	}
}


/*----------------------------------------------------------------------------*/
/**@brief  IDLE 任务创建
   @param  priv：任务间参数传递指针
   @return 无
   @note   static void idle_task_init(void *priv)
*/
/*----------------------------------------------------------------------------*/
static void light_task_init(void *priv)
{
    u32 err;
    err = os_task_create(light_task,
                         (void*)0,
                         TaskIdlePrio,
                         10
#if OS_TIME_SLICE_EN > 0
                         ,1
#endif
                         ,RDEF_LIGHT_TASK_NAME);

    if(OS_NO_ERR == err)
    {
        key_msg_register(RDEF_LIGHT_TASK_NAME, idle_ad_table, idle_io_table, idle_ir_table, NULL);
    }
}


/*----------------------------------------------------------------------------*/
/**@brief  IDLE 任务删除退出
   @param  无
   @return 无
   @note   static void idle_task_exit(void)
*/
/*----------------------------------------------------------------------------*/
static void light_task_exit(void)
{
    if (os_task_del_req(RDEF_LIGHT_TASK_NAME) != OS_TASK_NOT_EXIST)
    {
        os_taskq_post_event(RDEF_LIGHT_TASK_NAME, 1, SYS_EVENT_DEL_TASK);
        do
        {
            OSTimeDly(1);
        }
        while(os_task_del_req(RDEF_LIGHT_TASK_NAME) != OS_TASK_NOT_EXIST);
        puts("del_light_task: succ\n");
    }
}


/*----------------------------------------------------------------------------*/
/**@brief  IDLE 任务信息
   @note   const struct task_info idle_task_info
*/
/*----------------------------------------------------------------------------*/
TASK_REGISTER(light_task_info) =
{
    .name = RDEF_LIGHT_TASK_NAME,
    .init = light_task_init,
    .exit = light_task_exit,
};



#if SUPPORT_APP_RCSP_EN

light_info_t  light_info_tab;
light_disp_t  *light_sence_table;

extern void bt_led_init(void);

void light_init(void)
{
    u32 err;

    P_FUNCTION

    memset(&light_info_tab,0x00,sizeof(light_info_t));
    light_info_tab.color_value[LGT_COLOR_BRIGHT] = 20;//bright

#if RCSP_LIGHT_HW	
    bt_led_init();
#endif

    light_info_tab.button = 1;
    light_info_tab.mode = LGT_MD_WHITE;
    light_info_tab.color_value[LGT_COLOR_WHITE] = 255;
    light_control_set(&light_info_tab,0);
    light_control_set(&light_info_tab,1);

}


void light_exit(void)
{
    P_FUNCTION
    ;
}


#endif //#if
