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
#include "music/music_info.h"
#include "dac/eq_api.h"

#if SUPPORT_APP_RCSP_EN

extern u8 rcsp_cbk_answer_data_send(task_frame_parm_t *frame_gp,u16 frame_len);


typedef enum
{
    JL_FRAME_ID_MODN = 0,//0
    JL_FRAME_ID_MODI,
    JL_FRAME_ID_TOTF,
    JL_FRAME_ID_CURF,
    JL_FRAME_ID_FILN,

    JL_FRAME_ID_TOTT,//5
    JL_FRAME_ID_SATT,
    JL_FRAME_ID_SART,
    JL_FRAME_ID_BTRT,
    JL_FRAME_ID_CHAL,

    JL_FRAME_ID_MVOL,//10
    JL_FRAME_ID_CVOL,
    JL_FRAME_ID_MDEV,
    JL_FRAME_ID_CDEV,
    JL_FRAME_ID_DEVN,

    JL_FRAME_ID_EQSL,//15
    JL_FRAME_ID_EQVL,
    JL_FRAME_ID_EQCU,
    JL_FRAME_ID_FMTF,
    JL_FRAME_ID_FMCF,

    JL_FRAME_ID_FMCP,//20
    JL_FRAME_ID_PATH,
    JL_FRAME_ID_FITP,
    JL_FRAME_ID_PLMD,
    JL_FRAME_ID_PLCU,

    JL_FRAME_ID_JLID,//25
    JL_FRAME_ID_LMOD,
    JL_FRAME_ID_LBRI,
    JL_FRAME_ID_LEFF,
    JL_FRAME_ID_LWHI,

    JL_FRAME_ID_LRED,//30
    JL_FRAME_ID_LGRR,
    JL_FRAME_ID_LBLU,
    JL_FRAME_ID_LSCE,
    JL_FRAME_ID_LBLK,

    JL_FRAME_ID_CURT,//35
    JL_FRAME_ID_APIC,
	JL_FRAME_ID_PLST,

    //ADD
    //...

    //END

    AP_FRAME_ID_MAX,
} fram_id_e;

const char JL_frame_id_group[AP_FRAME_ID_MAX][4] =
{
    {"MODN"},//0
    {"MODI"},//
    {"TOTF"},//
    {"CURF"},//
    {"FILN"},//

    {"TOTT"},//5
    {"SATT"},//
    {"SART"},//
    {"BTRT"},//
    {"CHAL"},//

    {"MVOL"},//10
    {"CVOL"},//
    {"MDEV"},//
    {"CDEV"},//
    {"DEVN"},//

    {"EQSL"},//15
    {"EQVL"},//
    {"EQCU"},//
    {"FMTF"},//
    {"FMCF"},//

    {"FMCP"},//20
    {"PATH"},//
    {"FITP"},//
    {"PLMD"},//
    {"PLCU"},//

    {"JLID"},//25
    {"LMOD"},//
    {"LBRI"},//
    {"LEFF"},//
    {"LWHI"},//

    {"LRED"},//30
    {"LGRR"},//
    {"LBLU"},//
    {"LSCE"},//
    {"LBLK"},//

    {"CURT"},//35
    {"APIC"},
    {"PLST"},

};

///------------------------------------------------------------
#if 0
const light_flash_param_t test_scene_red[]=
{
    {0,0,1},
    {255,255,1},
    {0,0,1},
};

const light_flash_param_t test_scene_green[]=
{
    {200,255,1},
    {255,255,1},
    {0,0,1},
};

const light_flash_param_t test_scene_blue[]=
{
    {0,0,2},
    {200,255,1},
};

const light_flash_param_t test_scene_freq[]=
{
    {3,0,3},
};
#endif
///------------------------------------------------------------
#if 1
const light_flash_param_t test_scene_red[]=
{
    {225,225,3},
    {0,0,1},
    {124,124,1},
    {0,0,1},
    {160,160,1},
};

const light_flash_param_t test_scene_green[]=
{
    {0,0,1},
    {97,97,1},
    {255,255,2},
    {252,252,1},
    {0,0,1},
    {32,32,1}
};

const light_flash_param_t test_scene_blue[]=
{
    {0,0,5},
    {255,255,1},
    {240,240,1},
};

const light_flash_param_t test_scene_freq[]=
{
    {14,0,7},
};
#endif

///------------------------------------------------------------


static u8 get_frame_id_table(char *id_str)
{
    u8 cnt;

    for(cnt = 0; cnt < AP_FRAME_ID_MAX; cnt++)
    {
        if(0 == memcmp(id_str,JL_frame_id_group[cnt],4))
        {
            break;
        }
    }
    rcsp_db_printf("get framde_id= %d\n",cnt);
    return cnt;
}


static u8 get_eq_from_bitmap(u8 bit_map)
{
    u8 cnt;

    P_FUNCTION
    rcsp_db_printf("bit_map= 0x%x\n",bit_map);

    for(cnt = 0; cnt < 7; cnt++)
    {
        if((bit_map&0x01) != 0)
        {
            return rcspEQ_to_sysEQ_talbe[cnt];
        }
        bit_map = bit_map>>1;
    }
    return 0x7f;
}

static u8 get_playmode_from_bitmap(u8 bit_map)
{
    u8 cnt;

    P_FUNCTION
    rcsp_db_printf("bit_map= 0x%x\n",bit_map);

    for(cnt = 0; cnt < 7; cnt++)
    {
        if((bit_map&0x01) != 0)
        {
            return cnt;
        }
        bit_map = bit_map>>1;
    }
    return 0x7f;
}

void rcsp_update_eq_table_do(s8 *eq_table,u16 offset,u16 eq_size)
{

#if EQ_EN
	u8 cur_mode;

	puts("update eq_table......\n");


	if(rcsp_eq_sw_flag)
	{
		update_eq_table_data(eq_table,offset,eq_size,(void*)user_eq_global_gain_default);    
	}
	else
	{
		update_eq_table_data(eq_table,offset,eq_size,(void*)user_eq_global_gain_default_all0);    
	}
		
	if(0 == strcmp(curr_task->name,RDEF_BTSTACK_TASK_NAME))
    {
        cur_mode =  sysEQ_to_rcspEQ_talbe[get_bt_eq_mode()];
    }
    else
    {
        cur_mode =  sysEQ_to_rcspEQ_talbe[eq_mode];
    }
	eq_mode_sw(cur_mode);	
#endif
}

static void modify_eq_new_value(u8 eq_sn,s8 *eq_value)
{
    int cnt;

    P_FUNCTION
    rcsp_db_printf("eq_sn=  %d\n",eq_sn);

    for(cnt = 0; cnt < 10; cnt++)
    {
        rcsp_user_eq_seg_gain[eq_sn][cnt] = eq_value[cnt];
    }
	rcsp_update_eq_table_do(rcsp_user_eq_seg_gain[eq_sn],eq_sn*10,10);
}

static void reset_eq_new_value(u8 eq_sn)
{
    int cnt;

    P_FUNCTION
    rcsp_db_printf("eq_sn=  %d\n",eq_sn);

    memcpy(rcsp_user_eq_seg_gain[eq_sn],user_eq_seg_gain_default[eq_sn],10);
	rcsp_update_eq_table_do(rcsp_user_eq_seg_gain[eq_sn],eq_sn*10,10);
	rcsp_com_report_flag |= (RCSP_REPORT_EQ_CONFIG);
}

static void save_eq_table(void)
{
    int cnt;

    P_FUNCTION

    rcsp_put_buf((void*)rcsp_user_eq_seg_gain,60);

}

