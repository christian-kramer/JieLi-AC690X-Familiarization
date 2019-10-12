/*
 *********************************************************************************************************
 *                                                BC51
 *                                      Remote Control System Protocol (RCSP)
 *                                             CODE
 *
 *                          (c) Copyright 2015-2016, ZHUHAI JIELI
 *                                           All Rights Reserved
 *
 * File : *
 * By   : jamin.li
 * DATE : 8/20/2015 build this file
 *********************************************************************************************************
 */
#include "rcsp/rcsp_head.h"
#include "rcsp/rcsp_interface.h"
#include "dac/dac_api.h"
#include "dac/dac.h"
#include "fm/fm_radio.h"
#include "fm/fm_api.h"
#include "music/music_info.h"
#include "key_drv/key_voice.h"
#include "bluetooth/avctp_user.h"
#include "fat/fs_io.h"
#include "file_operate/file_op.h"
#include "file_operate/file_op_err.h"

#if SUPPORT_APP_RCSP_EN


extern u8 *dev_get_phydev_name(void *drv_it);
extern u8 get_bt_connect_status(void);
extern BT_MUSIC_STATE a2dp_get_status(void);

//---------------------------------------------------------

static void rcsp_set_eq_mode(int new_mode)
{
	P_FUNCTION
#if EQ_EN
	if(new_mode > 5)
	{
		new_mode = 0;
	}
	eq_mode = new_mode;
	eq_mode_sw(new_mode);
	rcsp_report_status(RCSP_REPORT_EQ_VALUE);
#endif
}

extern void set_bt_eq_mode(int new_mode);
static void rcsp_set_bt_eq_mode(int new_mode)
{
	P_FUNCTION
#if EQ_EN
	set_bt_eq_mode(new_mode);
	rcsp_report_status(RCSP_REPORT_EQ_VALUE);
#endif
}



//---------------------------------------------------------

static void rcsp_main_app_msg_deal(int *msg)
{
	P_FUNCTION
	switch(msg[0])
	{
		case RCSP_CBK_MSG_COMM_SET_VOL:
			dac_ctl.sys_vol_l = (u8)(msg[1]);
			if(dac_ctl.sys_vol_l > MAX_SYS_VOL_L)
			{
				dac_ctl.sys_vol_l = MAX_SYS_VOL_L;
			}
#if KEY_TONE_EN
		    if(dac_ctl.sys_vol_l == MAX_SYS_VOL_L)
		        send_key_voice(500);
#endif
		    printf("V_set = %d\n",dac_ctl.sys_vol_l);
			dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
            set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
#if (SYS_DEFAULT_VOL==0)
		    vm_cache_write(VM_SYS_VOL,&dac_ctl.sys_vol_l,2);
#endif
			rcsp_report_status(RCSP_REPORT_VOL_VALUE);

			UI_menu_arg(MENU_MAIN_VOL,0);	
			break;
				
		default:
			break;
	}

}

void rcsp_main_task_msg_deal_after(int *msg)
{
	/* P_FUNCTION */
	switch(msg[0])
	{
		case MSG_HALF_SECOND:
			break;
		case SYS_EVENT_LGDEV_ONLINE:
			rcsp_device_change(dev_get_phydev_name((void*)msg[1]));
			break;

		case MSG_VOL_DOWN:
		case MSG_VOL_UP:
			if(is_dac_mute())
			{
				dac_mute(0,1);
			}
			rcsp_report_status(RCSP_REPORT_VOL_VALUE);
			break;
		case MSG_APP_MAIN_MSGLIST:
			rcsp_main_app_msg_deal(msg+1);
			break;

		default:
			break;
	}
	/* Update_rcsp_Task_State(msg[0],1); */
}

void rcsp_main_task_msg_deal_before(int *msg)
{
	/* P_FUNCTION */
}

//---------------------------------------------------------

