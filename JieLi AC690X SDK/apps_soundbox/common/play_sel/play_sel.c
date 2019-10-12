#include "play_sel/play_sel.h"
#include "common/app_cfg.h"
#include "rtos/os_api.h"
#include "rtos/os_cfg.h"
#include "common/error.h"
#include "stdarg.h"
#include "common/msg.h"
#include "rtos/task_manage.h"
#include "dec/music_api.h"
#include "dac/dac_api.h"
#include "dec/decoder_phy.h"
#include "dac/dac.h"
#include "sys_detect.h"

//宏定义
#define PLAY_SEL_TASK_NAME      "play_sel"
#define PLAY_SEL_DEC_PHY_NAME   "play_sel_phy"

//全局变量
static _PLAY_SEL_API g_psel;

extern const char dec_file_ext[][3];

/*----------------------------------------------------------------------------*/
/**@brief  停止提示音播放器解码
   @param  mapi，psel
   @return none
   @note
*/
/*----------------------------------------------------------------------------*/
void play_sel_stop_decoder(_MUSIC_API *mapi, _PLAY_SEL_API *psel)
{
    dsel_printf("dec sel stop\n");
    stop_decoder(&mapi->dec_api);

    if(psel->busy)
    {
        dsel_printf("psel busy\n");
        if(psel->file_par)
        {
            free_fun((void**)&psel->file_par);
        }
        psel->busy = 0;
        if(NULL != psel->father_name)
        {
            os_taskq_post_event((char *)psel->father_name, 1, SYS_EVENT_PLAY_SEL_END);
        }
    }
    else
    {
        dsel_printf("psel idle\n");
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  PP键控制播放器解码
   @param  mapi
   @return 返回控制后的状态
   @note
*/
/*----------------------------------------------------------------------------*/
_DECODE_STATUS play_sel_pp_decoder(_MUSIC_API *mapi)
{
    //mapi->dec_api->phy_ops->status
    return pp_decode(&mapi->dec_api);
}

/*----------------------------------------------------------------------------*/
/**@brief  开始提示音播放器解码
   @param  mapi
   @return none
   @note
*/
/*----------------------------------------------------------------------------*/
void play_sel_play_decoder(_MUSIC_API *mapi)
{
    if(DECODER_PAUSE == mapi->dec_api.phy_ops->status)
    {
        pp_decode(&mapi->dec_api);
    }
}


/*----------------------------------------------------------------------------*/
/**@brief  暂停提示音播放器解码
   @param  mapi
   @return none
   @note
*/
/*----------------------------------------------------------------------------*/
void play_sel_pause_decoder(_MUSIC_API *mapi)
{
    if(DECODER_PLAY == mapi->dec_api.phy_ops->status)
    {
        pp_decode(&mapi->dec_api);
    }
}

static u32 play_sel_api(MUSIC_OP_API * mapi, _PLAY_SEL_API *psel, u32 file_cnt)
{
    u32 err = ERR_MUSIC_API_NULL;
/***********************zhgq 2016-11-03 start******************************/
#ifdef RESTRICT_NOTICE_VOL
	if(BPF_RING_MP3 == psel->file_par->file_number[0])//来电"嘟嘟"	声调大一点
		set_sys_vol(NOTICE_RING_VOL_VALUE, NOTICE_RING_VOL_VALUE, FADE_ON);
	else
		set_sys_vol(NOTICE_VOL_VALUE, NOTICE_VOL_VALUE, FADE_ON);
#endif
/**********************************end************************************/
    if(psel->file_par->file_name)
    {
        mapi->fop_api->fop_init->filepath = (void *)psel->file_par->file_name[file_cnt];
        err = music_play(mapi, DEV_SEL_SPEC,psel->device_sel, PLAY_FILE_BYPATH,&(mapi->dop_api->file_num));
    }
    else if(psel->file_par->file_number)
    {
        mapi->dop_api->file_num = psel->file_par->file_number[file_cnt];
        err = music_play(mapi, DEV_SEL_SPEC, psel->device_sel, PLAY_SPEC_FILE, &(mapi->dop_api->file_num));
    }
    return err;
}
/*----------------------------------------------------------------------------*/
/**@brief  提示音播放器线程函数
   @param  p  传入线程的参数
   @return none
   @note
*/
/*----------------------------------------------------------------------------*/
static void play_sel_task(void *p)
{
    u32 err;
    tbool flag = 0;
    int msg[3];
    u32 file_cnt = 0;
    u32 rpt_time = 0;
    MUSIC_OP_API *mapi = NULL;
    tbool enable = 1;
    _PLAY_SEL_API *psel = p;

    u8 *buf_addr = NULL;
    u32 need_buf;

    dsel_printf("\n func = %s\n", __FUNCTION__);

    msg[0] = 0;

    need_buf = sizeof(MUSIC_OP_API);
    need_buf += sizeof(_MUSIC_API);
    need_buf += sizeof(FILE_OPERATE);
    need_buf += sizeof(DEC_API_IO);
    need_buf += sizeof(FILE_OPERATE_INIT);

    buf_addr = malloc_fun(buf_addr, need_buf, 0);
    if(NULL == buf_addr)
    {
        while(1);
    }

    mapi = (MUSIC_OP_API *)buf_addr;
    buf_addr+=sizeof(MUSIC_OP_API);
//    mapi = malloc_fun(mapi, sizeof(MUSIC_OP_API), 0);
//    if(NULL == mapi)
//    {
//        while(1);
//    }

    /*-------解码接口资源申请----------------*/
    mapi->dop_api = (_MUSIC_API *)(buf_addr);
    buf_addr+=sizeof(_MUSIC_API);
//    mapi->dop_api = malloc_fun(mapi->dop_api, sizeof(_MUSIC_API), 0);
//    if(NULL == mapi->dop_api)
//    {
//        dsel_printf("malloc mapi->dop_api fail\n");
//        psel->busy = 0;
//        goto __run_loop_psel;
//    }

    /*-------文件操作器资源申请--------------*/
    mapi->fop_api = (FILE_OPERATE *)(buf_addr);
    buf_addr+=sizeof(FILE_OPERATE);
//    mapi->fop_api = (void*)malloc_fun(mapi->fop_api, sizeof(FILE_OPERATE), 0);
//    if(NULL == mapi->fop_api)
//    {
//        dsel_printf(" if(NULL==mapi->fop_api) \r\n");
//        while(1);
//    }

    mapi->dop_api->dec_api.format_name = 0; //"WAVWMAMP3";


	mapi->dop_api->dec_api.type_enable = DEC_PHY_MP3 | DEC_PHY_WAV |  \
										 DEC_PHY_WMA | DEC_PHY_FLAC | \
										 DEC_PHY_APE ;//使能响应的解码库

	mapi->dop_api->file_type = (void*)dec_file_ext;

    mapi->dop_api->dec_api.dec_phy_name = PLAY_SEL_DEC_PHY_NAME;

    dsel_printf("build dec phy\n");
    //dsel_printf("mapi  2=%08x  \n",(u32)mapi);
    err = os_task_create_ext(decoder_phy_task, 0, TaskPselPhyDecPrio, 50, mapi->dop_api->dec_api.dec_phy_name, TONE_PHY_TASK_STACK_SIZE);
    if(OS_NO_ERR != err)
    {
        dsel_printf("build dec phy fail %02x\r",err);
        psel->busy = 0;
        goto __run_loop_psel;
    }
//    if(NULL == mapi->dop_api->io)
//    {
//        mapi->dop_api->io = (void *)malloc_fun(mapi->dop_api->io, sizeof(DEC_API_IO), 0);
        mapi->dop_api->io = (DEC_API_IO *)(buf_addr);
        buf_addr+=sizeof(DEC_API_IO);
//        if(NULL == mapi->dop_api->io)
//        {
//            //dsel_printf(" if(NULL == mapi->dec_io) \r\n");
//            psel->busy = 0;
//            dsel_printf("malloc mapi->dop_api->io fail\r");
//            goto __run_loop_psel;
//        }
//    }
    /*-------文件操作器初始化信息资源申请--------------*/
//    mapi->fop_api->fop_init = (void*)malloc_fun(mapi->fop_api->fop_init, sizeof(FILE_OPERATE_INIT), 0);
    mapi->fop_api->fop_init = (FILE_OPERATE_INIT *)(buf_addr);
//    if(mapi->fop_api->fop_init == NULL)
//    {
//        dsel_printf("mem err fop_init \n");
//        goto __run_loop_psel;
//    }
    //mapi->dop_api->file_num = (u32)psel->file_par;
    rpt_time = psel->file_par->rpt_time;
    mapi->dop_api->io->father_name = PLAY_SEL_TASK_NAME;//解码结束的消息返回给谁
    msg[0] = SYS_EVENT_BEGIN_DEC; //触发解码(资源都申请成功)
    flag = 1;

    //puts("----start playsel----\n");
	if(dac_ctl.toggle == 0)
	{
		puts("\n\n###########DAC_ON##############\n\n");
		dac_on_control();
	}
    dac_channel_on(MUSIC_CHANNEL, FADE_ON);
    pa_umute();
/***********************zhgq2013-11-03***********************************/
#ifdef RESTRICT_NOTICE_VOL
	if(BPF_RING_MP3 == psel->file_par->file_number[0])//来电"嘟嘟"	声调大一点
		set_sys_vol(NOTICE_RING_VOL_VALUE, NOTICE_RING_VOL_VALUE, FADE_ON);
	else
		set_sys_vol(NOTICE_VOL_VALUE, NOTICE_VOL_VALUE, FADE_ON);
#else
	set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_ON);
#endif
/************************************************************************/
	audio_sfr_dbg(0);

    while(1)
    {
__run_loop_psel:
        err = 0;
        if(!flag)
        {
            dsel_printf("play sel pend\n");
            os_taskq_pend(0, ARRAY_SIZE(msg), msg);
            dsel_printf("play sel pend~\n");
        }
        flag = 0;
        if((!enable) && (SYS_EVENT_DEL_TASK != msg[0]))
        {
            continue;
        }
        switch(msg[0])
        {
        case SYS_EVENT_BEGIN_DEC:
            if( (psel->file_par) && (file_cnt < psel->file_par->max_file_number) )
            {
                err = play_sel_api(mapi, psel, file_cnt);
				printf("play_sel_api err = %d\n", err);
            }
            else
            {
                play_sel_stop_decoder(mapi->dop_api,psel);
            }
            break;
        case MSG_MUSIC_PP:
            play_sel_pp_decoder(mapi->dop_api);
            break;
        case MSG_MUSIC_PLAY:
            play_sel_play_decoder(mapi->dop_api);
            break;
        case MSG_MUSIC_PAUSE:
            play_sel_pause_decoder(mapi->dop_api);
            break;
        case SYS_EVENT_DEC_FR_END:
        case SYS_EVENT_DEC_FF_END:
        case SYS_EVENT_DEC_END:
			set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_ON);
            dsel_printf("SYS_EVENT_DEC_END\n");
            if(NULL == psel->file_par)
            {
                break;
            }
            file_cnt++;
            if (file_cnt < psel->file_par->max_file_number)
            {
                OSTimeDly(psel->file_par->delay);
                err = play_sel_api(mapi, psel, file_cnt);
            }
            else
            {
                tbool rpt;
                rpt = 0;
                if(0 == psel->file_par->rpt_time)
                {
                    rpt = 1;
                }
                if(rpt_time)
                {
                    rpt_time--;
                    if(rpt_time)
                    {
                        rpt = 1;
                    }
                }
                if(rpt)
                {
                    file_cnt = 0;
                    OSTimeDly(psel->file_par->delay);
                    err = play_sel_api(mapi, psel, file_cnt);
                }
                else
                {
                    play_sel_stop_decoder(mapi->dop_api,psel);
                }

            }
            break;
       case SYS_EVENT_DEC_DEVICE_ERR:
		dsel_printf("SYS_EVENT_DEC_DEVICE_ERR\n");
            play_sel_stop_decoder(mapi->dop_api,psel);
            break;
        case SYS_EVENT_DEL_TASK:
            play_sel_stop_decoder(mapi->dop_api,psel);
            dac_channel_off(MUSIC_CHANNEL, FADE_ON);

            if(mapi->dop_api->dec_api.dec_phy_name)
            {
                if (os_task_del_req(mapi->dop_api->dec_api.dec_phy_name) != OS_TASK_NOT_EXIST)
                {
                    os_taskq_post_event(mapi->dop_api->dec_api.dec_phy_name, 1, SYS_EVENT_DEL_TASK);
                    do
                    {
                        OSTimeDly(1);
                    }
                    while(os_task_del_req(mapi->dop_api->dec_api.dec_phy_name) != OS_TASK_NOT_EXIST);
                    mapi->dop_api->dec_api.dec_phy_name = NULL;
                    puts("del music dec phy succ\n");
                }
            }
            enable = 0;
//            ///<释放文件操作器初始化信息
//            free_fun((void**)&mapi->fop_api->fop_init);
//            free_fun((void**)&mapi->fop_api);
//            ///<关闭文件操作器，释放资源
//
//            free_fun((void**)&mapi->dop_api->io);
//            free_fun((void**)&mapi->dop_api);

            ///<关闭文件操作器，释放资源
			file_operate_ctl(FOP_CLOSE_LOGDEV,mapi->fop_api,0,0);

            free_fun((void**)&mapi);
            if (os_task_del_req_name(OS_TASK_SELF) == OS_TASK_DEL_REQ)
            {
                os_task_del_res_name(OS_TASK_SELF); 	//确认可删除，此函数不再返回
            }

            break;
        default:
            dsel_printf("psel default %08x\r",msg[0]);
            break;
        }
        while(0 != err)
        {
            if(SUCC_MUSIC_START_DEC == err)
            {
                err = 0;
                break;
            }
            dsel_printf("************************\r       psel default %08x\r\n",err);
            play_sel_stop_decoder(mapi->dop_api,psel);
            err = 0;
        }

    }
}