void check_eq_sw_update(void)
{
#if EQ_EN
	if(rcsp_eq_sw_flag == 1)
	{
		rcsp_update_eq_table_do((void*)rcsp_user_eq_seg_gain,0,sizeof(rcsp_user_eq_seg_gain));
		eq_enable();
	}
	else
	{
		eq_disable();
/*
		u8 cn;
		for(cnt = 0; cnt < 6; cnt++)
		{
			rcsp_update_eq_table_do((void*)&user_eq_seg_gain_default[5],cnt * 10,10);
		}
*/
	}
#endif
}

u16 Request_Light_Info_to_rcsp(task_frame_parm_t *frame_input,u8 *buf,u16 *buf_pt)
{
    u32 tmp32;
    u8 tmp8;
    u16 cnt = 0;
    task_frame_parm_t *frame_p = (void*)frame_input;
    u8 *value_buf_addr = buf;

    P_FUNCTION

    if(light_info_tab.button)
    {
        if(light_info_tab.mode == LGT_MD_SCENE)
        {
            value_buf_addr[0] = 0xff;
            value_buf_addr[1] = light_info_tab.m_value[0];
        }
        else
        {
            value_buf_addr[0] = 0;
            value_buf_addr[1] = light_info_tab.mode;
        }
    }
    else
    {
        value_buf_addr[0] = 0;
        value_buf_addr[1] = 0;
    }

    packet_frame_data(frame_p+cnt,NULL,"LMOD",FRAME_TYPE_ANSI,value_buf_addr,2);
    cnt++;
    value_buf_addr += 2;

    if(light_info_tab.mode == LGT_MD_COLOR_FLASH)
    {
        packet_frame_data(frame_p+cnt,NULL,"LEFF",FRAME_TYPE_ANSI,(void*)&light_info_tab.m_value[0],1);
        cnt++;
    }

    if(light_info_tab.mode == LGT_MD_WHITE)
    {
        packet_frame_data(frame_p+cnt,NULL,"LWHI",FRAME_TYPE_ANSI,(void*)&light_info_tab.color_value[LGT_COLOR_WHITE],1);
        cnt++;
    }

    if(light_info_tab.mode == LGT_MD_COLOR)
    {
        packet_frame_data(frame_p+cnt,NULL,"LRED",FRAME_TYPE_ANSI,(void*)&light_info_tab.color_value[LGT_COLOR_RED],1);
        cnt++;

        packet_frame_data(frame_p+cnt,NULL,"LGRR",FRAME_TYPE_ANSI,(void*)&light_info_tab.color_value[LGT_COLOR_GREEN],1);
        cnt++;

        packet_frame_data(frame_p+cnt,NULL,"LBLU",FRAME_TYPE_ANSI,(void*)&light_info_tab.color_value[LGT_COLOR_BLUE],1);
        cnt++;
    }

    if(light_info_tab.button != 0)
    {
        packet_frame_data(frame_p+cnt,NULL,"LBRI",FRAME_TYPE_ANSI,(void*)&light_info_tab.color_value[LGT_COLOR_BRIGHT],1);
        cnt++;
    }

    *buf_pt =  *buf_pt + (u8)(value_buf_addr - buf);

    return cnt;
}


u32 cbk_send_apic_data(void)
{
    u8 play_flag = 0;
    u32 send_cnt = 0;

    P_FUNCTION
	return 0;
}

#define CBK_BACK_FRAME_DATA_SIZE    384

static u32 cbk_request_frame_deal(u8 *data,u16 len)
{
    u16 cnt,data_cnt,frame_cnt;
    u32 tmp32,send_data_cnt;
    u8  id;
    u8  *id_str = data;
    u32 err = RCSP_CBK_ERR_NONE;

    task_frame_parm_t *frame_p;
    u8  *data_buf_addr;

    P_FUNCTION;

    rcsp_db_printf("len= %d\n",len);

    if((data == NULL) || (len == 0))
    {
        //resend all info
        rcsp_db_puts("resend all info\n");
        rcsp_report_status(RCSP_REPORT_START_CONFIG);
        rcsp_report_status(RCSP_REPORT_STATUS_CONTROL);
        return RCSP_CBK_ERR_NONE;
    }

    data_buf_addr = malloc(CBK_BACK_FRAME_DATA_SIZE + sizeof(task_frame_parm_t) * 8);

    CHECK_MALLOC(data_buf_addr,1);

    frame_p = (void*)(data_buf_addr + CBK_BACK_FRAME_DATA_SIZE);
    frame_cnt = 0;
    data_cnt = 0;
    send_data_cnt = 0;

#if 0
    for(cnt = 0; cnt < len; cnt ++)
    {
        putchar(id_str[cnt]);
    }
    putchar('\n');
#endif

    for(cnt = 0; cnt < len; cnt += 4)
    {
        rcsp_db_printf("request frame:  %d   ",cnt);

        if(frame_cnt >= 6)
        {
            frame_p = (void*)(data_buf_addr + CBK_BACK_FRAME_DATA_SIZE);
            if(0 != rcsp_cbk_answer_data_send(frame_p, frame_cnt * sizeof(task_frame_parm_t)))
            {
                frame_cnt = 0;
                break;
            }
            frame_cnt = 0;
            data_cnt = 0;
            send_data_cnt += frame_cnt;
        }

        id = get_frame_id_table((void*)&id_str[cnt]);

        if(id >= AP_FRAME_ID_MAX)
        {
            rcsp_db_puts("ID_MAX\n");
            continue;
        }

        frame_p->code_type = 1;
        frame_p->value_len = 0;
        frame_p->value_buf = data_buf_addr + data_cnt;

        memcpy(frame_p->id_str,&id_str[cnt],4);
        frame_p->reserved[0] = 0;
        rcsp_db_printf("id_str: %s\n",frame_p->id_str);

        switch(id)
        {
        case JL_FRAME_ID_MODN: //0
        case JL_FRAME_ID_MODI:
        case JL_FRAME_ID_TOTF:
        case JL_FRAME_ID_CURF:
        case JL_FRAME_ID_FILN:

        case JL_FRAME_ID_SART:
        case JL_FRAME_ID_BTRT:
        case JL_FRAME_ID_CHAL:
            break;

        case JL_FRAME_ID_TOTT://5
            data_buf_addr[data_cnt++] = (u8)(music_playfile_total_time>>8)&0xff;
            data_buf_addr[data_cnt++] = (u8)(music_playfile_total_time)&0xff;
            frame_p->value_len = 2;
            break;

        case JL_FRAME_ID_CURT:
        case JL_FRAME_ID_SATT:
            tmp32 = get_decode_time();
            data_buf_addr[data_cnt++] = (u8)(tmp32>>8)&0xff;
            data_buf_addr[data_cnt++] = (u8)(tmp32)&0xff;
            frame_p->value_len = 2;
            break;

        case JL_FRAME_ID_MVOL://10
            data_buf_addr[data_cnt++] = rcsp_vol_max_value;
            frame_p->value_len = 1;
            break;

        case JL_FRAME_ID_CVOL:
            data_buf_addr[data_cnt++] = dac_ctl.sys_vol_l;
            frame_p->value_len = 1;
            break;

        case JL_FRAME_ID_MDEV:
        case JL_FRAME_ID_CDEV:
        case JL_FRAME_ID_DEVN:
            break;

        case JL_FRAME_ID_EQSL://15
            data_buf_addr[data_cnt++] = RCSP_SYS_EQ_SL;
            frame_p->value_len = 1;
            break;

        case JL_FRAME_ID_EQVL:
            get_eq_table_value((void*)&data_buf_addr[data_cnt]);
            frame_p->value_len = 6*10;
            data_cnt += (6*10);
            break;

        case JL_FRAME_ID_EQCU:
            if(rcsp_eq_sw_flag == 0)
            {
                data_buf_addr[data_cnt++] = 0;
            }
            else
            {
                if(0 == strcmp(RDEF_BTSTACK_TASK_NAME,curr_task->name))
                {
                    data_buf_addr[data_cnt++] = 1 << sysEQ_to_rcspEQ_talbe[get_bt_eq_mode()];
                }
                else
                {
                    data_buf_addr[data_cnt++] = 1 << sysEQ_to_rcspEQ_talbe[eq_mode];
                }
            }
            frame_p->value_len = 1;
            break;

        case JL_FRAME_ID_FMTF:
        case JL_FRAME_ID_FMCF:

        case JL_FRAME_ID_FMCP://20
        case JL_FRAME_ID_PATH:
        case JL_FRAME_ID_FITP:
            break;

        case JL_FRAME_ID_PLMD:
            data_buf_addr[data_cnt++] = RCSP_SYS_PLAYMODE_SL;
            frame_p->value_len = 1;
            break;

        case JL_FRAME_ID_PLCU:
            data_buf_addr[data_cnt++] = play_mode - REPEAT_ALL;
            frame_p->value_len = 1;
            break;

        case JL_FRAME_ID_LMOD:
//            rcsp_com_report_flag |= RCSP_REPORT_LIGHT_INFO;
            tmp32 = (u32)Request_Light_Info_to_rcsp(frame_p,data_buf_addr + data_cnt,&data_cnt);
            frame_p += tmp32;
            frame_cnt += tmp32;
            continue;
            break;

        case JL_FRAME_ID_APIC:
            send_data_cnt = cbk_send_apic_data();
            if(send_data_cnt == 0)
            {
                rcsp_db_puts("---ID3---no_apic-----\n");
                err = RCSP_CBK_ERR_MASK_FLAG|RCSP_CBK_MUSIC_ERR_NO_APIC;
            }
            goto requset_end;
            break;

		case JL_FRAME_ID_PLST:
			if(rcsp_cur_task_status == STAUS_PLAY)
			{
				data_buf_addr[data_cnt++] = 0;
			}
			else
			{
				data_buf_addr[data_cnt++] = 1;
			}
            frame_p->value_len = 1;
			break;

        case AP_FRAME_ID_MAX:
            break;
        default:
            break;

        }
        frame_cnt++;
        frame_p++;
    }

    if(frame_cnt > 0)
    {
        rcsp_db_printf("frame_cnt= %d\n",frame_cnt);
        frame_p = (void*)(data_buf_addr + CBK_BACK_FRAME_DATA_SIZE);
        rcsp_cbk_answer_data_send(frame_p, frame_cnt * sizeof(task_frame_parm_t));
        send_data_cnt += frame_cnt;
    }

requset_end:
    free(data_buf_addr);
    if(send_data_cnt > 0)
	{
		return send_data_cnt;	
	}
	else
	{
		return err;
	}
}