static int is_bt_playing(void)
{
	u8 bt_cnn_st = get_bt_connect_status();
			
	if((bt_cnn_st == BT_STATUS_CONNECTING) || 
	(bt_cnn_st == BT_STATUS_TAKEING_PHONE) || 
	(bt_cnn_st == BT_STATUS_PLAYING_MUSIC)) 
	{
		if(BT_MUSIC_STATUS_STARTING == a2dp_get_status())
		{
			return 1;		
		}
		else if(BT_STATUS_TAKEING_PHONE == bt_cnn_st)
		{
			return 1;	
		}		
		
		else
		{
			;
		}
	}

	return 0;
}


static void rcsp_bt_app_msg_deal(int *msg)
{
	char *target_name = RDEF_BTMSG_TASK_NAME;
	P_FUNCTION
	switch(msg[0])
	{
		case RCSP_CBK_MSG_COMM_PLAY:
			if(is_bt_playing() == 0)
			{
        		os_taskq_post(target_name,1,MSG_BT_PP);
			}
			break;

		case RCSP_CBK_MSG_COMM_PAUSE:
			if(is_bt_playing())
			{
        		os_taskq_post(target_name,1,MSG_BT_PP);
			}
			break;

		case RCSP_CBK_MSG_COMM_EQ:
			rcsp_set_bt_eq_mode(msg[1]);
			break;

		default:
			break;
	}
}


void rcsp_bt_task_msg_deal_after(int *msg)
{
	/* P_FUNCTION */
	switch(msg[0])
	{
		case MSG_HALF_SECOND:
			if(is_bt_playing())
			{
				rcsp_set_task_status(STAUS_PLAY);
			}
			else
			{
				rcsp_set_task_status(STAUS_PAUSE);
			}
			break;
		
		case MSG_BT_NEXT_FILE:
		case MSG_BT_PREV_FILE:
			/* rcsp_set_task_status(STAUS_PLAY); */
			break;

		case MSG_APP_MSGLIST:
			rcsp_bt_app_msg_deal(msg+1);
			break;

		default:
			break;
	}
	Update_rcsp_Task_State(msg[0],1);
}

void rcsp_bt_task_msg_deal_before(int *msg)
{
	/* P_FUNCTION */
}

//---------------------------------------------------------
extern MUSIC_OP_API *music_operate_mapi;
u8 rcsp_music_curr_device;
u8 rcsp_music_getting_info;
static int is_music_playing(void)
{
	_DECODE_STATUS pp_s;
    
	pp_s = get_decode_statu(&music_operate_mapi->dop_api->dec_api);
    if(DECODER_PLAY != pp_s)
    {
  		return 0; 
    }
	return 1;
}


static void rcsp_music_app_msg_deal(int *msg)
{
	char *target_name = RDEF_MUSIC_TASK_NAME;
	P_FUNCTION
	switch(msg[0])
	{
		case RCSP_CBK_MSG_COMM_PLAY:
			if(is_music_playing() == 0)
			{
        		os_taskq_post(target_name,1,MSG_MUSIC_PP);
			}
			break;

		case RCSP_CBK_MSG_COMM_PAUSE:
			if(is_music_playing())
			{
        		os_taskq_post(target_name,1,MSG_MUSIC_PP);
			}
			break;
		
		case RCSP_CBK_MSG_COMM_EQ:
			rcsp_set_eq_mode(msg[1]);
			break;
	
		case RCSP_CBK_MSG_COMM_PLAYMODE:
			if(music_operate_mapi && music_operate_mapi->fop_api && music_operate_mapi->fop_api->fop_init)
		    {	
                play_mode = msg[1];
		        if(play_mode >= MAX_PLAY_MODE)
		        {	
		        	play_mode = REPEAT_ALL;
		        }
		       	music_operate_mapi->fop_api->fop_init->cur_play_mode = play_mode;
		    }
			rcsp_report_status(RCSP_REPORT_PLAYMODE_VALUE);	
			break;

		default:
			break;
	}

}

