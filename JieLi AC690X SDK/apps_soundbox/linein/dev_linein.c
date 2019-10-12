#include "sdk_cfg.h"
#include "dev_linein.h"
#include "dev_manage/device_driver.h"
#include "common/msg.h"
#include "linein.h"

//全局变量
AUX_VAR g_aux_var; ///<line in检测变量


/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 在线检测实体函数
   @param  cnt：检测滤波次数
   @return 在线情况
   @note   AUX_STATUS linein_check(u8 cnt)
*/
/*----------------------------------------------------------------------------*/
AUX_STATUS linein_check(u8 cnt)
{
    u16 cur_aux_status;

    cur_aux_status = AUX_IN_CHECK; //获取当前AUX状态
    if(cur_aux_status != g_aux_var.pre_status)
    {
        g_aux_var.pre_status = cur_aux_status;
        g_aux_var.status_cnt = 0;
    }
    else
    {
        g_aux_var.status_cnt++;
    }

#ifdef AUX_OUT_WITHOUT_CNT
///*//////拔出不去抖////////
    if((AUX_ON == g_aux_var.bDevOnline) && g_aux_var.pre_status)
    {
        g_aux_var.bDevOnline = AUX_OFF;
        return AUX_OFF;
    }
///*/////////////////////
#endif

    if(g_aux_var.status_cnt < cnt) //消抖
    {
        return NULL_AUX;
    }
    g_aux_var.status_cnt = 0;

    ///检测到AUX插入
    if((AUX_OFF == g_aux_var.bDevOnline) && (!g_aux_var.pre_status))
    {
        g_aux_var.bDevOnline = AUX_ON;
        return AUX_ON;
    }
#ifndef AUX_OUT_WITHOUT_CNT
    ///检测到AUX拔出
    else if((AUX_ON == g_aux_var.bDevOnline) && g_aux_var.pre_status)
    {
        g_aux_var.bDevOnline = AUX_OFF;
        return AUX_OFF;
    }
#endif

    return NULL_AUX;
}


/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 在线检测调度函数
   @param  无
   @return 在线情况
   @note   s32 aux_detect(void)
*/
/*----------------------------------------------------------------------------*/
s32 aux_detect(void)
{
    u32 dev_cur_sta = DEV_HOLD;
    AUX_STATUS res;

    res = linein_check(20); //aux在线检测，去抖计数为50
    if(AUX_OFF == res)
    {
        aux_puts("AUX_off\n");
        dev_cur_sta = DEV_OFFLINE;
    }
    else if(AUX_ON == res)
    {
        aux_puts("AUX_on\n");
        dev_cur_sta = DEV_ONLINE;
    }

    if(g_aux_var.last_sta == dev_cur_sta)
    {
        return DEV_HOLD;//设备状态保持不变
    }

    g_aux_var.last_sta = dev_cur_sta;

    return dev_cur_sta;
}


/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 在线状态
   @param  无
   @return LINE IN在线情况
   @note   u32 aux_is_online(void)
*/
/*----------------------------------------------------------------------------*/
u32 aux_is_online(void)
{
    return g_aux_var.bDevOnline;
}


/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 模块初始化函数
   @param  无
   @return 无
   @note   void aux_init_api(void)
*/
/*----------------------------------------------------------------------------*/
void aux_init_api(void)
{
    /*linein check port set*/
#if AUX_IO_DET
    AUX_DIR_SET;
    AUX_PU_SET;
#endif
    my_memset((u8 *)&g_aux_var, 0, sizeof(AUX_VAR));
}


/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 在线列表
   @note   const u8 aux_event_tab[]
*/
/*----------------------------------------------------------------------------*/
const u8 aux_event_tab[] =
{
    SYS_EVENT_AUX_OUT, //LINE IN拔出
    SYS_EVENT_AUX_IN,  //LINE IN插入
};


/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 在线检测API函数
   @param  无
   @return 无
   @note   void aux_check_api(void)
*/
/*----------------------------------------------------------------------------*/
void aux_check_api(u32 info)
{
    s32 dev_status;

    dev_status = aux_detect();
    if(dev_status != DEV_HOLD)
    {
        os_taskq_post_event(MAIN_TASK_NAME, 2, aux_event_tab[dev_status],info);
    }
}