static u32 cbk_post_msg_to_task(char *task_name,int msg,int *param,int param_cnt)
{

    if(msg < MSG_MAIN_MAX)
    {
        task_name = RDEF_MAIN_TASK_NAME;
    }

    rcsp_db_printf("post  <%s>  msg_cnt= %d ,%x,%x,%x\n",task_name,param_cnt,msg,param[0],msg,param[1],msg,param[2]);

    switch(param_cnt)
    {
    case 0:
        os_taskq_post(task_name,1,msg);
        break;

    case 1:
        os_taskq_post(task_name,2,msg,param[0]);
        break;

    case 2:
        os_taskq_post(task_name,3,msg,param[0],param[1]);
        break;

    case 3:
        os_taskq_post(task_name,4,msg,param[0],param[1],param[2]);
        break;

    default:
        break;
    }

    return 0;
}


extern const int task_connect_dev_valsize;
extern const struct task_dev_info *task_connect_dev[];

static u32 fw_dev_is_exist(const char *check_task_name)
{
#if 1
    u8 i;
    u32 dev_cnt;

    dev_cnt = task_connect_dev_valsize / sizeof(int);
    //    rcsp_printf("is-dev: %d \n",dev_cnt);

    for (i = 0; i < dev_cnt; i++)
    {
        if (0 == strcmp((void*) check_task_name, (void*) (task_connect_dev[i]->name)))
        {
            if (0 == task_connect_dev[i]->dev_check())
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
#endif
    return TRUE;
}



static u32 cbk_change_taskmode(char *next_task_name)
{
    P_FUNCTION

    rcsp_db_printf("rcsp: app switch task, %s\n",next_task_name);

    if(0 == strcmp(next_task_name,RDEF_MUSIC_TASK_NAME))
    {
        if(FALSE == fw_dev_is_exist(next_task_name))
        {
            rcsp_err_puts("rcsp:sw music fail,dev is not exist\n");
            return RCSP_CBK_ERR_MASK_FLAG|RCSP_CBK_MUSIC_ERR_NO_DEV;
        }
    }

    else if(0 == strcmp(next_task_name,RDEF_LINEIN_TASK_NAME))
    {
        if(FALSE == fw_dev_is_exist(next_task_name))
        {
            rcsp_err_puts("rcsp:sw linein fail,dev is not exist\n");
            return RCSP_CBK_ERR_MASK_FLAG|RCSP_CBK_MUSIC_ERR_NO_DEV;
        }
    }

    else if(0 == strcmp(next_task_name,RDEF_UDISK_TASK_NAME))
    {
        if(FALSE == fw_dev_is_exist(next_task_name))
        {
            rcsp_err_puts("rcsp:sw udisk fail,dev is not exist\n");
            return RCSP_CBK_ERR_MASK_FLAG|RCSP_CBK_MUSIC_ERR_NO_DEV;
        }
    }

    else if(0 == strcmp(next_task_name,RDEF_LIGHT_TASK_NAME))
    {
//        os_taskq_post(RDEF_MAIN_TASK_NAME, 2, SYS_EVENT_TASK_RUN_REQ, RDEF_IDLE_TASK_NAME);
//        os_taskq_post(RDEF_MAIN_TASK_NAME, 2, SYS_EVENT_TASK_RUN_REQ, RDEF_LIGHT_TASK_NAME);
//        return RCSP_CBK_ERR_NONE;
    }

    else if(0 == strcmp(next_task_name,RDEF_BTSTACK_TASK_NAME))
    {
        ;
    }

    else
    {
        ;
    }

    if(0 == strcmp(next_task_name,curr_task->name))
    {
        //resend frame
        rcsp_app_start(next_task_name);
    }
    else
    {
        os_taskq_post(RDEF_MAIN_TASK_NAME, 2, SYS_EVENT_TASK_RUN_REQ, next_task_name);
    }

    return RCSP_CBK_ERR_NONE;
}


//change time_value,cycle
u8 light_save_flash_cfg(light_flash_t *dest,u8 *src, u16 len)
{
    u16 cnt;
    u16 cycle = 0;

    for(cnt = 0; cnt < len; cnt += sizeof(light_flash_param_t))
    {
        dest->start_value = src[1] + (src[0] << 8);
        dest->end_value = src[3] + (src[2] << 8);
        dest->time_value = (src[5] + (src[4] << 8));
        cycle += dest->time_value;
        dest->time_order = cycle;///note
        dest++;
        src += sizeof(light_flash_param_t);
    }
    return cycle;
}

u8 light_save_flash_cfg_test(light_flash_t *dest,u8 *src, u16 len)
{
    u16 cnt;
    u16 cycle = 0;

    for(cnt = 0; cnt < len; cnt += sizeof(light_flash_param_t))
    {
        dest->start_value = src[0] + (src[1] << 8);
        dest->end_value = src[2] + (src[3] << 8);
        dest->time_value = (src[4] + (src[5] << 8));
        cycle += dest->time_value;
        dest->time_order = cycle;///note
        dest++;
        src += sizeof(light_flash_param_t);
    }
    return cycle;
}

void light_scene_frame_deal_test(void)
{
    u16 cnt,tmp16;
    static u8 on_off_flag = 0;

    if(on_off_flag != 0)
    {
        return;
    }

    P_FUNCTION

    on_off_flag = 1;

    light_sence_table = malloc(sizeof(light_disp_t) * LGT_SCENE_UNIT_MAX);
    CHECK_MALLOC(light_sence_table,1);

    memset(light_sence_table,0x00,sizeof(light_disp_t) * LGT_SCENE_UNIT_MAX);

#if 0
    tmp16 = sizeof(test_scene_red)/sizeof(light_flash_param_t);
    rcsp_put_buf((void*)test_scene_red,sizeof(test_scene_red));

    light_sence_table[LGT_SCENE_RED].tab_cnt = tmp16;
    light_sence_table[LGT_SCENE_RED].tab_cycle = light_save_flash_cfg_test(light_sence_table[LGT_SCENE_RED].flash_struct,(void*)test_scene_red,sizeof(test_scene_red));
    rcsp_db_puts("SCENE_RED--:\n");
    rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_RED].tab_cnt,light_sence_table[LGT_SCENE_RED].tab_cycle);
    rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_RED].flash_struct,sizeof(light_flash_t) * tmp16);



    tmp16 = sizeof(test_scene_green)/sizeof(light_flash_param_t);
    rcsp_put_buf((void*)test_scene_green,sizeof(test_scene_green));

    light_sence_table[LGT_SCENE_GREEN].tab_cnt = tmp16;
    light_sence_table[LGT_SCENE_GREEN].tab_cycle = light_save_flash_cfg_test(light_sence_table[LGT_SCENE_GREEN].flash_struct,(void*)test_scene_green,sizeof(test_scene_green));
    rcsp_db_puts("SCENE_GREEN--:\n");
    rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_GREEN].tab_cnt,light_sence_table[LGT_SCENE_GREEN].tab_cycle);
    rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_GREEN].flash_struct,sizeof(light_flash_t) * tmp16);


    tmp16 = sizeof(test_scene_blue)/sizeof(light_flash_param_t);
    rcsp_put_buf((void*)test_scene_blue,sizeof(test_scene_blue));

    light_sence_table[LGT_SCENE_BLUE].tab_cnt = tmp16;
    light_sence_table[LGT_SCENE_BLUE].tab_cycle = light_save_flash_cfg_test(light_sence_table[LGT_SCENE_BLUE].flash_struct,(void*)test_scene_blue,sizeof(test_scene_blue));
    rcsp_db_puts("SCENE_BLUE--:\n");
    rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_BLUE].tab_cnt,light_sence_table[LGT_SCENE_BLUE].tab_cycle);
    rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_BLUE].flash_struct,sizeof(light_flash_t) * tmp16);


    tmp16 = sizeof(test_scene_freq)/sizeof(light_flash_param_t);
    rcsp_put_buf((void*)test_scene_freq,sizeof(test_scene_freq));

    light_sence_table[LGT_SCENE_FREQ].tab_cnt = tmp16;
    light_sence_table[LGT_SCENE_FREQ].tab_cycle = light_save_flash_cfg_test(light_sence_table[LGT_SCENE_FREQ].flash_struct,(void*)test_scene_freq,sizeof(test_scene_freq));
    rcsp_db_puts("SCENE_LBLK--:\n");
    rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_FREQ].tab_cnt,light_sence_table[LGT_SCENE_FREQ].tab_cycle);
    rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_FREQ].flash_struct,sizeof(light_flash_t) * tmp16);


    light_control_set(light_sence_table,2);
