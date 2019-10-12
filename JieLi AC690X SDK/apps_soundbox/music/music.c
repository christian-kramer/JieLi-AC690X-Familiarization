#include "sdk_cfg.h"
#include "common/app_cfg.h"
#include "rtos/os_api.h"
#include "rtos/os_cfg.h"
#include "common/error.h"
#include "stdarg.h"
#include "music.h"
#include "common/msg.h"
#include "rtos/task_manage.h"
#include "music_key.h"
#include "dec/music_api.h"
#include "vm/vm_api.h"
#include "play_sel/play_sel.h"
#include "dac/dac_api.h"
#include "dev_manage/drv_manage.h"
#include "ui/ui_api.h"
#include "ui/led/led.h"
#include "key_drv/key.h"
#include "music_prompt.h"
#include "music_info.h"
#include "music_ui.h"
#include "fat/tff.h"
#include "fat/fs_io.h"
#include "common/common.h"
#include "lyrics_api.h"
#include "dac/eq_api.h"
#include "dac/dac_api.h"
#include "dac/dac.h"
#include "dac/eq.h"
#include "music_ab_rpt.h"
#include "music_decrypt.h"
#include "clock_interface.h"
#include "clock.h"
#include "dec/decoder_phy.h"
#include "vm/vm.h"
#include "rcsp/rcsp_interface.h"
#include "vm/Vm_api.h"
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD    voice different  start//////////////////////////////////
#include "dev_manage\drv_manage.h"
//////////////////////USB SD   voice different     end//////////////////////////////////
#endif
extern void ble_server_disconnect(void);
extern u32 file_operate_get_total_lgdev();
extern void *sup_dec_phy_op(void *priv  ,void *data, u32 len);
extern u32 get_decode_time(void);
extern const char dec_file_ext[][3];
const struct task_info music_task_info;

//FS_DISP_NAME_T *music_play_display;
//char *music_play_tpath;
FS_DISP_NAME music_name_buf;//buf need malloc


MUSIC_DIS_VAR music_ui;
void *music_reverb;


MUSIC_OP_API *music_operate_mapi=NULL;


//const char rpt_str[][7] =
//{
//    {"ALL"},
//    {"FOLDER"},
//    {"ONE"},
//    {"RANDOM"}
//};

const char rpt_str[][10] =
{
    {"ALL"},
    {"ONE_DEV"},
    {"ONE_FILE"},
    {"RANDOM"},
    {"FOLDER"},
};



//#define MUSIC_TONE_NUM 1
#define MUSIC_DEC_PHY_NAME "MusicPhyDec"
#define EF_OUT_NAME    "ef_out_task"
/******************************************/
#ifdef MEMERY_MODE
extern u8 curr_work_mode;
#endif
u8 curr_device;
u8 getting_music_info;
bool g_device_in_flag = 0;//Ö¸¶¨Éè±¸
bool music_pp_flag=0;
/****************************************/
/**********************************start**********************************/
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD      voice different      start//////////////////////////////////
bool dec_dev_changed = 0;
extern u8 usb_in_flag;
extern u8 sd_in_flag;

extern u32 try_play_dev(void);

u32 curr_device_using;
u8 power_on_flag=0;
extern u32 dev_get_online_status(void * parm);
extern DEV_HANDLE volatile sd0;        ///Éè±¸¾ä±ú
extern DEV_HANDLE volatile sd1;        ///Éè±¸¾ä±ú
extern DEV_HANDLE volatile usb;        ///Éè±¸¾ä±ú
//////////////////////USB SD     voice different      end//////////////////////////////////
#endif
/********************************voice different end**************************************/
/**********************************voice different  start**********************************/
bool music_play_voice_flag=0;
bool lowbat_play_voice_flag=0;
#if MAX_VOL_VOICE
extern u8 key_short_cnt;
#endif
/**********************************end*****************************/
u8 play_mode = REPEAT_ALL;
u8 eq_mode = 0;

void music_enter_updata(void)
{
	echo_exit_api(&music_reverb);
}

/*----------------------------------------------------------------------------*/
/**@brief  music解码初始化
   @param  mapi_out -- 解码模块指针
   @return 0-成功,非0失败
   @note
*/
/*----------------------------------------------------------------------------*/
int music_decoder_init(MUSIC_OP_API **mapi_out)
{
    MUSIC_OP_API *mapi = NULL;
    int err_id = 0;
    u32 err;
    do
    {
        mapi = malloc_fun(mapi,sizeof(MUSIC_OP_API),0);
        if(NULL == mapi)
        {
            err_id = 1;
            break;
        }
        /*-------解码接口资源申请----------------*/
        mapi->dop_api=malloc_fun(mapi->dop_api,sizeof(_MUSIC_API),0);
        if(NULL==mapi->dop_api)
        {
            dec_phy_printf(" if(NULL==mapi->dop_api) \r\n");
            err_id = 2;
            break;
        }
        /*-------文件操作器资源申请--------------*/
        mapi->fop_api=malloc_fun(mapi->fop_api,sizeof(FILE_OPERATE),0);
        if(NULL==mapi->fop_api)
        {
            dec_phy_printf(" if(NULL==mapi->fop_api) \r\n");
            err_id = 3;
            break;
        }
	
        mapi->dop_api->dec_api.type_enable = DEC_PHY_MP3 | DEC_PHY_WAV |  \
											 DEC_PHY_WMA | DEC_PHY_FLAC | \
											 DEC_PHY_APE ;//使能响应的解码库

        mapi->dop_api->file_type = (void*)dec_file_ext;

        mapi->dop_api->dec_api.dec_phy_name = MUSIC_DEC_PHY_NAME;

        err = os_task_create_ext(decoder_phy_task, 0, TaskMusicPhyDecPrio, 5, mapi->dop_api->dec_api.dec_phy_name, MUSIC_PHY_TASK_STACK_SIZE);
        if(OS_NO_ERR == err)
        {
            dec_phy_printf("creat decoder_phy_task ok \n");
        }
        else
        {
            printf("decoder_phy_task err=%08x\n",err);
            err_id = 4;
            break;
        }

        if(NULL == mapi->dop_api->io)
        {
            mapi->dop_api->io = malloc_fun(mapi->dop_api->io,sizeof(DEC_API_IO),0);
            if(NULL == mapi->dop_api->io)
            {
                dec_phy_printf(" if(NULL == mapi->dop_api->dec_io) \r\n");
                err_id = 5;
                break;
            }
        }

        mapi->dop_api->io->father_name = "MusicTask";//解码结束的消息返回给谁
        mapi->dop_api->file_num = 1;
        mapi->dop_api->dec_api.eq = eq_mode;

        //psel_enable = tone_play_by_name("MusicTask", BPF_MUSIC_MP3);
        /*-------文件操作器初始化信息资源申请--------------*/
        mapi->fop_api->fop_init = malloc_fun(mapi->fop_api->fop_init,sizeof(FILE_OPERATE_INIT),0);
        if(mapi->fop_api->fop_init==NULL)
        {
            dec_phy_printf("mem err fop_init \n");
            err_id = 6;
            break;
        }
        mapi->fop_api->fop_init->cur_play_mode = play_mode;
        mapi->dop_api->brk = malloc_fun(mapi->dop_api->brk,sizeof(FS_BRK_POINT),0);
        mapi->dop_api->dec_api.bp_buff = &mapi->dop_api->brk->inf.norm_brk_buff[0];
        mapi->dop_api->dec_api.flac_bp_buff = &mapi->dop_api->brk->inf.flac_brk_buff[0];

    }while(0);

    if(err_id != 0)
    {
        printf("decoder init err = %d\n",err_id);
        while(1);
    }

    *mapi_out = mapi;

    return err_id;
}

