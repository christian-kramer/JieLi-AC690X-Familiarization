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

#include "rcsp/rcsp_interface.h"
#include "rcsp/rcsp_head.h"

#if SUPPORT_APP_RCSP_EN 

typedef u32 (*app_subcmd_cbk_t)(u8 *OperationData,u8 *data,u16 len);
///蓝牙模式 手机音乐模式 设备音乐模式 灯光控制 收音机 外部音源 USB声卡
static const char app_mode_name_string[TASK_APP_MAX][16] =
{
    { "bt_case.app" },
    { "music.app" },

#ifdef TASK_LIGHT_APP
    { "light.app" },
#endif

#ifdef TASK_RADIO_APP
	{ "radio.app" },
#endif

	{ "linein.app" },

#ifdef TASK_UDISK_APP
    { "udisk.app" },
#endif

};

const char support_taskname_group[TASK_APP_MAX][16] =
{
    { RDEF_BTSTACK_TASK_NAME },
    { RDEF_MUSIC_TASK_NAME },

#ifdef TASK_LIGHT_APP
	{ RDEF_LIGHT_TASK_NAME },
#endif

#ifdef TASK_RADIO_APP
    { RDEF_RADIO_TASK_NAME },
#endif

	{ RDEF_LINEIN_TASK_NAME },

#ifdef TASK_UDISK_APP
    { RDEF_UDISK_TASK_NAME },
#endif
};

const rcsp_apptask_cbk_t support_app_msg_callback_group[TASK_APP_MAX] =
{
    rcsp_bt_callback,
    rcsp_music_callback,

#ifdef TASK_LIGHT_APP
    rcsp_light_callback,//独立模式 callback
#endif

#ifdef TASK_RADIO_APP
    rcsp_radio_callback,
#endif

	rcsp_linein_callback,

#ifdef TASK_UDISK_APP
    rcsp_udisk_callback,
#endif
};




rcsp_apptask_cbk_t  apptask_msg_cbk_deal;
u8 cur_cmd_opt_task_id;


static u32 light_subcmd_deal(u8 *OperationData,u8 *data,u16 len);
static u32 music_subcmd_deal(u8 *OperationData,u8 *data,u16 len);
static u32 radio_subcmd_deal(u8 *OperationData,u8 *data,u16 len);
static u32 linein_subcmd_deal(u8 *OperationData,u8 *data,u16 len);
static u32 udisk_subcmd_deal(u8 *OperationData,u8 *data,u16 len);
static u32 bt_subcmd_deal(u8 *OperationData,u8 *data,u16 len);

/* static u32 fw_report_support_app_info(void); */
static const app_subcmd_cbk_t  task_subcmd_cbk_group [TASK_APP_MAX] =
{
    bt_subcmd_deal,
    music_subcmd_deal,

#ifdef TASK_LIGHT_APP
    light_subcmd_deal,
#endif

#ifdef TASK_RADIO_APP
    radio_subcmd_deal,
#endif

	linein_subcmd_deal,

#ifdef TASK_UDISK_APP
    udisk_subcmd_deal,
#endif

};


/*
 *********************************************************************************************************
 *
 * Description: 确定task的app id
 *
 * Arguments  : app_name
 *
 * Returns    : phone_app_id
 *
 * Note:
 *********************************************************************************************************
 */

static u8 *get_taskname_from_appid(u8 appid)
{
	if(appid < TASK_APP_MAX)
	{
		return (u8*)support_taskname_group[appid];
	}
	return NULL;
}

/*
 *********************************************************************************************************
 *
 * Description: 确定当前task的app id
 *
 * Arguments  : none
 *
 * Returns    : app id
 *
 * Note:
 *********************************************************************************************************
 */
u8 get_taskid_from_name(u8 *name)
{
    u8 task_id;

    for(task_id = 0; task_id < TASK_APP_MAX; task_id++)
    {
        if(0 == strcmp((void*)name,(void*)support_taskname_group[task_id]))
        {
            break;
        }
    }
    return task_id;
}

bool check_is_light_control(u8 app_id)
{
    u8 *tmp_name;;

///    P_FUNCTION
    tmp_name = (void*)get_taskname_from_appid(app_id);

///    rcsp_db_printf("0x%x, name= %s\n",(u32)tmp_name,tmp_name);
///    put_buf((void*)tmp_name,16);

    if (0 == strcmp((void*)tmp_name,RDEF_LIGHT_TASK_NAME))
    {
        return true;
    }
    else
    {
        return false;
    }

}