#endif

    free(light_sence_table);
    light_sence_table = NULL;

}

u32 light_scene_frame_deal(u8 *data,u16 len)
{
    u16 cnt,tmp16;
    u8  code_type,id;
    u8 *id_str = data;
    u32 err = RCSP_CBK_ERR_NONE;

    P_FUNCTION

    memset(light_sence_table,0x00,sizeof(light_disp_t) * LGT_SCENE_UNIT_MAX);

//#if 0
//    tmp16 = sizeof(test_scene_red)/sizeof(light_flash_param_t);
//    rcsp_put_buf((void*)test_scene_red,sizeof(test_scene_red));
//
//    light_sence_table[LGT_SCENE_RED].tab_cnt = tmp16;
//    light_sence_table[LGT_SCENE_RED].tab_cycle = light_save_flash_cfg_test(light_sence_table[LGT_SCENE_RED].flash_struct,(void*)test_scene_red,sizeof(test_scene_red));
//    rcsp_db_puts("SCENE_RED--:\n");
//    rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_RED].tab_cnt,light_sence_table[LGT_SCENE_RED].tab_cycle);
//    rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_RED].flash_struct,sizeof(light_flash_t) * tmp16);
//
//
//
//    tmp16 = sizeof(test_scene_green)/sizeof(light_flash_param_t);
//    rcsp_put_buf((void*)test_scene_green,sizeof(test_scene_green));
//
//    light_sence_table[LGT_SCENE_GREEN].tab_cnt = tmp16;
//    light_sence_table[LGT_SCENE_GREEN].tab_cycle = light_save_flash_cfg_test(light_sence_table[LGT_SCENE_GREEN].flash_struct,(void*)test_scene_green,sizeof(test_scene_green));
//    rcsp_db_puts("SCENE_GREEN--:\n");
//    rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_GREEN].tab_cnt,light_sence_table[LGT_SCENE_GREEN].tab_cycle);
//    rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_GREEN].flash_struct,sizeof(light_flash_t) * tmp16);
//
//
//    tmp16 = sizeof(test_scene_blue)/sizeof(light_flash_param_t);
//    rcsp_put_buf((void*)test_scene_blue,sizeof(test_scene_blue));
//
//    light_sence_table[LGT_SCENE_BLUE].tab_cnt = tmp16;
//    light_sence_table[LGT_SCENE_BLUE].tab_cycle = light_save_flash_cfg_test(light_sence_table[LGT_SCENE_BLUE].flash_struct,(void*)test_scene_blue,sizeof(test_scene_blue));
//    rcsp_db_puts("SCENE_BLUE--:\n");
//    rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_BLUE].tab_cnt,light_sence_table[LGT_SCENE_BLUE].tab_cycle);
//    rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_BLUE].flash_struct,sizeof(light_flash_t) * tmp16);
//
//
//    tmp16 = sizeof(test_scene_freq)/sizeof(light_flash_param_t);
//    rcsp_put_buf((void*)test_scene_freq,sizeof(test_scene_freq));
//
//    light_sence_table[LGT_SCENE_FREQ].tab_cnt = tmp16;
//    light_sence_table[LGT_SCENE_FREQ].tab_cycle = light_save_flash_cfg_test(light_sence_table[LGT_SCENE_FREQ].flash_struct,(void*)test_scene_freq,sizeof(test_scene_freq));
//    rcsp_db_puts("SCENE_LBLK--:\n");
//    rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_FREQ].tab_cnt,light_sence_table[LGT_SCENE_FREQ].tab_cycle);
//    rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_FREQ].flash_struct,sizeof(light_flash_t) * tmp16);
//
//    return 0;
//
//#endif

    for(cnt = 0; cnt < len;)
    {
        id = get_frame_id_table((void*)&id_str[0]);
        tmp16 = id_str[7] + (id_str[6]<<8);

        code_type = id_str[4]>>4;
        id_str[4] = 0;

        rcsp_db_printf("id - <%s>,len= %d,type= %d\n",&id_str[0],tmp16,code_type);

        if(id >= AP_FRAME_ID_MAX)
        {
            rcsp_db_puts("ID_MAX\n");
            cnt += (tmp16 + 8);
            continue;
        }

        id_str += 8;
        cnt += (tmp16 + 8);

        switch(id)
        {
        case JL_FRAME_ID_LBRI:
            break;

        case JL_FRAME_ID_LWHI:
            break;

        case JL_FRAME_ID_LRED:
            light_sence_table[LGT_SCENE_RED].tab_cnt = tmp16/sizeof(light_flash_param_t);
            light_sence_table[LGT_SCENE_RED].tab_cycle = light_save_flash_cfg(light_sence_table[LGT_SCENE_RED].flash_struct,id_str,tmp16);
            rcsp_db_puts("SCENE_RED--:\n");
            rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_RED].tab_cnt,light_sence_table[LGT_SCENE_RED].tab_cycle);
            rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_RED].flash_struct,sizeof(light_flash_t) * (tmp16/sizeof(light_flash_param_t)));
            break;

        case JL_FRAME_ID_LGRR:
            light_sence_table[LGT_SCENE_GREEN].tab_cnt = tmp16/sizeof(light_flash_param_t);
            light_sence_table[LGT_SCENE_GREEN].tab_cycle = light_save_flash_cfg(light_sence_table[LGT_SCENE_GREEN].flash_struct,id_str,tmp16);
            rcsp_db_puts("SCENE_GREEN--:\n");
            rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_GREEN].tab_cnt,light_sence_table[LGT_SCENE_GREEN].tab_cycle);
            rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_GREEN].flash_struct,sizeof(light_flash_t) * (tmp16/sizeof(light_flash_param_t)));
            break;

        case JL_FRAME_ID_LBLU:
            light_sence_table[LGT_SCENE_BLUE].tab_cnt = tmp16/sizeof(light_flash_param_t);
            light_sence_table[LGT_SCENE_BLUE].tab_cycle = light_save_flash_cfg(light_sence_table[LGT_SCENE_BLUE].flash_struct,id_str,tmp16);
            rcsp_db_puts("SCENE_BLUE--:\n");
            rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_BLUE].tab_cnt,light_sence_table[LGT_SCENE_BLUE].tab_cycle);
            rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_BLUE].flash_struct,sizeof(light_flash_t) * (tmp16/sizeof(light_flash_param_t)));
            break;

        case JL_FRAME_ID_LBLK:
            light_sence_table[LGT_SCENE_FREQ].tab_cnt = tmp16/sizeof(light_flash_param_t);
            light_sence_table[LGT_SCENE_FREQ].tab_cycle = light_save_flash_cfg(light_sence_table[LGT_SCENE_FREQ].flash_struct,id_str,tmp16);
            rcsp_db_puts("SCENE_LBLK--:\n");
            rcsp_db_printf("tab_cnt= %d, tab_cycle= %d\n",light_sence_table[LGT_SCENE_FREQ].tab_cnt,light_sence_table[LGT_SCENE_FREQ].tab_cycle);
            rcsp_put_buf((void*)&light_sence_table[LGT_SCENE_FREQ].flash_struct,sizeof(light_flash_t) * (tmp16/sizeof(light_flash_param_t)));
            break;

        default:
            rcsp_db_puts("undeal id\n");
            break;
        }
        id_str += tmp16;
    }