/*----------------------------------------------------------------------------*/
/**@brief  文件解码前回调函数处理
   @param  parm -- 解码模块指针,dev_let-逻辑盘符
   @return void
   @note
*/
/*----------------------------------------------------------------------------*/
void music_stop_decoder(MUSIC_OP_API *parm)
{
    _MUSIC_API *mapi;
	u32 err;
	puts("stop dec in\n");
	mapi=parm->dop_api;
	err =stop_decoder(&mapi->dec_api);
    if(STOP_DEC_GOT_BRKINFO & err)
    {
        file_operate_ctl(FOP_SAVE_BPINFO,parm->fop_api,&mapi->brk->inf,0);
    }
    else if(STOP_DEC_GOT_FLACBRKINFO & err)
    {
        file_operate_ctl(FOP_SAVE_FLACBPINFO,parm->fop_api,&mapi->brk->inf,0);
    }
#if USB_DISK_EN
    usb->dev_io->ioctrl(0,DEV_SET_REMAIN);
#endif
	puts("stop dec out\n");
}

/*----------------------------------------------------------------------------*/
/**@brief  文件解码前回调函数处理
   @param  m_op_api -- 解码模块指针，dev_let-逻辑盘符
   @return void
   @note
*/
/*----------------------------------------------------------------------------*/
void before_decoder_callback(MUSIC_OP_API *m_op_api,u32 dev_let)
{
    music_ui.lrc_flag = FALSE;
    if(dev_let != 'A')
    {
#if LRC_LYRICS_EN
        if(0 == lrc_find(m_op_api))
        {
            music_ui.lrc_flag = TRUE;
        }
        else
        {
            printf("lrc_init err\n");
        }
#endif

    }
    else
    {
        puts("play a music\n");
    }

#if MUSIC_DECRYPT_EN
	get_decode_file_info(m_op_api->dop_api->io->f_p);
#endif

	vm_check_all(0);
}

void after_decoder_callback(MUSIC_OP_API *m_op_api,u32 dev_let)
{
	if(0 == strcmp(m_op_api->dop_api->dec_api.phy_ops->dec_ops->name,"FLA"))//解码器FLAC
	{
		set_sys_freq(FLAC_SYS_Hz);
	}
	else if(0 == strcmp(m_op_api->dop_api->dec_api.phy_ops->dec_ops->name,"APE"))//解码器APE
	{
		set_sys_freq(APE_SYS_Hz);
	}
	else
	{
		set_sys_freq(SYS_Hz);
	}
}


/*----------------------------------------------------------------------------*/
/**@brief  文件播放总入口
   @param  m_op_api -- 解码模块指针，dev_sel- 磁盘操作模式 dev_let-逻辑盘符，
   @param  file_sel-文件选择模式，pra--参数
   @return 0-成功,非0失败
   @note
*/
/*----------------------------------------------------------------------------*/
u32 music_play_api(MUSIC_OP_API *m_op_api,ENUM_DEV_SEL_MODE dev_sel,u32 dev_let,u32 file_sel,void *pra)
{
    u32 err;
    u8 tmp8;
	u32 time_out;

   puts("play api in\n");
//   reg_get_id3head(music_get_file_id3);
    music_ui.opt_state = MUSIC_OPT_BIT_SEL;


	GET_SYS_MEM_STAT();

#if SUPPORT_APP_RCSP_EN
	rcsp_music_play_api_before(dev_sel,dev_let);
#endif
	puts("111111\n");
	err = music_play(m_op_api, dev_sel, dev_let, (ENUM_FILE_SELECT_MODE)file_sel, pra);
#ifdef MUSIC_LED_PLAYONE_FLASH
		if(play_mode == REPEAT_ONE)
			led_fre_set(5,0);
		else
#endif
        	led_fre_set(15,0);
	pa_umute();
#if SUPPORT_APP_RCSP_EN
	rcsp_music_play_api_after(dev_sel,dev_let,err);
#endif

#if 1//UI_ENABLE
    u8  last_device = music_ui.ui_curr_device;
    if((err != 0)&&(err != SUCC_MUSIC_START_DEC))
    {
        UI_menu(MENU_WAIT);
    }
    else
    {
    
		puts("111111\n");
        //get device
        music_ui.ui_curr_device = (u32)file_operate_ctl(FOP_GET_PHYDEV_INFO,m_op_api->fop_api,0,0);

		music_ui.ui_curr_statu = music_ui.ui_curr_statu = (u32*)&m_op_api->dop_api->dec_api.phy_ops->status;
    	music_ui.ui_curr_file = (u32)m_op_api->dop_api->file_num;
		music_ui.ui_total_file =m_op_api->fop_api->cur_lgdev_info->total_file_num;
		// music_ui.ui_total_file = (u32)file_operate_ctl(FOP_GET_TOTALFILE_NUM,m_op_api->fop_api,m_op_api->dop_api->file_type,0);//reflash

		music_ui.play_time = get_decode_time(); 
		if(PLAY_BREAK_POINT == file_sel)
		{
			//断点播放，等待正确时间更新，避免显示00:00
			time_out = 0;
			while(music_ui.play_time == 0)
			{
				os_time_dly(1);
				music_ui.play_time = get_decode_time(); 

				time_out++;
				/* puts("time_out\n"); */
				if(time_out == 100)//os_time_dly 100
				{
					break;
				}
			}
			/* printf("----ui_play_time= %d\n",music_ui.play_time); */
		}

#if LCD_128X64_EN
		music_file_info(m_op_api);//get_file_info
#endif
    	music_ui.opt_state = MUSIC_OPT_BIT_PLAY;
    	UI_menu_mux(MENU_MUSIC_MAIN,MENU_LIST_DISPLAY);
    }
#endif

    puts("play api out\n");
    return err;
}