static u32 app_report_frame_analysis(u8 *data,u16 len)
{
    u16 tmp16,report_count;
    u32 tmp32;
    int frame_gp_len;
    task_frame_parm_t *report_frame_p;
    u8 *framp_buf_addr = malloc(sizeof(task_frame_parm_t) * 8);

    P_FUNCTION

    if(framp_buf_addr == NULL)
    {
        rcsp_err_puts("01malloc fail\n");
        return RCSP_OP_ERR_MALLOC;
    }

    report_frame_p = (task_frame_parm_t*)framp_buf_addr;

    tmp16 = READ_BIG_U16(data);
    if(tmp16 == 0xffff)
    {
        tmp16 = READ_BIG_U16(data+2);
        if(tmp16 != cal_crc16_deal(data+6, len -6))
        {
            rcsp_err_puts("crc error\n");
        }
    }

    rcsp_db_printf("len= %d\n",len);

    frame_gp_len = len -6;
    data += 6;
    report_count = 0;
    while(frame_gp_len > 0)
    {
//        rcsp_db_printf("frame_gp_len= %d\n",frame_gp_len);
        tmp16 = data[4]>>4;
        tmp32 =  READ_BIG_U16((data+6));
        data[4] = 0;
        rcsp_db_printf("\nframe_id = %s ,code_tpe= %d, len= %d\n",data,tmp16,tmp32);

        rcsp_put_buf(data+8, tmp32);

        memcpy(report_frame_p->id_str,data,4);
        report_frame_p->code_type = (u8)tmp16;
        report_frame_p->value_len= (u16)tmp32;
        report_frame_p->value_buf = data+8;
        report_count++;
        report_frame_p++;

        if(report_count == 8)
        {
            apptask_msg_cbk_deal(RSCP_CBK_MSG_REPORT_FRAME, framp_buf_addr,report_count * sizeof(task_frame_parm_t));
            report_frame_p = (task_frame_parm_t*)framp_buf_addr;
            report_count = 0;
        }

        data += (8 + tmp32);
        frame_gp_len -= (8 + tmp32);
    }

    if(report_count > 0)
    {
        apptask_msg_cbk_deal(RSCP_CBK_MSG_REPORT_FRAME, framp_buf_addr,report_count * sizeof(task_frame_parm_t));
    }

    free(framp_buf_addr);

    return 0;

}

static u32 app_request_frame_analysis(u8 *data,u16 len)
{
    u16 tmp16;
    u16 frame_gp_len;

    P_FUNCTION

    tmp16 = READ_BIG_U16(data);
    if(tmp16 == 0xffff)
    {
        tmp16 = READ_BIG_U16(data+2);
        if(tmp16 != cal_crc16_deal(data+6, len -6))
        {
            rcsp_err_printf("crc error\n");
        }
    }

    frame_gp_len = len -6;
    data += 6;

    if(0 != memcmp(data,"JLID",4))
    {
        return RCSP_OP_ERR_NO_CMD;
    }

    rcsp_db_puts("app req string:");
    frame_gp_len -= 8;
    data += 8;
    rcsp_put_buf(data,frame_gp_len);

    frame_gp_len = (frame_gp_len>>2)<<2;

    rcsp_db_printf("frame_gp_len= %d\n",frame_gp_len);

#ifdef TASK_LIGHT_APP
    if((frame_gp_len == 0) && (cur_cmd_opt_task_id  == TASK_LIGHT_APP))
    {
        tmp16 = (u16)apptask_msg_cbk_deal(RSCP_CBK_MSG_REQUEST_FRAME, (void*)"LMOD",4);
        return CSW_ERR_NO_ACK;
    }
    else
#endif

	{
        tmp16 = (u16)apptask_msg_cbk_deal(RSCP_CBK_MSG_REQUEST_FRAME, data,frame_gp_len);
        if(tmp16 & RCSP_CBK_ERR_MASK_FLAG)
        {
            return (tmp16 & 0x0ff);
        }

		if(tmp16 > 0)
		{
        	return CSW_ERR_NO_ACK;
		}
		else
		{
			return CSW_SUCC;
		}

	}
}