//    free(light_sence_table);

    return err;
}

u32 cbk_lightTask_frame_deal(u8 *data,u16 len)
{
    u8  id;
    u16 cnt,tmp16,set_mode = 0;
    u32 err = RCSP_CBK_ERR_NONE;
    task_frame_parm_t *report_frame_p;
    u16 frame_count = len/sizeof(task_frame_parm_t);

    P_FUNCTION

    rcsp_db_printf("frame_count =%d\n",frame_count);

    rcsp_put_buf(data,len);

    for(cnt = 0; cnt < frame_count; cnt++)
    {
        report_frame_p = (void*)(data + sizeof(task_frame_parm_t)*cnt);
///        rcsp_db_printf("p =%x\n",(u32)report_frame_p);
        report_frame_p->reserved[0] = 0;
        id = get_frame_id_table((void*)report_frame_p->id_str);
        rcsp_db_printf("id - <%s>,len= %d,type= %d\n",report_frame_p->id_str,report_frame_p->value_len,report_frame_p->code_type);

        if(id >= AP_FRAME_ID_MAX)
        {
            rcsp_db_puts("id_max\n");
            continue;
        }

        switch(id)
        {
        case JL_FRAME_ID_LMOD: //0
            tmp16 = (report_frame_p->value_buf[0] << 8) + report_frame_p->value_buf[1];
            if(tmp16 < LGT_MD_SCENE)
            {
                light_info_tab.mode = (u8)tmp16;
                if(tmp16 > 0)
                {
                    light_info_tab.button = 1;
                }
                else
                {
                    light_info_tab.button = 0;
                }
            }
            else
            {
                err = RCSP_CBK_ERR_OPT;
            }
            break;

        case JL_FRAME_ID_LBRI:
            light_info_tab.color_value[LGT_COLOR_BRIGHT] = report_frame_p->value_buf[0];
            set_mode = 1;
            break;

        case JL_FRAME_ID_LEFF:
            light_info_tab.m_value[0] = report_frame_p->value_buf[0]&0xCF;
            break;

        case JL_FRAME_ID_LWHI:
            light_info_tab.color_value[LGT_COLOR_WHITE] = report_frame_p->value_buf[0];
            break;

        case JL_FRAME_ID_LRED:
            light_info_tab.color_value[LGT_COLOR_RED] = report_frame_p->value_buf[0];
            break;

        case JL_FRAME_ID_LGRR:
            light_info_tab.color_value[LGT_COLOR_GREEN] = report_frame_p->value_buf[0];
            break;

        case JL_FRAME_ID_LBLU:
            light_info_tab.color_value[LGT_COLOR_BLUE] = report_frame_p->value_buf[0];
            break;

        case JL_FRAME_ID_LSCE:
            if(report_frame_p->value_buf[0] < 6)
            {
                light_info_tab.mode = LGT_MD_SCENE;
                light_info_tab.m_value[0] = report_frame_p->value_buf[0]%6;

                light_sence_table = malloc(sizeof(light_disp_t) * LGT_SCENE_UNIT_MAX);
                CHECK_MALLOC(light_sence_table,1);

                err = light_scene_frame_deal(&report_frame_p->value_buf[1], report_frame_p->value_len -1);
                set_mode = 2;
            }
            else
            {
                err = RCSP_CBK_ERR_OPT;
            }
            goto deal_end;
            break;

        case JL_FRAME_ID_LBLK:
///            light_info_tab.bright = id_str[0];
            break;

        default:
            rcsp_db_puts("undeal id\n");
            break;
        }
    }

deal_end:
    if(err == RCSP_CBK_ERR_NONE)
    {
        rcsp_db_puts("set light mode success!!!\n");
        rcsp_put_buf((void*)&light_info_tab,sizeof(light_info_t));

        if(set_mode == 2)
        {
            light_control_set(light_sence_table,set_mode);
            free(light_sence_table);
            light_sence_table = NULL;
        }
        else
        {
            light_control_set(&light_info_tab,set_mode);
        }
    }
    return err;
}