/*----------------------------------------------------------------------------*/
/**@brief  根据全路径播放文件
   @param  mapi -- 解码模块指针
   @return 0-成功,非0失败
   @note
*/
/*----------------------------------------------------------------------------*/
u32  music_play_by_path(MUSIC_OP_API *mapi)
{
    u32 err;
//    u8 *path;
    const char path[]="/folder1/abc.mp3";
//    path="/音乐/";
    mapi->fop_api->fop_init->filepath = (u8*)path;
//    printf("file path : %s  \n",path);
//    err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_FILE_BYPATH,&(mapi->dop_api->file_num));
    err = music_play_api(mapi,DEV_SEL_SPEC,0,PLAY_FILE_BYPATH,&(mapi->dop_api->file_num));
    return err;
}
/*----------------------------------------------------------------------------*/
/**@brief  根据全路径clust播放文件
   @param  mapi -- 解码模块指针
   @return 0-成功,非0失败
   @note
*/
/*----------------------------------------------------------------------------*/
#if 0
u32  music_play_by_PathClust(MUSIC_OP_API *mapi,u8 *path_clust,u8 path_len)
{
    u32 err;
    u8 *path_buf = malloc(path_len);
    
	if(path_buf == NULL)
	{
		return ERR_MUSIC_NULL_OPT;
	}
	memcpy(path_buf,path_clust,path_len);	
	mapi->fop_api->fop_init->filepath = (u8*)path_buf;
    err = music_play_api(mapi,DEV_SEL_SPEC,0,PLAY_FILE_BYPATH,&(mapi->dop_api->file_num));
    free(path_buf); 
	return err;
}
#endif
/*----------------------------------------------------------------------------*/
/**@brief  根据目录播放文件
   @param  mapi -- 解码模块指针
   @return 0-成功,非0失败
   @note
*/
/*----------------------------------------------------------------------------*/
u32  music_play_by_folder(MUSIC_OP_API *mapi)
{
    u32 err;
    err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_PRE_FOLDER,&(mapi->dop_api->file_num));
    return err;
}

/*----------------------------------------------------------------------------*/
/**@brief  根据文件首簇播放文件
   @param  mapi -- 解码模块指针，sclust-文件首簇
   @return 0-成功,非0失败
   @note
*/
/*----------------------------------------------------------------------------*/
u32 music_play_by_sclust(MUSIC_OP_API *mapi,u32 sclust)
{
    u32 err;
    mapi->fop_api->fop_init->filesclust = sclust;
    err = music_play_api(mapi,DEV_SEL_CUR,0,FOP_OPEN_FILE_BYSCLUCT,&(mapi->dop_api->file_num));
//    printf("mapi->dop_api->file_num = %d\n",mapi->dop_api->file_num);
    return err;
}