void rcsp_music_task_msg_deal_after(int *msg)
{
	/* P_FUNCTION */
	/* return; */
	switch(msg[0])
	{
		case MSG_HALF_SECOND:
			if(is_music_playing())
			{
				rcsp_set_task_status(STAUS_PLAY);	
			}
			else
			{
				rcsp_set_task_status(STAUS_PAUSE);	
			}
			break;

		case SYS_EVENT_DEC_SET_END:
		case SYS_EVENT_DEC_FR_END:
		case SYS_EVENT_DEC_FF_END:
		case SYS_EVENT_DEC_END:
			break;

		case MSG_MUSIC_EQ:
			rcsp_report_status(RCSP_REPORT_EQ_VALUE);
			break;

		case MSG_MUSIC_PLAY:
		case MSG_MUSIC_FF:
		case MSG_MUSIC_FR:
			rcsp_set_task_status(STAUS_PLAY);	
			break;

		case MSG_MUSIC_PAUSE:
			rcsp_set_task_status(STAUS_PAUSE);	
			break;

		case MSG_APP_MSGLIST:
			rcsp_music_app_msg_deal(msg+1);
			break;	
		
		default:
			break;
	}
	Update_rcsp_Task_State(msg[0],1);
}


void rcsp_music_task_msg_deal_before(int *msg)
{
	/* P_FUNCTION */
	/* return; */
	switch(msg[0])
	{
		case SYS_EVENT_PLAY_SEL_END:
			rcsp_music_curr_device = 0xff;
   			rcsp_music_getting_info = 0x00;
			
			if(music_name_buf.tpath ==NULL)
			{
				music_name_buf.tpath= malloc(128 + ALIGN_4BYTE(sizeof(LONG_FILE_NAME)) *2);
				if(music_name_buf.tpath ==NULL)
				{
	    			puts("***disp malloc fail11****\n");
				}
				else
				{
					music_name_buf.file_name = (LONG_FILE_NAME*) (music_name_buf.tpath + 128);
					music_name_buf.dir_name = (LONG_FILE_NAME*) (music_name_buf.tpath + 128 + ALIGN_4BYTE(sizeof(LONG_FILE_NAME)));	
				}
			}
			else
			{
	    		puts("***disp not null111****\n");
			}

			break;

		case SYS_EVENT_DEL_TASK:
   			rcsp_music_getting_info = 0x00;
		    if(music_name_buf.tpath != NULL)
   			{
        		free(music_name_buf.tpath);
        		music_name_buf.tpath = NULL;
   			}

		case SYS_EVENT_DEC_DEVICE_ERR:
		case SYS_EVENT_DEV_ONLINE:
			rcsp_set_music_bs_op_api(NULL);
			break;
		
		case MSG_MUSIC_U_SD:
			if(file_operate_get_total_phydev() > 1)
			{
				rcsp_set_music_bs_op_api(NULL);
			}
			break;
		
		case SYS_EVENT_DEV_OFFLINE:
		{
        	lg_dev_info *tmplgdev;
			tmplgdev = music_operate_mapi->fop_api->cur_lgdev_info;
			if(tmplgdev != NULL)
			{
				if(tmplgdev->lg_hdl->phydev_item == (void *)msg[1])
				{
					rcsp_set_music_bs_op_api(NULL);
				}	
			}
			break;
		}

		default:
			break;
	}
	Update_rcsp_Task_State(msg[0],1);
}


void rcsp_music_play_api_before(ENUM_DEV_SEL_MODE dev_sel,u32 dev_let)
{
	/* P_FUNCTION */
   	rcsp_music_getting_info = 0x00;
	if(dev_sel != DEV_SEL_CUR)
	{
		rcsp_set_music_bs_op_api(NULL);
	}
}

void rcsp_music_play_api_after(ENUM_DEV_SEL_MODE dev_sel,u32 dev_let,u32 reslut)
{
	/* P_FUNCTION */
	u8 tmp8;
	if(reslut == SUCC_MUSIC_START_DEC)
	{
		rcsp_set_music_bs_op_api(music_operate_mapi);
		
        _FIL_HDL *f_h=(_FIL_HDL *)(music_operate_mapi->fop_api->cur_lgdev_info->lg_hdl->file_hdl);
        FIL *pf=(FIL *)(f_h->hdl);
		/* printf("pf->dir_info->fname %s\n",pf->dir_info.fname); */
        music_file_info(music_operate_mapi);//get_file_info
   		rcsp_music_getting_info  = 0x50;

		tmp8 = (u8)file_operate_ctl(FOP_GET_PHYDEV_INFO,music_operate_mapi->fop_api,0,0);
		/* printf("-----rcsp_music_curr_device = %d\n",tmp8); */
		if(rcsp_music_curr_device != 0xff)
	    {	 
	         rcsp_report_status(RCSP_REPORT_MUSIC_FILEINFO);
		}
		else
		{	 
		     rcsp_report_status(RCSP_REPORT_START_CONFIG);
		}
        if((dev_sel != DEV_SEL_CUR) && (tmp8 != rcsp_music_curr_device))
        {		
            rcsp_music_curr_device = tmp8;
            puts("send change_dev cmd\n");
        }
        rcsp_update_cur_device(rcsp_music_curr_device);

        rcsp_set_task_status(STAUS_PLAY);
		
	}
}