//u32 cbk_lightTask_frame_deal(u8 *data,u16 len)
u32 cbk_task_frame_deal(u8 *data,u16 len)
{
    u8  id;
    u16 cnt,tmp16;
    u32 err = RCSP_CBK_ERR_NONE;
    task_frame_parm_t *report_frame_p;
    u16 frame_count = len/sizeof(task_frame_parm_t);

    P_FUNCTION

    rcsp_db_printf("frame_count =%d\n",frame_count);

    rcsp_put_buf(data,len);

    for(cnt = 0; cnt < frame_count; cnt++)
    {
        report_frame_p = (void*)(data + sizeof(task_frame_parm_t)*cnt);
        ///rcsp_db_printf("p =%x\n",(u32)report_frame_p);
        report_frame_p->reserved[0] = 0;
        id = get_frame_id_table((void*)report_frame_p->id_str);
        rcsp_db_printf("id - <%s>,len= %d,type= %d\n",report_frame_p->id_str,report_frame_p->value_len,report_frame_p->code_type);

        if(id >= AP_FRAME_ID_MAX)
        {
            rcsp_db_puts("id_max\n");
            continue;
        }

        switch(id)
        {
        case JL_FRAME_ID_LMOD: //0

        default:
            rcsp_db_puts("undeal id\n");
            break;
        }
    }
    return err;
}


u32 rcsp_bt_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len)
{
    int msg = MSG_NULL;
    int param_cnt = 0;
    int param[3];
    u8 byte_val;
    u32 err = RCSP_CBK_ERR_NONE;

    P_FUNCTION

    switch(pkg_type)
    {
    case RCSP_CBK_MSG_CHANGE_MODE:
        memcpy(param,data,4);
        return cbk_change_taskmode((void*)param[0]);
        break;

    case RCSP_CBK_MSG_COMM_PREV:
        msg = MSG_BT_PREV_FILE;
        break;

    case RCSP_CBK_MSG_COMM_NEXT:
        msg = MSG_BT_NEXT_FILE;
        break;

    case RCSP_CBK_MSG_COMM_PLAY:
        msg = MSG_BT_PP;
        param[param_cnt] = RCSP_CBK_MSG_COMM_PLAY;
        param_cnt++;
        break;

    case RCSP_CBK_MSG_COMM_PAUSE:
        msg = MSG_BT_PP;
        param[param_cnt] = RCSP_CBK_MSG_COMM_PAUSE;
        param_cnt++;
        break;

    case RCSP_CBK_MSG_COMM_VOL_UP:
        msg = MSG_VOL_UP;
        break;

    case RCSP_CBK_MSG_COMM_VOL_DOWN:
        msg = MSG_VOL_DOWN;
        break;

    case RCSP_CBK_MSG_COMM_SET_VOL:
        msg = MSG_APP_MAIN_MSGLIST;
        param[param_cnt++] = RCSP_CBK_MSG_COMM_SET_VOL;
        param[param_cnt++] = data[0];

		/* msg = MSG_VOL_UP; */
        /* param[param_cnt] = data[0]|0x80; */
        /* param_cnt++; */
        break;

    case RCSP_CBK_MSG_COMM_EQ:
        if(data[0] == 0)
        {
            //close EQ
			rcsp_eq_sw_flag = 0;
	        check_eq_sw_update();
            break;
        }
        byte_val = get_eq_from_bitmap(data[0]);
        if(byte_val < 6)
        {
            /* msg = MSG_BT_MUSIC_EQ; */
            rcsp_db_puts("sel eq\n");
            msg = MSG_APP_MSGLIST;
        	param[param_cnt++] = RCSP_CBK_MSG_COMM_EQ;
        	param[param_cnt++] = byte_val;
            rcsp_eq_sw_flag = 1;
	        check_eq_sw_update();
        }
        else
        {
            err = RCSP_CBK_ERR_OPT;
        }
        break;

    case RCSP_CBK_MSG_COMM_SET_EQ:
        byte_val = get_eq_from_bitmap(data[0]);
        if(byte_val < 6)
        {
            rcsp_db_puts("set eq\n");
            modify_eq_new_value(byte_val,(void*)&data[1]);
        }
        else
        {
            err = RCSP_CBK_ERR_OPT;
        }
        break;

    case RCSP_CBK_MSG_COMM_SAVE_EQ:
        rcsp_db_puts("save eq\n");
        save_eq_table();
        break;

    case RCSP_CBK_MSG_COMM_RESET_EQ:
        byte_val = get_eq_from_bitmap(data[0]);
        if(byte_val < 6)
        {
            rcsp_db_puts("reset eq\n");
            reset_eq_new_value(byte_val);
        }
        else
        {
            err = RCSP_CBK_ERR_OPT;
        }
        break;

    case RCSP_CBK_MSG_COMM_PLAYMODE:
        break;

    case RSCP_CBK_MSG_REPORT_FRAME:
        cbk_task_frame_deal(data,len);
        break;

    case RSCP_CBK_MSG_REQUEST_FRAME:
        err = cbk_request_frame_deal(data,len);
        break;

    //private msg
    default:
        break;
    }

    if(msg != MSG_NULL)
    {
        cbk_post_msg_to_task(RDEF_BTMSG_TASK_NAME,msg,param,param_cnt);
    }
    return err;
}