extern void *file_operate_get_next_phydev();
DEV_HANDLE music_next_dev_type(void)
{
	DEV_HANDLE next_dev_hl;
	next_dev_hl = (DEV_HANDLE)file_operate_get_next_phydev();
	if(next_dev_hl == usb)
	{
		puts("next_dev:usb\n");
	}
	else if(next_dev_hl == sd0)
	 {
		puts("next_dev:sd\n");
	}
	else 
	{
		puts("next_dev:NULL\n");
	}
	return next_dev_hl;
}
/*----------------------------------------------------------------------------*/
/**@brief  music 主线程
   @param  p -- 输入参数
   @return void
   @note
*/
/*----------------------------------------------------------------------------*/
static void music_task(void *p)
{
    MUSIC_OP_API *mapi=NULL;
    tbool psel_enable = 0;
    u32 err;
    tbool enable = 1;
    tbool file_auto_next = 1;
    volatile u16 wait_exit=0;
    volatile u16 wait_ok=0;
    int msg[6];
    static u32 play_time;
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD  VOICE DIFFERENT  start//////////////////////////////////
	void *device_file_num;
//////////////////////USB SD   VOICE DIFFERENT   end//////////////////////////////////
#endif
    music_reverb = NULL;
    puts("\n************************music TASK********************\n");
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD   VOICE DIFFERENT    start//////////////////////////////////
    if(power_on_flag)   ///FIRST POWER ON FLAG
    {
        power_on_flag=0;
		os_time_dly(20);
		if(dev_get_online_status(usb)!=0)
        {
            ///SD1  ONLINE
            device_file_num = BPF_USB_MP3;
            curr_device_using = (u32)usb;
        }
        else if(dev_get_online_status(sd1)!=0)
        {
            ///USB  ONLINE
            printf("oopp===ooo==\n");
            device_file_num = BPF_SD_MP3;
            curr_device_using = (u32)sd0;
        }
        else //if(dev_get_online_status(sd0)!=0)
        {
            ///SD0   ONLINE
            printf("ootttttt==ooo==\n");
            device_file_num = BPF_SD_MP3;
            curr_device_using = (u32)sd0;
        }
    }
    else
    {
        curr_device_using = try_play_dev(); /////
        {
            if((u32)sd0 == curr_device_using)
            {
                device_file_num = BPF_SD_MP3;
            }
            else //if((u32)usb == curr_device_using)
            {
                device_file_num = BPF_USB_MP3;
            }
        }
        puts("=======pp======\n");
    }
//////////////////////USB SD VOICE DIFFERENT end//////////////////////////////////
#endif
#if MUSIC_DECRYPT_EN
	cipher_init(0x12345678);
#endif // MUSIC_DECRYPT_EN

#if EQ_EN
    eq_enable();
    eq_mode = get_eq_default_mode();
#endif // EQ_EN
#ifdef MUSIC_LED_PLAYONE_FLASH
		if(play_mode == REPEAT_ONE)
			led_fre_set(5,0);
		else
#endif
    led_fre_set(15,0);

     //os_taskq_post_msg("MusicTask",1,SYS_EVENT_PLAY_SEL_END);
//   music_prompt_flag = PROMPT_STATUS_PLAY;
//   music_prompt_stopped = 1;
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD VOICE DIFFERENT  start//////////////////////////////////
	psel_enable = tone_play_by_name("MusicTask", 1, device_file_num);//BPF_MUSIC_MP3);
//////////////////////USB SD VOICE DIFFERENT  end//////////////////////////////////
#else
    psel_enable = tone_play_by_name("MusicTask", 1, BPF_MUSIC_MP3);
#endif
    dec_phy_printf("---music psel_enable %d \r\n",psel_enable);

    music_decoder_init(&mapi);
	music_decoder_info(before_decoder_callback, after_decoder_callback);

#if LRC_LYRICS_EN
    lrc_init();
#endif

	music_operate_mapi = mapi;
    music_name_buf.tpath = NULL;
#ifdef MEMERY_MODE
	curr_work_mode=0;
	curr_work_mode |= BIT(MUSIC_MODE);
	vm_write_api(VM_MODE,&curr_work_mode);
#endif
#if MAX_VOL_VOICE
	key_short_cnt=0;
#endif
	while(1)
    {
        memset(msg,0x00,sizeof(msg));
        os_taskq_pend(0, ARRAY_SIZE(msg), msg);
        if(msg[0] != MSG_HALF_SECOND)
        {
            /* printf("\nmusic_msg= 0x%x\n",msg[0]); */
        }

        clear_wdt();
        //dec_phy_printf("---------music msg %x \r\n",msg[0]);
        err = 0;
        if ( (!enable) && (SYS_EVENT_DEL_TASK != msg[0]))
        {
            continue;
        }
        if(psel_enable)
        {
            switch(msg[0])
            {
            case SYS_EVENT_PLAY_SEL_END:
			case SYS_EVENT_BEGIN_DEC:
                msg[0] = SYS_EVENT_PLAY_SEL_END;
				goto play_sel_end;            	
				break;
            case MSG_MUSIC_NEXT_FILE:
            case MSG_MUSIC_PREV_FILE:
            case MSG_MUSIC_PP:
            case MSG_MUSIC_PLAY:
            case MSG_MUSIC_PAUSE:
                msg[0] = NO_MSG;// SYS_EVENT_PLAY_SEL_END will come next msg;
            case SYS_EVENT_DEL_TASK: 				//请求删除music任务
play_sel_end:      
				puts("\n----play_sel_stop------\n");
                play_sel_stop();
                psel_enable = 0;
                break;
            default:
                msg[0] = NO_MSG;
                break;
            }
        }

        mapi->dop_api->dec_api.save_brk = 0;
        lg_dev_info *tmplgdev;
        if(wait_exit)
        {
            ///<进入删除状态，不再响应其他消息
			if(wait_ok)
			{
				err = os_taskq_post(MAIN_TASK_NAME, 1, MSG_CHANGE_WORKMODE);
				if(err == OS_NO_ERR)
				{
					wait_ok = 0;
				}
			}
            if(msg[0]!=SYS_EVENT_DEL_TASK)
            {
                dec_phy_printf("msg ddddd %08x \r\n",msg[0]);
                continue;
            }
        }


#if SUPPORT_APP_RCSP_EN
		rcsp_music_task_msg_deal_before(msg);
#endif
		switch(msg[0])
        {
        case SYS_EVENT_PLAY_SEL_END:
            dec_phy_printf("notic music end \r\n");
            if(music_prompt_break)
			{
                puts("music_prompt_break\n");
                music_prompt_break = 0;
                break;
            }
#if MAX_VOL_VOICE
			if(key_short_cnt==1)
			{
				key_short_cnt=2;
				break;
			}
#endif
            dac_channel_on(MUSIC_CHANNEL, FADE_ON);
//                SET_UI_MAIN(MENU_MUSIC_MAIN);
//                UI_menu(MENU_WAIT);
            music_ui.mapi = mapi;
            ui_open_music(&music_ui, sizeof(MUSIC_DIS_VAR));
        case SYS_EVENT_BEGIN_DEC:
            mapi->dop_api->dec_api.save_brk = 0;
#ifdef MUSIC_USB_SD_VOICE
//////////////////////start end//////////////////////////////////
			if(lowbat_play_voice_flag)
			{
				lowbat_play_voice_flag=0;
				if(music_pp_flag==0)
					music_play_voice_flag=0;
			}
          	err = prompt_play_end();
			switch(err)
			{
				case 0:
					puts("end res 0\n");
					break;
				case 1:
					puts("end res 1\n");					
					//printf("LGDEV ONLINE--%08x\n",curr_device_using);
            		err = music_play_api(mapi,DEV_SEL_SPEC,curr_device_using,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
					break;
                case 2:
					puts("end res 2\n");
            		err = music_play_api(mapi,DEV_SEL_SPEC,curr_device_using,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
					break;
                case 3:
					puts("end res 3\n");
					err = music_play_api(mapi,DEV_SEL_SPEC,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
					break;
				case 4:
					puts("end res 4\n");
					err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_AUTO_NEXT,&(mapi->dop_api->file_num));
					break;
				default:
					puts("end res err\n");
					break;
			}
////////////////////// end//////////////////////////////////

#else
#if 0//def MUSIC_PROMPT_EN
			if(prompt_play_end())
			{
           		err = music_play_api(mapi,DEV_SEL_SPEC,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
			}
			else
			{
				break;
			}
#else
			if(lowbat_play_voice_flag)
			{
       			lowbat_play_voice_flag=0;
				err = prompt_play_end();
				if(music_pp_flag==0)
				{
					music_play_voice_flag=0;
					err = music_play_api(mapi,DEV_SEL_SPEC,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
				}
			}
			else
			{
				music_pp_flag=0;
				if(g_device_in_flag)
				{
					g_device_in_flag = 0;
					err = music_play_api(mapi,DEV_SEL_SPEC,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
				}
				else
				{
					if(dev_get_online_status(usb)==0)
					{
						err =music_play_api(mapi,DEV_SEL_SPEC,(u32)(sd0),PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
					}
					else
					{
						err = music_play_api(mapi,DEV_SEL_SPEC,(u32)usb,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
					}
				}
			}
#endif
#endif

			break;
        case SYS_EVENT_DEC_SET_END:
            /* puts("----SYS_EVENT_DEC_SET_END---\n"); */
            break;

        case SYS_EVENT_DEC_FR_END:
			puts("----SYS_EVENT_DEC_FR_END---\n");
			break;
        case SYS_EVENT_DEC_FF_END:
			puts("----SYS_EVENT_DEC_FR_END---\n");

        case SYS_EVENT_DEC_END:
            /* puts("----SYS_EVENT_DEC_FR_END---\n"); */

            dec_phy_printf("decoder stop\r\n");
            err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_AUTO_NEXT,&(mapi->dop_api->file_num));
            break;

        case SYS_EVENT_DEV_OFFLINE:
#ifndef MUSIC_USB_SD_VOICE
		if(g_device_in_flag)g_device_in_flag = 0;
#endif
            dec_phy_printf("music SYS_EVENT_DEV_OFFLINE\n");
            tmplgdev = mapi->fop_api->cur_lgdev_info;
            if(tmplgdev)
            {
                if(tmplgdev->lg_hdl->phydev_item == (void *)msg[1])
                {
                    UI_menu(MENU_MUSIC_MAIN);
                    if((!mapi->dop_api->dec_api.phy_ops) || (DECODER_PAUSE != mapi->dop_api->dec_api.phy_ops->status))
                    {
                        ///< 解码设备非暂停状态，有解码自己报错
                        break;
                    }///else的解码暂停状态，有下一个消息SYS_EVENT_DEC_DEVICE_ERR处理
                }
                else
                {
                    ///<非解码设备
                    UI_menu(MENU_MUSIC_MAIN);
                    break;
                }
            }
            else
                break;
        case SYS_EVENT_DEC_DEVICE_ERR:
            puts("********sys device err*********\n");
            mapi->dop_api->dec_api.save_brk = 1;
            mapi->dop_api->file_num = 1;
			music_next_dev_type();
            err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
            break;
        case MSG_MUSIC_NEXT_FILE:
			if(music_pp_flag==1)
				music_pp_flag=0;
            file_auto_next = 1;
#ifdef MUSIC_LED_PLAYONE_FLASH
			if(play_mode == REPEAT_ONE)
					led_fre_set(5,0);
			else
#endif
            led_fre_set(15,0);
            dec_phy_printf("KEY_EVENT_NEXT\r\n");
            err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_NEXT_FILE,&(mapi->dop_api->file_num));

//                err=music_play_by_path(mapi);
//                err=music_play_by_folder(mapi);
            break;
        case MSG_MUSIC_PREV_FILE:
			if(music_pp_flag==1)
				music_pp_flag=0;
            file_auto_next = 0;
#ifdef MUSIC_LED_PLAYONE_FLASH
			if(play_mode == REPEAT_ONE)
				led_fre_set(5,0);
			else
#endif
            led_fre_set(15,0);
            dec_phy_printf("KEY_EVENT_PREV\r\n");
            err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_PREV_FILE,&(mapi->dop_api->file_num));

//                mapi->dop_api->file_num = 1;
            break;
        case MSG_MUSIC_RPT:
#if 0
            if(mapi && mapi->fop_api && mapi->fop_api->fop_init)
            {
                play_mode++;
                if(play_mode >= MAX_PLAY_MODE)
                {
                    play_mode = REPEAT_ALL;
                }
                mapi->fop_api->fop_init->cur_play_mode = play_mode;
            }
#else
			if(play_mode == REPEAT_ALL)
			{
				play_mode = REPEAT_ONE;
				led_fre_set(5,0);
			}
			else
			{
				play_mode = REPEAT_ALL;
           		led_fre_set(15,0);
			}
			mapi->fop_api->fop_init->cur_play_mode = play_mode;
#endif

/* #if LCD_SUPPORT_MENU */
            /* UI_menu(MENU_REFRESH); */
/* #else */
            UI_menu_arg(MENU_SET_PLAY_MODE,play_mode);
/* #endif */
            printf("cur_play_mode : %s\r", &rpt_str[mapi->fop_api->fop_init->cur_play_mode-REPEAT_ALL][0] );
            break;

        case MSG_MUSIC_U_SD:
            puts("MSG_MUSIC_U_SD\r\n");
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD è¯­éŸ³åŒºåˆ† start//////////////////////////////////
            {
                dec_dev_changed=1;

#ifdef MUSIC_LED_PLAYONE_FLASH
				if(play_mode == REPEAT_ONE)
					led_fre_set(5,0);
				else
#endif
                	led_fre_set(15,0);
                mapi->dop_api->dec_api.save_brk = 1;
                printf("MSG_MUSIC_U_SD\r\n");
#ifdef MUSIC_PROMPT_EN
                if((u32)sd0 == curr_device_using)
                {
                    curr_device_using = (u32)usb;
                    music_prompt_play(mapi,BPF_USB_MP3,msg[0]);
                }
                else
                {
                    curr_device_using = (u32)sd0;
                    music_prompt_play(mapi,BPF_SD_MP3,msg[0]);
                }
#else
                err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
#endif
            }
//////////////////////USB SD è¯­éŸ³åŒºåˆ† end//////////////////////////////////
#else
            if(file_operate_get_total_phydev()>1)
            {
                mapi->dop_api->dec_api.save_brk = 1;
#ifdef MUSIC_LED_PLAYONE_FLASH
					if(play_mode == REPEAT_ONE)
						led_fre_set(5,0);
					else
#endif
                led_fre_set(15,0);
                    ///led_fre_set(15,0);
                    ///mapi->dop_api->dec_api.save_brk = 1;
                err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));

            }
#endif
            break;

#if EQ_EN
        case MSG_MUSIC_EQ:
            if(mapi && mapi->dop_api)
            {
                eq_mode++;
                if(eq_mode > 5)
                {
                    eq_mode = 0;
                }
                eq_mode_sw(eq_mode);
                printf("MSG_MUSIC_EQ %d, \n ", eq_mode);
            }

/* #if LCD_SUPPORT_MENU */
            /* UI_menu(MENU_REFRESH); */
/* #else */
            UI_menu_arg(MENU_SET_EQ,eq_mode);
/* #endif */
            break;
#endif/*EQ_EN*/

            //  case MSG_MUSIC_SRC:
            //         src_mode++;
            //         if(src_mode >= 9)
            //         {
            //             src_mode = 0;
            //         }
            //    dec_phy_printf("samplerate_change= %d\r\n",samplerate_tab[src_mode]);
            //    samplerate_change(samplerate_tab[src_mode]);
            //    break;

        case MSG_MUSIC_PP:
#if UI_ENABLE
            if (UI_var.bCurMenu == MENU_INPUT_NUMBER)
            {
                os_taskq_post("MusicTask", 1 , MSG_INPUT_TIMEOUT);
                break;
            }
#endif

            puts("MSG_MUSIC_PP\n");
            _DECODE_STATUS pp_s;
            pp_s = pp_decode(&mapi->dop_api->dec_api);
            if(DECODER_PAUSE == pp_s)
            {
                puts("*PAUSE*\n");
                music_ui.opt_state = (MUSIC_OPT_BIT_PAUSE);
				music_pp_flag=1;
//                task_status = STAUS_PAUSE;
#if USB_DISK_EN
                usb->dev_io->ioctrl(0,DEV_SET_REMAIN);
#endif
				pa_mute();
                led_fre_set(0,0);
            }
            else
            {
				pa_umute();
				if(music_play_voice_flag==1)
				{
					music_play_voice_flag=0;
					if(music_pp_flag==1)
					{
						music_pp_flag=0;
					#ifdef MUSIC_LED_PLAYONE_FLASH
						if(play_mode == REPEAT_ONE)
							led_fre_set(5,0);
						else
					#endif
							led_fre_set(15,0);
						err = music_play_api(mapi,DEV_SEL_SPEC,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
						break;
					}
				}
				music_pp_flag=0;
                if(DECODER_PLAY == pp_s)
                {
                    puts("*PLAY*\n");
//                    task_status = STAUS_PLAY;
                    music_ui.opt_state = MUSIC_OPT_BIT_PLAY;
                }
                else
                {
                    puts("*STOP*\n");
//                    task_status = STAUS_STOP;
                    music_ui.opt_state = (MUSIC_OPT_BIT_PAUSE);
                }
#ifdef MUSIC_LED_PLAYONE_FLASH
				if(play_mode == REPEAT_ONE)
					led_fre_set(5,0);
				else
#endif
                led_fre_set(15,0);
            }
//                UI_menu(MENU_MUSIC_MAIN);
            /* UI_menu(MENU_REFRESH); */
    		UI_menu_mux(MENU_MUSIC_MAIN,MENU_LIST_DISPLAY);
            break;

        case MSG_MUSIC_FF:
            dec_phy_printf("ff");
            if(msg[1] == 0)
			{
				msg[1] = 2;
			}
			if (0 == dec_ff_fr(&mapi->dop_api->dec_api,PLAY_MOD_FF,msg[1]))
            {
                music_ui.opt_state = (MUSIC_OPT_BIT_FF);
                UI_menu(MENU_MUSIC_MAIN);
            }
			clean_ab_rpt();
            break;
        case MSG_MUSIC_FR:
            dec_phy_printf("fr");
            if(msg[1] == 0)
			{
				msg[1] = 2;
			}
	   
			/* msg[1] = get_decode_time(); */
			/* if(msg[1] > 15) */
			/* { */
				/* msg[1] = 15; */
			/* } */
			
			if (0 == dec_ff_fr(&mapi->dop_api->dec_api,PLAY_MOD_FB,msg[1]))
            {
                music_ui.opt_state = (MUSIC_OPT_BIT_FR);
                UI_menu(MENU_MUSIC_MAIN);

            }
			clean_ab_rpt();
            break;

        case MSG_MUSIC_FFR_DONE:
            break;

        case SYS_EVENT_DECODE_END:
            break;

        case MSG_MUSIC_PLAY:
            play_sel_play_decoder(mapi->dop_api);
            music_ui.opt_state = 0;
            UI_menu(MENU_REFRESH);
            break;

        case MSG_MUSIC_PAUSE:
            play_sel_pause_decoder(mapi->dop_api);
            music_ui.opt_state = (MUSIC_OPT_BIT_PAUSE);
            UI_menu(MENU_REFRESH);
            break;

        case MSG_MUSIC_PLAY_SN:
            puts("MSG_MUSIC_PLAY_SN\n");
            if ((msg[1] <= mapi->fop_api->cur_lgdev_info->total_file_num)&&(msg[1] != 0))
            {
                mapi->dop_api->file_num = msg[1];
                err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_SPEC_FILE,&(mapi->dop_api->file_num));
            }
            UI_menu(MENU_MUSIC_MAIN);
            break;

#if 1//REC_EN
        case MSG_MUSIC_DEL_FILE:
            file_auto_next = 1;
#ifdef MUSIC_LED_PLAYONE_FLASH
			if(play_mode == REPEAT_ONE)
				led_fre_set(5,0);
			else
#endif
            led_fre_set(15,0);
            dec_phy_printf("KEY_EVENT_NEXT\r\n");

            music_ui.opt_state = (MUSIC_OPT_BIT_DEL);
//                UI_menu(MENU_MUSIC_MAIN);
            UI_menu(MENU_REFRESH);

            music_stop_decoder(mapi);
            _FIL_HDL *f_h=(_FIL_HDL *)(mapi->fop_api->cur_lgdev_info->lg_hdl->file_hdl);
            FIL *pf=(FIL *)(f_h->hdl);
            err=f_unlink(pf);
            printf("del file %08x  \n",err);



            music_ui.opt_state &= (~MUSIC_OPT_BIT_DEL);

            file_operate_ctl(FOP_GET_TOTALFILE_NUM,mapi->fop_api,mapi->dop_api->file_type,0);

            if(0 == mapi->fop_api->cur_lgdev_info->total_file_num)
            {
                err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_SPEC_FILE,&(mapi->dop_api->file_num));
            }
            else
            {
                err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_SPEC_FILE,&(mapi->dop_api->file_num));
            }


            break;
#endif
#if 0
		case MSG_MUSIC_CHANGE_MODE:
			dec_phy_printf("MSG_MUSIC_U_SD\r\n");
			if(file_operate_get_total_phydev()>1)
			{
#ifdef MUSIC_LED_PLAYONE_FLASH
				if(play_mode == REPEAT_ONE)
					led_fre_set(5,0);
				else
#endif
					led_fre_set(15,0);
				if(file_operate_ctl(FOP_GET_PHYDEV_INFO,mapi->fop_api,0,0) == USB_DEVICE)
				{
					mapi->dop_api->dec_api.save_brk = 1;
					err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
				}
				else
				{
						mapi->dop_api->dec_api.save_brk = 1;
						os_taskq_post_msg("MainTask",1,MSG_CHANGE_WORKMODE);
				}
			}
			else
			{
				mapi->dop_api->dec_api.save_brk = 1;
				os_taskq_post_msg("MainTask",1,MSG_CHANGE_WORKMODE);
			}
			break;
#endif
        case SYS_EVENT_DEV_ONLINE:
            dec_phy_printf("music SYS_EVENT_DEV_ONLINE\r");
            ///music_stop_decoder(mapi->dop_api);
            dec_phy_printf("AA\r");
            mapi->dop_api->dec_api.save_brk = 1;
//                mapi->dop_api->dec_api.enable_brk = 1;
            mapi->dop_api->file_num = 1;
#ifdef MUSIC_USB_SD_VOICE
/////////////////////USB SD è¯­éŸ³åŒºåˆ† start//////////////////////////////////
		   if((u32)sd0 == curr_device_using)
		   {
			   music_prompt_play(mapi,BPF_SD_MP3,msg[0]);
		   }
		   else
		   {
			   music_prompt_play(mapi,BPF_USB_MP3,msg[0]);
		   }
//////////////////////USB SD è¯­éŸ³åŒºåˆ† end//////////////////////////////////
#else
            err = music_play_api(mapi,DEV_SEL_SPEC,msg[1],PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
#endif
            dec_phy_printf("BB\r");
            UI_menu(MENU_MUSIC_MAIN);
            break;
        case SYS_EVENT_DEL_TASK: 				//请求删除music任务
            mapi->dop_api->dec_api.save_brk = 1;
            music_stop_decoder(mapi);

			music_operate_mapi = NULL;
#ifndef MUSIC_USB_SD_VOICE
		if(g_device_in_flag)g_device_in_flag = 0;
#endif
//                UI_menu(MENU_WAIT);
			set_sys_freq(SYS_Hz);
            ui_close_music();
            dac_channel_off(MUSIC_CHANNEL, FADE_ON);
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD è¯­éŸ³åŒºåˆ† start//////////////////////////////////
			dec_dev_changed = 0;
//////////////////////USB SD è¯­éŸ³åŒºåˆ† end//////////////////////////////////
#endif
#if MUSIC_DECRYPT_EN
			cipher_close();
#endif

#if EQ_EN
            eq_disable();
#endif/*EQ_EN*/
//                set_flac_sys_freq(0);//set_sys_clk
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
            if(mapi->dop_api->dec_api.ef_enable&&mapi->dop_api->dec_api.ef_out_name)
            {
                if (os_task_del_req(mapi->dop_api->dec_api.ef_out_name) != OS_TASK_NOT_EXIST)
                {
                    os_taskq_post_event(mapi->dop_api->dec_api.ef_out_name, 1, SYS_EVENT_DEL_TASK);
                    do
                    {
                        OSTimeDly(1);
                    }
                    while(os_task_del_req(mapi->dop_api->dec_api.ef_out_name) != OS_TASK_NOT_EXIST);
                    mapi->dop_api->dec_api.ef_out_name = NULL;
                    puts("del music ef_out_name succ\n");
                }
            }

            enable = 0;

#if LRC_LYRICS_EN
            lrc_exit();
#endif

            ///<关闭文件操作器，释放资源
            file_operate_ctl(FOP_CLOSE_LOGDEV,mapi->fop_api,0,0);
            ///<释放文件操作器初始化信息

            echo_exit_api(&music_reverb);
            if(mapi->fop_api)
            {
                if(mapi->fop_api->fop_init)
                    free_fun((void**)&mapi->fop_api->fop_init);
                free_fun((void**)&mapi->fop_api);
            }
            free_fun((void**)&mapi->dop_api->brk);
            free_fun((void**)&mapi->dop_api->io);
            free_fun((void**)&mapi->dop_api);
            free_fun((void**)&mapi);

            if (os_task_del_req_name(OS_TASK_SELF) == OS_TASK_DEL_REQ)
            {
                os_task_del_res_name(OS_TASK_SELF); 	//确认可删除，此函数不再返回
            }

            break;

        case MSG_MUSIC_SPC_FILE:
            file_auto_next = 1;
#ifdef MUSIC_LED_PLAYONE_FLASH
			if(play_mode == REPEAT_ONE)
				led_fre_set(5,0);
			else
#endif
            led_fre_set(15,0);
            /* printf("MSG_MUSIC_SPC_FILE-----,0x%x\n",msg[1]); */
            err=music_play_by_sclust(mapi,msg[1]);
            break;

#if LCD_SUPPORT_MENU
        case MSG_ENTER_MENULIST:
            UI_menu_arg(MENU_LIST_DISPLAY,UI_MENU_LIST_ITEM);
            break;
#endif

#if MUSIC_AB_RPT_EN
		case MSG_MUSIC_AB_RPT:       ///<AB段循环播放设置
			puts("MUSIC_AB_RPT\n");
			if((DECODER_PLAY == get_decode_statu(&mapi->dop_api->dec_api))||(DECODER_PAUSE== get_decode_statu(&mapi->dop_api->dec_api)))
			{
				switch_a_b_repeat(mapi);
			}
			break;
#endif

        case MSG_HALF_SECOND:
#if MAX_VOL_VOICE
			if(key_short_cnt == 3)
			{
				key_short_cnt = 0;
                os_taskq_post_msg("MusicTask", 1, SYS_EVENT_PLAY_SEL_END);
			}

			if(key_short_cnt == 2)
			{
				key_short_cnt = 3;
			}
#endif
            play_time = get_decode_time();
            {
                music_ui.play_time = play_time;
                UI_menu(MENU_HALF_SEC_REFRESH);
            }

#if MUSIC_AB_RPT_EN
            if((ab_rept.b_stage.play_time)&&(ab_rept.b_stage.play_time <= play_time)&&(DECODER_PLAY == get_decode_statu(&mapi->dop_api->dec_api)))
            {
                if(ab_rept.b_stage.file_num == mapi->dop_api->file_num)
                {
					if(0 == strcmp("FLA",mapi->dop_api->dec_api.phy_ops->dec_ops->name))
					{
						mapi->dop_api->dec_api.flac_bp_buff = ab_rept.a_stage->inf.flac_brk_buff;
						err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_AB_REPEAT,&(mapi->dop_api->file_num));
						mapi->dop_api->dec_api.flac_bp_buff = &mapi->dop_api->brk->inf.flac_brk_buff[0];
					}
					else
					{
						mapi->dop_api->dec_api.bp_buff = ab_rept.a_stage->inf.norm_brk_buff;
						err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_AB_REPEAT,&(mapi->dop_api->file_num));
						mapi->dop_api->dec_api.bp_buff = &mapi->dop_api->brk->inf.norm_brk_buff[0];
					}
                }
            }
#endif // MUSIC_AB_RPT
            music_ui.opt_state &= (~(MUSIC_OPT_BIT_FF|MUSIC_OPT_BIT_FR));
            break;

		case MSG_PROMPT_PLAY:
        case MSG_LOW_POWER:
            dec_phy_puts("Music_Low_Power\n");
            music_prompt_play(mapi,BPF_LOW_POWER_MP3,msg[0]);
            break;
		case MSG_MAX_VOL:
			music_play_voice_flag=1;
			lowbat_play_voice_flag=1;
			music_prompt_play(mapi,BPF_MAX_VOL_MP3,msg[0]);
			break;
		case MSG_LOW_POWER_VOICE:
			music_play_voice_flag=1;
			lowbat_play_voice_flag=1;
            music_prompt_play(mapi,BPF_LOW_POWER_MP3,msg[0]);
            break;
			
		case MSG_SD_MP3:
		//	key_short_cnt = 1;
			music_play_voice_flag=1;
			lowbat_play_voice_flag=1;
			music_prompt_play(mapi,BPF_SD_MP3,msg[0]);
			break;
		case MSG_USB_MP3:
		//	key_short_cnt = 1;
			music_play_voice_flag=1;
			lowbat_play_voice_flag=1;
			music_prompt_play(mapi,BPF_USB_MP3,msg[0]);
			break;
        case MSG_INPUT_TIMEOUT:

            if ((input_number <= mapi->fop_api->cur_lgdev_info->total_file_num)&&(input_number != 0))
            {
                mapi->dop_api->file_num = input_number;
                err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_SPEC_FILE,&(mapi->dop_api->file_num));
            }
            input_number = 0;
            input_number_cnt= 0;

            UI_menu(MENU_MUSIC_MAIN);
            break;
        case SYS_EVENT_DEV_BEGIN_MOUNT:         ///<新设备开始枚举，结束解码，防止出现新设备枚举慢，当前设备播放
            printf("new dev mount,stop  \n");   ///<一小段，又切换到新设备
            mapi->dop_api->dec_api.save_brk = 1;
            music_stop_decoder(mapi);
            break;
        case SYS_EVENT_DEV_MOUNT_ERR:       ///<新设备枚举失败，继续当前设备播放
			printf("dev mount err\n");
            mapi->dop_api->dec_api.save_brk = 0;
            err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
            break;
        default:
            dec_phy_printf("dev status =%08x \n",err);
            echo_msg_deal_api(&music_reverb, msg);
            break;
        }
        u32 totalerrfile=0;
        while(0 != err)
        {
            dec_phy_printf("--music err  : %02x\n",err);
            u32 status;

/*
			memset(msg,0x00,sizeof(msg));
			status = os_taskq_pend(2, ARRAY_SIZE(msg), msg);
			clear_wdt();

			if(status == OS_NO_ERR)
			{
				switch(msg[0])
				{
					case SYS_EVENT_DEL_TASK:
						os_taskq_flush(OS_TASK_SELF);
						os_taskq_post_msg(OS_TASK_SELF,1,SYS_EVENT_DEL_TASK);
						err = 0;
						break;
				}
			}
			else
*/
			{
            switch(err)
            {
            case FILE_OP_ERR_INIT:              ///<文件选择器初始化错误
            case FILE_OP_ERR_OP_HDL:            ///<文件选择器指针错误
            case FILE_OP_ERR_LGDEV_NULL:        ///<没有设备
            case FILE_OP_ERR_NO_FILE_ALLDEV:    ///<没有文件（所有设备）
                file_operate_ctl(FOP_CLOSE_LOGDEV,0,0,0);
                wait_exit=1;            ///<进入等待退出状态
				wait_ok = 1;
				/* os_taskq_flush(MAIN_TASK_NAME); */
                /* os_taskq_post(MAIN_TASK_NAME, 1, MSG_CHANGE_WORKMODE); */
				if(OS_NO_ERR == os_taskq_post(MAIN_TASK_NAME, 1, MSG_CHANGE_BT))
				{
					wait_ok = 0;
				}
                err = 0;
                break;
            case FILE_OP_ERR_OPEN_BPFILE:
                dec_phy_printf("err  : %02x\r",err);
                mapi->dop_api->file_num = 1;
                err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_FIRST_FILE,&(mapi->dop_api->file_num));
                break;
            case FILE_OP_ERR_NUM:               ///<选择文件的序号出错
                mapi->dop_api->file_num = 1;
                err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_SPEC_FILE,&(mapi->dop_api->file_num));
                break;
            case FILE_OP_ERR_LGDEV_MOUNT:
            case FILE_OP_ERR_NO_FILE_ONEDEV:    ///<当前选择的设备没有文件
                mapi->dop_api->file_num = 1;
          //err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_BREAK_POINT,&(mapi->dop_api->file_num));
                 err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_SPEC_FILE,&(mapi->dop_api->file_num));

		        /* err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_SPEC_FILE,&(mapi->dop_api->file_num)); */
                break;
            case FILE_OP_ERR_LGDEV_NO_FIND:     ///<没找到指定的逻辑设备
                mapi->dop_api->file_num = 1;
                err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_SPEC_FILE,&(mapi->dop_api->file_num));
                break;

            case SUCC_MUSIC_START_DEC:
                err = 0;
                file_auto_next = 1;
                dec_phy_printf("SUCC_MUSIC_START_DEC \r");
                break;
            case FILE_OP_ERR_OPEN_FILE:         ///<打开文件失败
            case ERR_MUSIC_START_DEC:
                status=file_operate_ctl(FOP_DEV_STATUS,mapi->fop_api,0,0);
                if(status==FILE_OP_ERR_OP_HDL)
                {
                    ///<逻辑设备不再链表
                    err=FILE_OP_ERR_OP_HDL;
                    break;
                }
                else if(!status)
                {
                    ///<逻辑设备掉线
                    err=FILE_OP_ERR_LGDEV_NO_FIND;
                    break;
                }
                totalerrfile++;
                dec_phy_printf("---total err =%08x \n",totalerrfile);
                dec_phy_printf("---total_file_num =%08x \n",mapi->fop_api->cur_lgdev_info->total_file_num);
                if(totalerrfile>=mapi->fop_api->cur_lgdev_info->total_file_num)
                {
                    ///<当前设备中音乐文件全部不可以解码，做好标识
                    file_operate_ctl(FOP_ALLFILE_ERR_LGDEV,mapi->fop_api,0,0);
                    err=FILE_OP_ERR_NO_FILE_ONEDEV;
                    totalerrfile=0;
                    break;
                }
                if(file_auto_next)
                {
                    dec_phy_printf("err next \r");
                    err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_NEXT_FILE,&(mapi->dop_api->file_num));
                }
                else
                {
                    dec_phy_printf("err priv \r");
                    err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_PREV_FILE,&(mapi->dop_api->file_num));
                }
                if((err!=FILE_OP_ERR_OPEN_FILE)&&(err!=ERR_MUSIC_START_DEC))
                {
                    ///<不再是顺序找，清空错误数字
                    totalerrfile=0;
                }
                break;
            case ERR_MUSIC_API_NULL:
                dec_phy_printf("err  : %02x\r",err);
                err = 0;
                break;
            case FILE_OP_ERR_END_FILE:
                mapi->dop_api->file_num = 1;
				mapi->dop_api->dec_api.save_brk = 0;
				file_operate_ctl(FOP_CLEAR_BPINFO,mapi->fop_api,&mapi->dop_api->brk->inf,0);
                err = music_play_api(mapi,DEV_SEL_NEXT,0,PLAY_FIRST_FILE,&(mapi->dop_api->file_num));
                break;
            case FILE_OP_ERR_PRE_FILE:
                mapi->dop_api->file_num = 1;
                err = music_play_api(mapi,DEV_SEL_PREV,0,PLAY_LAST_FILE,&(mapi->dop_api->file_num));
                break;
            default:
                dec_phy_printf("err default : %02x\r",err);
                err = music_play_api(mapi,DEV_SEL_CUR,0,PLAY_NEXT_FILE,0);
                printf("err my \n");
                break;
				}
            }
        }

        {
            //dec_phy_printf("no err\n");
        }