static u32 light_subcmd_deal(u8 *OperationData,u8 *data,u16 len)
{
    u32 err = RCSP_OP_ERR_NONE;
    u32 msg = RCSP_CBK_NO_MSG;
    u32 back_len = 0;

    switch(OperationData[1])
    {
    case 0:
        err = app_report_frame_analysis(data,len);
        break;

    case 1:
        apptask_msg_cbk_deal(RSCP_CBK_MSG_REQUEST_FRAME, (void*)"LMOD",4);
        err = CSW_ERR_NO_ACK;
        break;

    case 2:
        //light Control
        err = app_report_frame_analysis(data,len);
        break;

    case 3:
        //light mode
        err = app_report_frame_analysis(data,len);
        break;

    default:
        break;
    }

    if(msg != RCSP_CBK_NO_MSG)
    {
        err |= apptask_msg_cbk_deal(msg,&OperationData[2],back_len); //
    }
    return err;
}

/*
 *********************************************************************************************************
 *
 * Description: music 模式 控制命令处理
 *
 * Arguments  : cmd
 *
 * Returns    : 0-为成功，其他值 失败
 *
 * Note:
 *********************************************************************************************************
 */

extern u32 rcsp_fs_dir_cmd_deal(u8 *OperationData,u8 *data,u16 len);
static u32 music_subcmd_deal(u8 *OperationData,u8 *data,u16 len)
{
    u32 err = RCSP_OP_ERR_NONE;
    u32 msg = RCSP_CBK_NO_MSG;
    u16 back_len = 0;

    switch(OperationData[1])
    {
    case 0:
        err = app_report_frame_analysis(data,len);
        break;

    case 1:
        err = app_request_frame_analysis(data,len);
        break;

    case 2:
    {
        switch(OperationData[2])
        {
        case 0:
            msg = RCSP_CBK_MSG_COMM_PREV;
            break;

        case 1:
            msg = RCSP_CBK_MSG_COMM_NEXT;
            break;

        case 2:
            rcsp_db_puts("play by no\n");
            msg = RCSP_CBK_MSG_MUSIC_SET_BYNO;
            back_len = 1;
            break;

        case 3:
            msg = RCSP_CBK_MSG_MUSIC_DEL_CUR_FILE;
            break;

        default:
            break;

        }
    }
    break;

    case 3:
    {
        switch(OperationData[2])
        {
        case 0:
            msg = RCSP_CBK_MSG_COMM_PLAY;
            break;

        case 1:
            msg = RCSP_CBK_MSG_COMM_PAUSE;
            break;

        case 2:
            msg = RCSP_CBK_MSG_MUSIC_FF;
            back_len = 1;
            break;

        case 3:
            msg= RCSP_CBK_MSG_MUSIC_FR;
            back_len = 1;
            break;

        default:
            rcsp_db_puts("unknown cmd\n");
            break;

        }
    }
    break;

    case 4:
        break;

    case 5:
        err = app_report_frame_analysis(data,len);
        break;

    case 6:
    {
        switch(OperationData[2])
        {
        case 0:
            msg = RCSP_CBK_MSG_MUSIC_PREV_DEV;
            break;

        case 1:
            msg = RCSP_CBK_MSG_MUSIC_NEXT_DEV;
            break;

        case 2:
            msg = RCSP_CBK_MSG_MUSIC_SET_DEV;
            back_len = 1;
            break;

        default:
            break;

        }
    }
    break;

    case 7:
        msg = RCSP_CBK_MSG_COMM_PLAYMODE;
        OperationData[3] = OperationData[2];
        back_len = 1;
        break;

    case 8:
        switch(OperationData[2])
        {
        case 0:
            break;
        case 1:
            err = app_request_frame_analysis(data,len);
            break;

        default:
            break;

        }
        break;

    case 9:
        rcsp_db_puts("get id3\n");
        break;

    case 0xff:
        // set file's ext,exsample  MP3\WMA\WAV\FLA\APE
        rcsp_db_puts("set file ext\n");
        break;

    case 0xfe:
        // set path
        err = rcsp_fs_dir_cmd_deal(OperationData,data,len);
		break;

    default:
        break;
    }

    if(msg != RCSP_CBK_NO_MSG)
    {
        err |= apptask_msg_cbk_deal(msg,&OperationData[3],back_len); //
    }

    return err;
}

/*
 *********************************************************************************************************
 *
 * Description: radio 模式 控制命令处理
 *
 * Arguments  : cmd
 *
 * Returns    : 0-为成功，其他值 失败
 *
 * Note:
 *********************************************************************************************************
 */