u32 rcsp_music_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len)
{
    int msg = MSG_NULL;
    int param_cnt = 0;
    int param[3];
    u8 byte_val;
    u32 err = RCSP_CBK_ERR_NONE;

    P_FUNCTION

    switch(pkg_type)
    {
	case RCSP_CBK_MSG_CHANGE_MODE:
        memcpy(param,data,4);
        return cbk_change_taskmode((void*)param[0]);
        break;

    case RCSP_CBK_MSG_COMM_PREV:
        msg = MSG_MUSIC_PREV_FILE;
        break;

    case RCSP_CBK_MSG_COMM_NEXT:
        msg = MSG_MUSIC_NEXT_FILE;
        break;

    case RCSP_CBK_MSG_COMM_PLAY:
        msg = MSG_MUSIC_PLAY;
        param_cnt++;
        break;

    case RCSP_CBK_MSG_COMM_PAUSE:
        msg = MSG_MUSIC_PAUSE;
        param_cnt++;
        break;

    case RCSP_CBK_MSG_COMM_VOL_UP:
        msg = MSG_VOL_UP;
        break;

    case RCSP_CBK_MSG_COMM_VOL_DOWN:
        msg = MSG_VOL_DOWN;
        break;

    case RCSP_CBK_MSG_COMM_SET_VOL:
		msg = MSG_APP_MAIN_MSGLIST;
        param[param_cnt++] = RCSP_CBK_MSG_COMM_SET_VOL;
        param[param_cnt++] = data[0];
        break;

    case RCSP_CBK_MSG_COMM_EQ:
        if(data[0] == 0)
        {
            //close EQ
            rcsp_eq_sw_flag = 0;
	        check_eq_sw_update();
            break;
        }
        byte_val = get_eq_from_bitmap(data[0]);
        if(byte_val < 6)
        {
            /* msg = MSG_MUSIC_EQ; */
            rcsp_db_puts("sel eq\n");
			msg = MSG_APP_MSGLIST;
        	param[param_cnt++] = RCSP_CBK_MSG_COMM_EQ;
        	param[param_cnt++] = byte_val;
            rcsp_eq_sw_flag = 1;
	        check_eq_sw_update();
        }
        else
        {
            err = RCSP_CBK_ERR_OPT;
        }
        break;

    case RCSP_CBK_MSG_COMM_SET_EQ:
        byte_val = get_eq_from_bitmap(data[0]);
        if(byte_val < 6)
        {
            rcsp_db_puts("set eq\n");
            modify_eq_new_value(byte_val,(void*)&data[1]);
        }
        else
        {
            err = RCSP_CBK_ERR_OPT;
        }
        break;

    case RCSP_CBK_MSG_COMM_SAVE_EQ:
        rcsp_db_puts("save eq\n");
        save_eq_table();
        break;

    case RCSP_CBK_MSG_COMM_RESET_EQ:
        byte_val = get_eq_from_bitmap(data[0]);
        if(byte_val < 6)
        {
            rcsp_db_puts("reset eq\n");
            reset_eq_new_value(byte_val);
        }
        else
        {
            err = RCSP_CBK_ERR_OPT;
        }
        break;

    case RCSP_CBK_MSG_COMM_PLAYMODE:
        byte_val = get_playmode_from_bitmap(data[0]);
        if(byte_val < 5)
        {
            msg = MSG_APP_MSGLIST;
            param[param_cnt++] = RCSP_CBK_MSG_COMM_PLAYMODE;
            param[param_cnt++] = byte_val+ REPEAT_ALL;
        }
        else
        {
            err = RCSP_CBK_ERR_OPT;
        }
        break;

    case RSCP_CBK_MSG_REPORT_FRAME:
        cbk_task_frame_deal(data,len);
        break;

    case RSCP_CBK_MSG_REQUEST_FRAME:
        err = cbk_request_frame_deal(data,len);
        break;

    case RCSP_CBK_MSG_MUSIC_FF:
        msg = MSG_MUSIC_FF;
	    param[param_cnt] = 12;
        param_cnt++;
		break;

    case RCSP_CBK_MSG_MUSIC_FR:
        msg = MSG_MUSIC_FR;
		param[param_cnt] = 12;
        param_cnt++;

		break;

    //private msg
    case RCSP_CBK_MSG_MUSIC_PREV_DEV:
        byte_val = (u8)file_operate_get_total_phydev();
        if(byte_val > 1)
        {
            msg = MSG_MUSIC_U_SD;
            param[param_cnt] = RCSP_CBK_MSG_MUSIC_PREV_DEV;
            param_cnt++;
        }
        else
        {
            rcsp_err_puts("no other devices!!!\n");
            err = RCSP_CBK_ERR_MASK_FLAG|RCSP_CBK_MUSIC_ERR_NO_DEV;
        }
        break;

    case RCSP_CBK_MSG_MUSIC_NEXT_DEV:
        byte_val = (u8)file_operate_get_total_phydev();
        if(byte_val > 1)
        {
            msg = MSG_MUSIC_U_SD;
            param[param_cnt] = RCSP_CBK_MSG_MUSIC_NEXT_DEV;
            param_cnt++;
        }
        else
        {
            rcsp_err_puts("no other devices!!!\n");
            err = RCSP_CBK_ERR_MASK_FLAG|RCSP_CBK_MUSIC_ERR_NO_DEV;
        }
        break;

    case RCSP_CBK_MSG_MUSIC_SET_DEV:
#if 0
        if(data[0] < 3)
        {
            rcsp_db_puts("cbk_SET_DEV\n");
            msg = MSG_MUSIC_U_SD;
            param[param_cnt] = RCSP_CBK_MSG_MUSIC_SET_DEV;
            param_cnt++;
            param[param_cnt] = data[0]+'B';///for debug
            param_cnt++;
        }
#endif
        break;

    case RCSP_CBK_MSG_MUSIC_SET_BYNO:
        msg = MSG_MUSIC_PLAY_SN;
        param[param_cnt] = data[0];
        param_cnt++;
        break;

    case RCSP_CBK_MSG_MUSIC_DEL_CUR_FILE:
        msg = MSG_MUSIC_DEL_FILE;
        break;

    case RCSP_CBK_MSG_MUSIC_BS_SET_EXT:
        break;

    case RCSP_CBK_MSG_MUSIC_BS_GET_INFO:
        break;

	case RCSP_CBK_MSG_UNKNOW_CMD:
    default:
        break;
    }

    if(msg != MSG_NULL)
    {
        cbk_post_msg_to_task(RDEF_MUSIC_TASK_NAME,msg,param,param_cnt);
    }

    return err;

}

bool check_radio_busy(void)
{
    if(fm_mode_var->scan_mode != FM_SCAN_STOP)
    {
        return TRUE;
    }
    return FALSE;
}

u32 rcsp_radio_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len)
{
    int msg = MSG_NULL;
    int param_cnt = 0;
    int param[3];
    u8 byte_val;
    u32 err = RCSP_CBK_ERR_NONE;

    P_FUNCTION

    switch(pkg_type)
    {
    case RCSP_CBK_MSG_CHANGE_MODE:
        memcpy(param,data,4);
        return cbk_change_taskmode((void*)param[0]);
        break;

    case RCSP_CBK_MSG_COMM_PREV:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_PREV_STATION;
        break;

    case RCSP_CBK_MSG_COMM_NEXT:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_NEXT_STATION;
        break;

    case RCSP_CBK_MSG_COMM_PLAY:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_PP;
        param[param_cnt] = RCSP_CBK_MSG_COMM_PLAY;
        param_cnt++;
        break;

    case RCSP_CBK_MSG_COMM_PAUSE:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_PP;
        param[param_cnt] = RCSP_CBK_MSG_COMM_PAUSE;
        param_cnt++;
        break;

    case RCSP_CBK_MSG_COMM_VOL_UP:
        msg = MSG_VOL_UP;
        break;

    case RCSP_CBK_MSG_COMM_VOL_DOWN:
        msg = MSG_VOL_DOWN;
        break;

    case RCSP_CBK_MSG_COMM_SET_VOL:
        msg = MSG_APP_MAIN_MSGLIST;
        param[param_cnt++] = RCSP_CBK_MSG_COMM_SET_VOL;
        param[param_cnt++] = data[0];
        break;

    case RCSP_CBK_MSG_COMM_EQ:
        if(data[0] == 0)
        {
            //close EQ
            rcsp_eq_sw_flag = 0;
	        check_eq_sw_update();
            break;
        }
        byte_val = get_eq_from_bitmap(data[0]);
        if(byte_val < 6)
        {
            /* msg = MSG_MUSIC_EQ; */
            rcsp_db_puts("sel eq\n");
            msg = MSG_APP_MSGLIST;
        	param[param_cnt++] = RCSP_CBK_MSG_COMM_EQ;
        	param[param_cnt++] = byte_val;
            rcsp_eq_sw_flag = 1;
	        check_eq_sw_update();
        }
        else
        {
            err = RCSP_CBK_ERR_OPT;
        }
        break;

    case RCSP_CBK_MSG_COMM_SET_EQ:
        byte_val = get_eq_from_bitmap(data[0]);
        if(byte_val < 6)
        {
            rcsp_db_puts("set eq\n");
            modify_eq_new_value(byte_val,(void*)&data[1]);
        }
        else
        {
            err = RCSP_CBK_ERR_OPT;
        }
        break;

    case RCSP_CBK_MSG_COMM_SAVE_EQ:
        rcsp_db_puts("save eq\n");
        save_eq_table();
        break;

    case RCSP_CBK_MSG_COMM_RESET_EQ:
        byte_val = get_eq_from_bitmap(data[0]);
        if(byte_val < 6)
        {
            rcsp_db_puts("reset eq\n");
            reset_eq_new_value(byte_val);
        }
        else
        {
            err = RCSP_CBK_ERR_OPT;
        }
        break;

    case RCSP_CBK_MSG_COMM_PLAYMODE:
        break;

    case RSCP_CBK_MSG_REPORT_FRAME:
        cbk_task_frame_deal(data,len);
        break;

    case RSCP_CBK_MSG_REQUEST_FRAME:
        err = cbk_request_frame_deal(data,len);
        break;

    //private msg
    case RCSP_CBK_MSG_RADIO_SEARCH_ALL:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }

        msg = MSG_FM_SCAN_ALL_INIT;
        break;

    case RCSP_CBK_MSG_RADIO_SEARCH_BACK:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_SCAN_ALL_DOWN;
        break;

    case RCSP_CBK_MSG_RADIO_SEARCH_FORWARD:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_SCAN_ALL_UP;
        break;

    case RCSP_CBK_MSG_RADIO_PREV_STEP:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_PREV_STEP;
        break;

    case RCSP_CBK_MSG_RADIO_NEXT_STEP:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_NEXT_STEP;
        break;

    case RCSP_CBK_MSG_RADIO_SAVE_FREQ:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_SAVE_CHANNEL;
        param[param_cnt] = data[1]+ (data[0]<<8);
        param_cnt++;
        break;

    case RCSP_CBK_MSG_RADIO_DEL_CHL:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_DEL_CHANNEL;
        param[param_cnt] = data[1]+ (data[0]<<8);
        param_cnt++;
        break;

    case RCSP_CBK_MSG_RADIO_SEL_CHL:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_SELECT_CHANNEL;
        param[param_cnt] = data[1]+ (data[0]<<8);
        param_cnt++;
        break;

    case RCSP_CBK_MSG_RADIO_SEL_FREQ:
        if(check_radio_busy())
        {
            err = RCSP_CBK_ERR_SYS_BUSY;
            break;
        }
        msg = MSG_FM_SELECT_FREQ;
        param[param_cnt] = data[1]+ (data[0]<<8);
        param_cnt++;
        break;

	case RCSP_CBK_MSG_UNKNOW_CMD:
    default:
        break;
    }

    if(msg != MSG_NULL)
    {
        cbk_post_msg_to_task(RDEF_RADIO_TASK_NAME,msg,param,param_cnt);
    }

    return err;

}