#if SUPPORT_APP_RCSP_EN
		rcsp_music_task_msg_deal_after(msg);
#endif
	}
}

/*----------------------------------------------------------------------------*/
/**@brief  创建音乐线程
   @param  priv -- 线程输入参数
   @return void
   @note
*/
/*----------------------------------------------------------------------------*/
static void music_task_init(void *priv)
{
    u32 err;

//      GET_SYS_MEM_STAT();
#if AB_D_EN
		ABD_HIGH();
#endif
    err = os_task_create_ext(music_task, 0, TaskMusicPrio, 50, "MusicTask", MUSIC_TASK_STACK_SIZE);
    if(OS_NO_ERR == err)
    {
        key_msg_register("MusicTask",music_ad_table,music_io_table,music_ir_table,NULL);
        puts("open_music_task: succ\n");
    }

}

/*----------------------------------------------------------------------------*/
/**@brief  退出音乐线程
   @param  void
   @return void
   @note
*/
/*----------------------------------------------------------------------------*/
static void music_task_exit()
{
    if (os_task_del_req("MusicTask") != OS_TASK_NOT_EXIST)
    {
        os_taskq_post_event("MusicTask", 1, SYS_EVENT_DEL_TASK);
        do
        {
            OSTimeDly(1);
        }
        while(os_task_del_req("MusicTask") != OS_TASK_NOT_EXIST);
#ifdef MUSIC_USB_SD_VOICE
//////////////////////USB SD è¯­éŸ³åŒºåˆ† start//////////////////////////////////
		power_on_flag=0;
//////////////////////USB SD è¯­éŸ³åŒºåˆ†end//////////////////////////////////
#endif
        puts("del_music_task: succ\n");
    }

//      GET_SYS_MEM_STAT();
}


TASK_REGISTER(music_task_info) =
{
    .name = "MusicTask",
    .init = music_task_init,
    .exit = music_task_exit,
};