static u32 radio_subcmd_deal(u8 *OperationData,u8 *data,u16 len)
{
    u32 err = RCSP_OP_ERR_NONE;
    u32 msg = NO_MSG;
    u16 back_len = 0;

    switch(OperationData[1])
    {
    case 0:
        err = app_report_frame_analysis(data,len);
        break;

    case 1:
        err = app_request_frame_analysis(data,len);
        break;

    case 2:
    {
        switch(OperationData[2])
        {
        case 0:
            msg = RCSP_CBK_MSG_RADIO_SEARCH_ALL;
            break;

        case 1:
            msg = RCSP_CBK_MSG_RADIO_SEARCH_BACK;
            break;

        case 2:
            msg = RCSP_CBK_MSG_RADIO_SEARCH_FORWARD;
            break;

        default:
            break;
        }

    }
    break;

    case 3:
    {
        switch(OperationData[2])
        {
        case 0:
            msg = RCSP_CBK_MSG_COMM_PREV;
            break;

        case 1:
            msg = RCSP_CBK_MSG_COMM_NEXT;
            break;

        case 2:
            msg = RCSP_CBK_MSG_RADIO_PREV_STEP;
            break;

        case 3:
            msg = RCSP_CBK_MSG_RADIO_NEXT_STEP;
            break;

        case 4:
            msg = RCSP_CBK_MSG_RADIO_SAVE_FREQ;
            back_len = 2;
            break;

        case 5:
            msg = RCSP_CBK_MSG_RADIO_DEL_CHL;
            back_len = 2;
            break;

        case 6:
            msg = RCSP_CBK_MSG_RADIO_SEL_CHL;
            back_len = 2;
            break;

        case 7:
            msg = RCSP_CBK_MSG_RADIO_SEL_FREQ;
            back_len = 2;
            break;

        case 8:
            msg = RCSP_CBK_MSG_COMM_PLAY;
            break;

        case 9:
            msg = RCSP_CBK_MSG_COMM_PAUSE;
            break;

        default:
            break;
        }

    }
    break;

    case 4:
        break;

    default:
        break;
    }

    if(msg != RCSP_CBK_NO_MSG)
    {
        err |=apptask_msg_cbk_deal(msg,&OperationData[3],back_len); //
    }
    return err;
}

/*
 *********************************************************************************************************
 *
 * Description: linein 模式 控制命令处理
 *
 * Arguments  : cmd
 *
 * Returns    : 0-为成功，其他值 失败
 *
 * Note:
 *********************************************************************************************************
 */