u32 rcsp_linein_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len)
{
    int msg = MSG_NULL;
    int param_cnt = 0;
    int param[3];
    u8 byte_val;
    u32 err = RCSP_CBK_ERR_NONE;

    P_FUNCTION

    switch(pkg_type)
    {
    case RCSP_CBK_MSG_CHANGE_MODE:
        memcpy(param,data,4);
        return cbk_change_taskmode((void*)param[0]);
        break;

    case RCSP_CBK_MSG_COMM_PREV:
        break;

    case RCSP_CBK_MSG_COMM_NEXT:
        break;

    case RCSP_CBK_MSG_COMM_PLAY:
        msg = MSG_AUX_MUTE;
        param[param_cnt] = RCSP_CBK_MSG_COMM_PLAY;
        param_cnt++;
        break;

    case RCSP_CBK_MSG_COMM_PAUSE:
        msg = MSG_AUX_MUTE;
        param[param_cnt] = RCSP_CBK_MSG_COMM_PAUSE;
        param_cnt++;
        break;

    case RCSP_CBK_MSG_COMM_VOL_UP:
        msg = MSG_VOL_UP;
        break;

    case RCSP_CBK_MSG_COMM_VOL_DOWN:
        msg = MSG_VOL_DOWN;
        break;

    case RCSP_CBK_MSG_COMM_SET_VOL:
        msg = MSG_APP_MAIN_MSGLIST;
        param[param_cnt++] = RCSP_CBK_MSG_COMM_SET_VOL;
        param[param_cnt++] = data[0];

		break;

    case RCSP_CBK_MSG_COMM_PLAYMODE:
        break;

    case RSCP_CBK_MSG_REPORT_FRAME:
        cbk_task_frame_deal(data,len);
        break;

    case RSCP_CBK_MSG_REQUEST_FRAME:
        err = cbk_request_frame_deal(data,len);
        break;

    //private msg
	case RCSP_CBK_MSG_UNKNOW_CMD:
    default:
        break;
    }

    if(msg != MSG_NULL)
    {
        cbk_post_msg_to_task(RDEF_LINEIN_TASK_NAME,msg,param,param_cnt);
    }

    return err;

}


u32 rcsp_udisk_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len)
{
    int msg = MSG_NULL;
    int param_cnt = 0;
    int param[3];
    u8 byte_val;
    u32 err = RCSP_CBK_ERR_NONE;

    P_FUNCTION

    switch(pkg_type)
    {
    case RCSP_CBK_MSG_CHANGE_MODE:
        memcpy(param,data,4);
        return cbk_change_taskmode((void*)param[0]);
        break;

    case RCSP_CBK_MSG_COMM_PREV:
        msg = MSG_PC_PLAY_PREV;
        break;

    case RCSP_CBK_MSG_COMM_NEXT:
        msg = MSG_PC_PLAY_NEXT;
        break;

    case RCSP_CBK_MSG_COMM_PLAY:
        msg = MSG_PC_PP;
        param[param_cnt] = RCSP_CBK_MSG_COMM_PLAY;
        param_cnt++;
        break;

    case RCSP_CBK_MSG_COMM_PAUSE:
        msg = MSG_PC_PP;
        param[param_cnt] = RCSP_CBK_MSG_COMM_PAUSE;
        param_cnt++;
        break;

    case RCSP_CBK_MSG_COMM_VOL_UP:
        msg = MSG_PC_VOL_UP;//MSG_VOL_UP;
        break;

    case RCSP_CBK_MSG_COMM_VOL_DOWN:
        msg = MSG_PC_VOL_DOWN;//MSG_VOL_DOWN;
        break;

    case RCSP_CBK_MSG_COMM_SET_VOL:
//        msg = MSG_VOL_UP;
//        param[param_cnt] = data[0];
//        param_cnt++;
        err = RCSP_CBK_ERR_MASK_FLAG|RCSP_RCSP_OPT_REJECT_ERR;
        break;


    case RCSP_CBK_MSG_COMM_PLAYMODE:
        break;

    case RSCP_CBK_MSG_REPORT_FRAME:
        cbk_task_frame_deal(data,len);
        break;

    case RSCP_CBK_MSG_REQUEST_FRAME:
        err = cbk_request_frame_deal(data,len);
        break;

    //private msg
	case RCSP_CBK_MSG_UNKNOW_CMD:
    default:
        break;
    }

    if(msg != MSG_NULL)
    {
        cbk_post_msg_to_task(RDEF_UDISK_TASK_NAME,msg,param,param_cnt);
    }

    return err;

}


u32 rcsp_light_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len)
{
    int msg = MSG_NULL;
    int param_cnt = 0;
    int param[3];
    u8 byte_val;
    u32 err = RCSP_CBK_ERR_NONE;

    P_FUNCTION

    switch(pkg_type)
    {
    case RCSP_CBK_MSG_CHANGE_MODE:
        memcpy(param,data,4);
        return cbk_change_taskmode((void*)param[0]);
        break;

    case RCSP_CBK_MSG_COMM_VOL_UP:
//        msg = MSG_VOL_UP;
        break;

    case RCSP_CBK_MSG_COMM_VOL_DOWN:
//        msg = MSG_VOL_DOWN;
        break;

    case RCSP_CBK_MSG_COMM_SET_VOL:
//        msg = MSG_VOL_UP;
//        param[param_cnt] = data[0]|0x80;
//        param_cnt++;
        break;

    case RSCP_CBK_MSG_REPORT_FRAME:
        cbk_lightTask_frame_deal(data,len);
//        cbk_task_frame_deal(data,len);
        break;

    case RSCP_CBK_MSG_REQUEST_FRAME:
        err = cbk_request_frame_deal(data,len);
        break;

    case RCSP_CBK_MSG_LIGHT_MODE:
        break;

    case RCSP_CBK_MSG_LIGHT_BRIGHTNESS:
        break;

    case RCSP_CBK_MSG_LIGHT_EFFECT:
        break;

    //private msg
	case RCSP_CBK_MSG_UNKNOW_CMD:
    default:
        break;
    }

    if(msg != MSG_NULL)
    {
        cbk_post_msg_to_task(RDEF_BTMSG_TASK_NAME,msg,param,param_cnt);
    }

    return err;
}


#endif //#if