//---------------------------------------------------------
static int is_fm_playing(void)
{
	if(fm_mode_var->fm_mute)
	{
		return 0;
	}

	return 1;
}

void rcsp_fm_scan_channel_deal(int *msg)
{
	P_FUNCTION
	int cnt = 3;
	rcsp_report_status(RCSP_REPORT_RADIO_TABLE);
	while(cnt--)
	{
		Update_rcsp_Task_State(msg[0],1);
		os_time_dly(10);
	}
}

static void rcsp_fm_app_msg_deal(int *msg)
{
	char *target_name = RDEF_RADIO_TASK_NAME;
	P_FUNCTION
	switch(msg[0])
	{
		case RCSP_CBK_MSG_COMM_PLAY:
			if(is_fm_playing() == 0)
			{
        		os_taskq_post(target_name,1,MSG_FM_PP);
			}
			break;

		case RCSP_CBK_MSG_COMM_PAUSE:
			if(is_fm_playing())
			{
        		os_taskq_post(target_name,1,MSG_FM_PP);
			}
			break;

		case RCSP_CBK_MSG_COMM_EQ:
			rcsp_set_eq_mode(msg[1]);
			break;

		default:
			break;
	}

}


void rcsp_fm_task_msg_deal_after(int *msg)
{
	/* P_FUNCTION */
	switch(msg[0])
	{
		case MSG_HALF_SECOND:
			if(is_fm_playing())
			{
				rcsp_set_task_status(STAUS_PLAY);	
			}
			else
			{
				rcsp_set_task_status(STAUS_PAUSE);	
			}
			break;
		
		case MSG_FM_SCAN_ALL_INIT:
		case MSG_FM_SCAN_ALL:
			if(fm_mode_var->scan_mode == FM_SCAN_STOP)
			{
				rcsp_report_status(RCSP_REPORT_RADIO_TABLE);
				rcsp_set_task_status(STAUS_PLAY);	
			}
			break;

		case MSG_FM_PREV_STEP:
		case MSG_FM_NEXT_STEP:
		case MSG_FM_PREV_STATION:
		case MSG_FM_NEXT_STATION:
		case MSG_FM_SELECT_CHANNEL:
		case MSG_FM_SELECT_FREQ:
		case MSG_INPUT_TIMEOUT:
			rcsp_report_status(RCSP_REPORT_RADIO_CHANNEL);
			break;

		case MSG_FM_DEL_CHANNEL:
		case MSG_FM_SAVE_CHANNEL:
			rcsp_report_status(RCSP_REPORT_RADIO_TABLE);
			break;

		case MSG_FM_PP:
			if(is_fm_playing())
			{
				rcsp_set_task_status(STAUS_PLAY);	
			}
			else
			{
				rcsp_set_task_status(STAUS_PAUSE);	
			}
			break;

		case MSG_MUSIC_EQ:
			rcsp_report_status(RCSP_REPORT_EQ_VALUE);
			break;

		case SYS_EVENT_PLAY_SEL_END:
			rcsp_report_status(RCSP_REPORT_START_CONFIG);
			break;

		case MSG_APP_MSGLIST: 
			rcsp_fm_app_msg_deal(msg+1);
			break;
		
		default:
			break;
	}
	Update_rcsp_Task_State(msg[0],1);
}

void rcsp_fm_task_msg_deal_before(int *msg)
{
	/* P_FUNCTION */
}