static u32 linein_subcmd_deal(u8 *OperationData,u8 *data,u16 len)
{
    u32 err = RCSP_OP_ERR_NONE;
    u32 msg = NO_MSG;
    u16 back_len = 0;

    switch(OperationData[1])
    {
    case 0:
        err = app_report_frame_analysis(data,len);
        break;

    case 1:
        err = app_request_frame_analysis(data,len);
        break;

    case 2:
        switch(OperationData[2])
        {
        case 0:
            msg = RCSP_CBK_MSG_COMM_PLAY;
            break;

        case 1:
            msg = RCSP_CBK_MSG_COMM_PAUSE;
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }

    if(msg != RCSP_CBK_NO_MSG)
    {
        err |= apptask_msg_cbk_deal(msg,&OperationData[3],back_len); //
    }
    return err;
}

/*
 *********************************************************************************************************
 *
 * Description: udisk 模式 控制命令处理
 *
 * Arguments  : cmd
 *
 * Returns    : 0-为成功，其他值 失败
 *
 * Note:
 *********************************************************************************************************
 */
static u32 udisk_subcmd_deal(u8 *OperationData,u8 *data,u16 len)
{
    u32 err = RCSP_OP_ERR_NONE;
    u32 msg = NO_MSG;
    u16 back_len = 0;

    switch(OperationData[1])
    {
    case 0:
        err = app_report_frame_analysis(data,len);
        break;

    case 1:
        err = app_request_frame_analysis(data,len);
        break;

    case 2:
        switch(OperationData[2])
        {
        case 0:
            msg = RCSP_CBK_MSG_COMM_PLAY;
            break;

        case 1:
            msg = RCSP_CBK_MSG_COMM_PAUSE;
            break;

        case 2:
            msg = RCSP_CBK_MSG_COMM_PREV;
            break;

        case 3:
            msg = RCSP_CBK_MSG_COMM_NEXT;
            break;

        default:
            break;

        }
        break;

    default:
        break;
    }

    if(msg != RCSP_CBK_NO_MSG)
    {
        err |= apptask_msg_cbk_deal(msg,&OperationData[3],back_len); //
    }

    return err;
}


/*
 *********************************************************************************************************
 *
 * Description: bt 模式 控制命令处理
 *
 * Arguments  : cmd
 *
 * Returns    : 0-为成功，其他值 失败
 *
 * Note:
 *********************************************************************************************************
 */
static u32 bt_subcmd_deal(u8 *OperationData,u8 *data,u16 len)
{
    u32 err = RCSP_OP_ERR_NONE;
    u32 msg = RCSP_CBK_NO_MSG;
    u32 back_len = 0;

    switch(OperationData[1])
    {
    case 0:
        err = app_report_frame_analysis(data,len);
        break;

    case 1:
        err = app_request_frame_analysis(data,len);
        break;

    case 2:
    {
        switch(OperationData[2])
        {
        case 0:
            msg = RCSP_CBK_MSG_COMM_PREV;
            break;
        case 1:
            msg = RCSP_CBK_MSG_COMM_NEXT;
            break;
        case 2:
            msg = RCSP_CBK_MSG_COMM_PLAY;
            break;
        case 3:
            msg = RCSP_CBK_MSG_COMM_PAUSE;
            break;

        default:
            break;

        }
    }
    break;

    default:
        break;
    }

    if(msg != RCSP_CBK_NO_MSG)
    {
        err |= apptask_msg_cbk_deal(msg,&OperationData[3],back_len); //
    }
    return err;
}
/*
 *********************************************************************************************************
 *
 * Description: 公共控制命令处理
 *
 * Arguments  : cmd
 *
 * Returns    : 0-为成功，其他值 失败
 *
 * Note:
 *********************************************************************************************************
 */
static u32 app_submode_cmd_parse(u8 *OperationData,u8 *data,u16 len)
{
    u32 err = RCSP_OP_ERR_NONE;
    app_subcmd_cbk_t subcmd_cbk;
   	
	P_FUNCTION	

	if(OperationData[0] != CBW_CMD_TASK_SECTION)
	{
		return RCSP_OP_ERR_WORDMODE; 
	}
	if(OperationData[1] >= TASK_APP_MAX)
	{
		return RCSP_OP_ERR_WORDMODE; 
	}
	cur_cmd_opt_task_id = OperationData[1];
	apptask_msg_cbk_deal = support_app_msg_callback_group[cur_cmd_opt_task_id];     
	subcmd_cbk = task_subcmd_cbk_group[cur_cmd_opt_task_id];

	err = subcmd_cbk(OperationData + 1,data,len);

	return err;
}



/*
 *********************************************************************************************************
 *
 * Description: 公共控制命令处理
 *
 * Arguments  : cmd
 *
 * Returns    : 0-为成功，其他值 失败
 *
 * Note:
 *********************************************************************************************************
 */
static u32 app_public_cmd_parse(u8 *OperationData,u8 *data,u16 len)
{
    u32 err = RCSP_OP_ERR_NONE,tmp32;
    u32 msg = RCSP_CBK_NO_MSG;
    u16 back_len = 0;
    u16 buf_offset = 0;
    u8 byte_val;

    P_FUNCTION
			
	apptask_msg_cbk_deal = support_app_msg_callback_group[rcsp_curr_task_id];     

    switch (OperationData[0])
    {

    case CBW_CMD_VOL_UP:
        msg = RCSP_CBK_MSG_COMM_VOL_UP;
        break;

    case CBW_CMD_VOL_DOWN:
        msg = RCSP_CBK_MSG_COMM_VOL_DOWN;
        break;

    case CBW_CMD_VOL_SET:
        msg = RCSP_CBK_MSG_COMM_SET_VOL;
        back_len = 1;
        break;

    case CBW_CMD_CHANGE_MODE:
        rcsp_db_puts("cmm app request change mode\n");
        if(OperationData[1] < TASK_APP_MAX)
        {
            tmp32 = (u32)&support_taskname_group[OperationData[1]];
			err = apptask_msg_cbk_deal(RCSP_CBK_MSG_CHANGE_MODE,(void*)&tmp32,4);
			os_time_dly(5);//wait switch task ok
		}
        else
        {
            rcsp_db_puts("app_callback is null\n");
            err = RCSP_OP_ERR_NO_APP;
        }
        break;

    case CBW_CMD_GET_MODE_INFO:
        rcsp_db_puts("cmm get mode info\n");
		err = fw_report_support_app_info((void*)app_mode_name_string,TASK_APP_MAX);
		break;

    case CBW_CMD_SET_TIMEOUT:
        break;

    case CBW_CMD_SET_EQ:
        rcsp_db_puts("cmm set eq\n");
        buf_offset = 1;
        switch(OperationData[1])
        {
        case 0:
            msg = RCSP_CBK_MSG_COMM_EQ;
            back_len = 1;
            break;

        case 1:
            msg = RCSP_CBK_MSG_COMM_SET_EQ;
            back_len = 11;
            break;

        case 2:
            msg = RCSP_CBK_MSG_COMM_SAVE_EQ;
            break;

        case 3:
            msg= RCSP_CBK_MSG_COMM_RESET_EQ;
            back_len = 1;
            break;

        default:
            rcsp_db_puts("unknown cmd\n");
            break;
        }
        break;

    default:
        err = RCSP_OP_ERR_NO_CMD;
        break;
    }

    if(msg != RCSP_CBK_NO_MSG)
    {
        err = apptask_msg_cbk_deal(msg,&OperationData[buf_offset+1],back_len);
    }

/*
    if(err == RCSP_OP_ERR_NONE)
    {
        err = CSW_SUCC;
    }
    else
    {
        if(err >= CSW_ERR_MAX)
        {
            byte_val = err;
            err = CSW_ERR_OPT;
        }
    }
*/
    return err;
}

u32 fill_mode_control_state(u8 *OperationData,u32 ctl_staus)
{
    u32 err = RCSP_OP_ERR_NONE;
	/* u8 *OperationData = rcsp_send_tmp_buf;/// + (sizeof(task_frame_parm_t)*3); */
    P_FUNCTION

	if(OperationData == NULL)
	{
        return err;
	}

    rcsp_db_printf("current_task_id= %d\n",rcsp_curr_task_id);

    OperationData[0] = rcsp_curr_task_id;

    switch(rcsp_curr_task_id)
    {
    case TASK_BTSTACK_APP:
        OperationData[1] = 2;
        if(STAUS_PLAY == ctl_staus)
        {
            OperationData[2] = 2;
        }
        else
        {
            OperationData[2] = 3;
        }
        break;

    case TASK_MUSIC_APP:
        OperationData[1] = 3;
        if(STAUS_PLAY == ctl_staus)
        {
            /* current_task_status = STAUS_PLAY; */
            OperationData[2] = 0;
        }
        else
        {
            /* current_task_status = STAUS_PAUSE; */
            OperationData[2] = 1;
        }
        break;

#ifdef TASK_LIGHT_APPP
    case TASK_LIGHT_APP:
        err = RCSP_OP_ERR_NO_APP;
        break;
#endif

#ifdef TASK_RADIO_APP
    case TASK_RADIO_APP:
        OperationData[1] = 3;
        if(STAUS_PLAY == ctl_staus)
        {
            OperationData[2] = 8;
        }
        else if(STAUS_PAUSE == ctl_staus)
        {
            OperationData[2] = 9;
        }
        else if(STAUS_SEARCHING == ctl_staus)
        {
            OperationData[1] = 2;
            OperationData[2] = 0;
        }
        else
        {
            ;
        }
        break;
#endif

    case TASK_LINEIN_APP:
        OperationData[1] = 2;
        if(STAUS_PLAY == ctl_staus)
        {
            OperationData[2] = 0;
        }
        else
        {
            OperationData[2] = 1;
        }
        break;

#ifdef TASK_UDISK_APP
    case TASK_UDISK_APP:
        OperationData[1] = 2;
        if(STAUS_PLAY == ctl_staus)
        {
            OperationData[2] = 0;
        }
        else
        {
            OperationData[2] = 1;
        }
        break;
#endif

    default :
        err = RCSP_OP_ERR_NO_APP;
        break;
    }

    if(err == RCSP_OP_ERR_NONE)
    {
		/* err = fw_report_ctl_state_send(OperationData); */
           /* Post_msg_to_rcsp_Task_do(MSG_RSCP_REPORT_CTL_STATE, (u32)OperationData,1); */
    }

    return err;

}

/*
static u8 light_sw_state;
u32 fw_report_light_state(u32 sw_state)
{
//    u32 err = RCSP_OP_ERR_NONE;

    P_FUNCTION
    light_sw_state = (u8)sw_state;
    return fw_report_light_state_send(light_sw_state);
}
*/

extern u32 app_get_cmd_event_callback(u32 event_type);
void rcsp_cmd_register_callback(void)
{
	rcsp_set_cmd_callback(app_public_cmd_parse,app_submode_cmd_parse,app_get_cmd_event_callback);
}

#endif // APP_RCSP_LIB_EN