/*----------------------------------------------------------------------------*/
/**@brief  创建提示音播放器线程
   @param  priv  传入线程的参数
   @return none
   @note
*/
/*----------------------------------------------------------------------------*/
static void psel_task_init(void *priv)
{
    u32 err;
    _PLAY_SEL_API *psel = priv;

    psel->alive = 1;
    psel->busy = 1;

    err = os_task_create_ext(play_sel_task, priv, TaskPselPrio, 50, PLAY_SEL_TASK_NAME, TONE_TASK_STACK_SIZE);
    if(OS_NO_ERR == err)
    {

        dsel_printf("build play sel succ\n");
        //key_msg_register(PLAY_SEL_TASK_NAME,music_ad_table,music_io_table,music_ir_table,NULL);
    }
    else
    {
        psel->alive = 0;
        psel->busy = 0;
        dsel_printf("build play sel fail\n");
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  退出提示音播放器线程
   @param  none
   @return none
   @note
*/
/*----------------------------------------------------------------------------*/
static void psel_task_exit()
{
    if (os_task_del_req(PLAY_SEL_TASK_NAME) != OS_TASK_NOT_EXIST)
    {
        os_taskq_post_event(PLAY_SEL_TASK_NAME, 1, SYS_EVENT_DEL_TASK);
        do
        {
            OSTimeDly(1);
        }
        while(os_task_del_req(PLAY_SEL_TASK_NAME) != OS_TASK_NOT_EXIST);
        puts("del_music_task: succ\n");
    }
	set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_ON);
}


/*----------------------------------------------------------------------------*/
/**@brief  播放磁盘提示音
   @param  task_name 调用语音播放的的线程
           device_sel 盘符
           file_par 播放器参数
   @return TRUE OR FALSE
   @note
*/
/*----------------------------------------------------------------------------*/
tbool play_sel(void *name, u32 device_sel, _PLAY_SEL_PARA *file_par)
{
    u8 *buf_addr = NULL;
    u32 need_buf;

    dsel_printf("\n func = %s\n", __FUNCTION__);

    if(g_psel.alive)
    {
        dsel_printf("play sel ~\r");
        psel_task_exit();
        g_psel.alive = 0;
    }
    if(NULL == file_par)
    {
        return FALSE;
    }
    if(NULL != g_psel.file_par)
    {
        return FALSE;
    }

    need_buf = sizeof(_PLAY_SEL_PARA)+(file_par->max_file_number * 4);
    buf_addr = (void *)malloc_fun(buf_addr, need_buf, 0);

    if(NULL == buf_addr)
    {
        dsel_printf("play sel fail 00\r");
        return FALSE;
    }

    g_psel.file_par = (void *)((u32)buf_addr);
    buf_addr += sizeof(_PLAY_SEL_PARA);
    memcpy(g_psel.file_par, file_par, sizeof(_PLAY_SEL_PARA));

    if(file_par->file_number)
    {
        dsel_puts("---file_number---\n");
        g_psel.file_par->file_number = (void *)buf_addr;
        memcpy(g_psel.file_par->file_number, file_par->file_number, file_par->max_file_number * 4);
    }
    else if(file_par->file_name)
    {
        dsel_puts("---file_name---\n");
        g_psel.file_par->file_name = (void *)buf_addr;
        memcpy(g_psel.file_par->file_name, file_par->file_name, file_par->max_file_number * 4);
    }
	else
	{
		free(buf_addr);
        return FALSE;
	}

    dsel_printf("play sel a\n");
	if(device_sel == 0)//default 'A'
	{
		device_sel = 'A';
	}
    g_psel.device_sel = device_sel;
    //g_psel.file_par = file_par;
    g_psel.father_name = name;
    psel_task_init(&g_psel);
    dsel_printf("\nafter psel_task_init\n");
    if(0 == g_psel.alive)
    {
//        free_fun((void**)&g_psel.file_par->file_number);
//        free_fun((void**)&g_psel.file_par->file_name);
        free_fun((void**)&g_psel.file_par);
        printf("----play_sel_free_fun2 = 0x%x----\n",g_psel.file_par);
//        free_fun((void**)&g_psel);
    }
    return TRUE;
}

/*----------------------------------------------------------------------------*/
/**@brief  检查播放器忙闲状态
   @param  none
   @return TRUE OR FALSE
   @note
*/
/*----------------------------------------------------------------------------*/
tbool play_sel_busy(void)
{
    return g_psel.busy;
}

/*----------------------------------------------------------------------------*/
/**@brief  停止播放器
   @param  none
   @return TRUE OR FALSE
   @note
*/
/*----------------------------------------------------------------------------*/
tbool play_sel_stop(void)
{
    if(g_psel.alive)
    {
        dsel_printf("play sel ~\r");
        psel_task_exit();
        g_psel.alive = 0;
    }
    return true;
}

/*
MSG_MUSIC_PP
MSG_MUSIC_PLAY
MSG_MUSIC_PAUSE
*/

/*----------------------------------------------------------------------------*/
/**@brief  播放控制键消息处理
   @param  msg
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void play_sel_pp(u32 msg)
{
    if (os_task_del_req(PLAY_SEL_TASK_NAME) != OS_TASK_NOT_EXIST)
    {
        os_taskq_post_event(PLAY_SEL_TASK_NAME, 1, msg);
        OSTimeDly(1);
        //puts("del_music_task: succ\n");
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  语音提示播放函数
   @param  task_name 调用语音播放的的线程
           file_num 语音播放文件好
   @return
   @note
           播放结束后会发出SYS_EVENT_PLAY_SEL_END 消息
*/
/*----------------------------------------------------------------------------*/
tbool tone_play(void *task_name, u32 file_num)
{
    tbool psel_enable = 0;
    _PLAY_SEL_PARA *psel_p = NULL;
    tbool b_res;

    dsel_printf("\n func = %s\n", __FUNCTION__);

    /*
        if(play_sel_busy() != 0)
        {
            puts("warnning:tone is playing...\n");
        }
    */
    psel_p = malloc_fun(psel_p, sizeof(_PLAY_SEL_PARA), 0);
    if(NULL != psel_p)
    {
        psel_p->file_number = malloc_fun(psel_p->file_number, sizeof(psel_p->file_number[0]), 0);
        if(NULL != psel_p->file_number)
        {
            psel_enable = 1;
            psel_p->max_file_number = 1;
            psel_p->delay = 1;
            psel_p->rpt_time = 1;
            psel_p->file_number[0] = file_num;
        }
        else
        {
            free_fun((void**)&psel_p);
        }
    }

    b_res = play_sel(task_name, 'A', psel_p);
    dsel_printf("\nafter play_sel\n");
    if(psel_enable)
    {
        free_fun((void**)&psel_p->file_number);
        free_fun((void**)&psel_p);
    }
    if(!b_res)
    {
        psel_enable = 0;
    }

    return psel_enable;
}

/*----------------------------------------------------------------------------*/
/**@brief  语音提示播放函数（根据文件名播放）
   @param  task_name 调用语音播放的的线程
           file_num 语音播放文件名
   @return
   @note
           播放结束后会发出SYS_EVENT_PLAY_SEL_END 消息
*/
/*----------------------------------------------------------------------------*/
/* tbool tone_play_by_name(void *task_name, void * file_name) */
tbool tone_play_by_name(void *task_name, u32 total_file, ...)
{
    tbool psel_enable = 0;
    _PLAY_SEL_PARA *psel_p = NULL;
    tbool b_res;
	u32 i;

    va_list argptr;
    va_start(argptr, total_file);

    dsel_printf("\n func = %s\n", __FUNCTION__);

    /*
        if(play_sel_busy() != 0)
        {
            puts("warnning:tone is playing...\n");
        }
    */
    psel_p = malloc_fun(psel_p, sizeof(_PLAY_SEL_PARA), 0);
    if(NULL != psel_p)
    {
        psel_p->file_name = malloc_fun(psel_p->file_name, sizeof(psel_p->file_name[0]) * total_file, 0);
        if(NULL != psel_p->file_name)
        {
            psel_enable = 1;
            psel_p->max_file_number = total_file;
            psel_p->delay = 1;
            psel_p->rpt_time = 1;

			for(i = 0; i<total_file; i++)
			{
            	psel_p->file_name[i] =(void **)va_arg(argptr,int);
				printf("file name = %s\n", psel_p->file_name[i]);
			}
        }
        else
        {
            free_fun((void**)&psel_p);
        }
    }

    b_res = play_sel(task_name, 'A', psel_p);
    dsel_printf("\nafter play_sel\n");
    if(psel_enable)
    {
        free_fun((void**)&psel_p->file_name);
        free_fun((void**)&psel_p);
    }
    if(!b_res)
    {
        psel_enable = 0;
    }
    va_end(argptr);

    return psel_enable;
}

void * tone_number_get_name(u32 number)
{
    switch(number)
    {
        case 0:return BPF_0_MP3;
        case 1:return BPF_1_MP3;
        case 2:return BPF_2_MP3;
        case 3:return BPF_3_MP3;
        case 4:return BPF_4_MP3;
        case 5:return BPF_5_MP3;
        case 6:return BPF_6_MP3;
        case 7:return BPF_7_MP3;
        case 8:return BPF_8_MP3;
        case 9:return BPF_9_MP3;
        default:return 0;
    }
    return 0;
}