//---------------------------------------------------------
static int is_linein_playing(void)
{
	if(is_dac_mute())
	{
		return 0;
	}

	return 1;
}


void rcsp_linein_app_msg_deal(int *msg)
{
	char *target_name = RDEF_LINEIN_TASK_NAME;
	P_FUNCTION
	switch(msg[0])
	{
		case RCSP_CBK_MSG_COMM_PLAY:
			if(is_linein_playing() == 0)
			{
        		os_taskq_post(target_name,1,MSG_AUX_MUTE);
			}
			break;

		case RCSP_CBK_MSG_COMM_PAUSE:
			if(is_linein_playing())
			{
        		os_taskq_post(target_name,1,MSG_AUX_MUTE);
			}
			break;

		default:
			break;
	}
}

void rcsp_linein_task_msg_deal_after(int *msg)
{
	/* P_FUNCTION */
	switch(msg[0])
	{
		case MSG_HALF_SECOND:
			if(is_linein_playing())
			{
				rcsp_set_task_status(STAUS_PLAY);	
			}
			else
			{
				rcsp_set_task_status(STAUS_PAUSE);	
			}
			break;
		
		case MSG_APP_MSGLIST:
			rcsp_linein_app_msg_deal(msg+1);
			break;

		default:
			break;
	}
	Update_rcsp_Task_State(msg[0],1);
}


void rcsp_linein_task_msg_deal_before(int *msg)
{
	/* P_FUNCTION */
}

//---------------------------------------------------------

#define USB_PLAY_ENERGY_MAX   (6000)
static int is_pc_playing(void)
{
	u32 dac_energy_value = get_dac_energy_value();
	if(dac_energy_value > USB_PLAY_ENERGY_MAX)
	{
		return 1;
	}
	return 0;
}


static void rcsp_pc_app_msg_deal(int *msg)
{
	char *target_name = RDEF_UDISK_TASK_NAME;
	P_FUNCTION
	switch(msg[0])
	{
		case RCSP_CBK_MSG_COMM_PLAY:
			if(is_pc_playing() == 0)
			{
        		os_taskq_post(target_name,1,MSG_PC_PP);
			}
			break;

		case RCSP_CBK_MSG_COMM_PAUSE:
			if(is_pc_playing())
			{
        		os_taskq_post(target_name,1,MSG_PC_PP);
			}
			break;

		default:
			break;
	}

}

void rcsp_pc_task_msg_deal_after(int *msg)
{
	static u8 sync_vol_do = 0;
	switch(msg[0])
	{
		case MSG_HALF_SECOND:
			if(is_pc_playing())
			{
				rcsp_set_task_status(STAUS_PLAY);	
			}
			else
			{
				rcsp_set_task_status(STAUS_PAUSE);	
			}
			break;
	
		case MSG_PC_SPK_VOL:
			rcsp_report_status(RCSP_REPORT_VOL_VALUE);
			if(sync_vol_do == 0)
			{
				rcsp_report_status(RCSP_REPORT_START_CONFIG);
				sync_vol_do = 1; 
			}
			break;

		case SYS_EVENT_PLAY_SEL_END:
		case MSG_PC_MODE_INIT:
			sync_vol_do = 0; 
			break;

		case MSG_APP_MSGLIST:
			rcsp_pc_app_msg_deal(msg +1);
			break;

		default:
			break;
	}
	Update_rcsp_Task_State(msg[0],1);

}

void rcsp_pc_task_msg_deal_before(int *msg)
{
	/* P_FUNCTION */
}

//---------------------------------------------------------
static void rcsp_light_app_msg_deal(int *msg)
{
	P_FUNCTION
}

void rcsp_light_task_msg_deal_after(int *msg)
{
	/* P_FUNCTION */
	switch(msg[0])
	{
		case MSG_HALF_SECOND:
			break;
		
		case MSG_APP_MSGLIST:
			rcsp_light_app_msg_deal(msg + 1);
			break;

		default:
			break;
	}
	Update_rcsp_Task_State(msg[0],1);

}

void rcsp_light_task_msg_deal_before(int *msg)
{
	/* P_FUNCTION */
}

#endif //#if





