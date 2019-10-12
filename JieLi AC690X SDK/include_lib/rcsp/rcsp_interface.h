/*
*********************************************************************************************************
*                                                AC46
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

#ifndef __RCSP_INTERFACE_H__
#define __RCSP_INTERFACE_H__

#include "typedef.h"
#include "common/msg.h"
#include "uart.h"
#include "file_operate/file_op.h"
// #include "sdk_cfg.h"


#define RDEF_MAIN_TASK_NAME        "MainTask"
#define RDEF_RTC_TASK_NAME         "RTCTask"
#define RDEF_MUSIC_TASK_NAME       "MusicTask"
#define RDEF_RECORD_TASK_NAME      "RECTask"
#define RDEF_RADIO_TASK_NAME       "FMRadioTask"
#define RDEF_LINEIN_TASK_NAME      "LineinTask"
#define RDEF_UDISK_TASK_NAME       "USBdevTask"
#define RDEF_BTMSG_TASK_NAME       "btmsg"
#define RDEF_BTSTACK_TASK_NAME     "BtStackTask"
#define RDEF_IDLE_TASK_NAME        "IdleTask"
#define RDEF_LIGHT_TASK_NAME       "LightTask"

#define RCSP_TASK_NAME 			   "RCSP_Task"

//frame code type
typedef enum
{
    RCSP_APP_TYPE_NULL = 0x00,
    RCSP_APP_TYPE_SPP,
    RCSP_APP_TYPE_BLE,
}app_cnn_type_e;


///#define RCSP_BUFF_IDLE_FLAG      (0xDDEE1234)

//线程任务操作状态

#define RCSP_CBK_ERR_MASK_FLAG   0x8000

typedef enum
{
    STAUS_NONE            = 0x00,
    STAUS_STOP            = 0x01,
    STAUS_PLAY            = 0x02,
    STAUS_PAUSE           = 0x03,

    //Special status
    STAUS_SETTING_RTC     = 0x04,
    STAUS_SEARCHING       = 0x05,
    STAUS_CHANGE_DEVIECE  = 0x06,
    STAUS_PREPARE         = 0x07,
    STAUS_SAVE            = 0x08,
    STAUS_START_FM_RECORD = 0x09,
    STAUS_START_AUX_RECORD= 0x0a,
    STAUS_START_MIC_RECORD= 0x0b,
    STAUS_ALARMING        = 0x0c,
    STAUS_CLOSE_ALARM     = 0x0d,
    STAUS_DEL_OK          = 0x0e,
    STAUS_START_AUX_REC   = 0x0f,
    STAUS_NO_DEVICE       = 0x10,

    //蓝牙新增加
    STAUS_NOT_CONNECT     = 0x11,
    STAUS_PHONEING        = 0x12,

    STAUS_NODEAL          = 0xFF,
} cur_task_st_e;


//frame code type
typedef enum
{
    FRAME_TYPE_UTF16 = 0x00,
    FRAME_TYPE_ANSI,
    FRAME_TYPE_GBK,
}frame_code_type_e;

typedef enum
{
    RCSP_CBK_MSG_COMM_NULL = 0x00,
    RCSP_CBK_MSG_COMM_PREV,
    RCSP_CBK_MSG_COMM_NEXT,
    RCSP_CBK_MSG_COMM_PLAY,
    RCSP_CBK_MSG_COMM_PAUSE,
    RCSP_CBK_MSG_COMM_VOL_UP,
    RCSP_CBK_MSG_COMM_VOL_DOWN,
    RCSP_CBK_MSG_COMM_SET_VOL,
    RCSP_CBK_MSG_COMM_EQ,
    RCSP_CBK_MSG_COMM_SET_EQ,
    RCSP_CBK_MSG_COMM_SAVE_EQ,
    RCSP_CBK_MSG_COMM_RESET_EQ,
    RCSP_CBK_MSG_COMM_PLAYMODE,
    RCSP_CBK_MSG_CHANGE_MODE,
    RCSP_CBK_MSG_UNKNOW_CMD,

    //FRAME
    RSCP_CBK_MSG_REPORT_FRAME = 0x10,    //
    RSCP_CBK_MSG_REQUEST_FRAME,

    //BT private msg
    RCSP_CBK_MSG_BT_RES = 0x20,

    //MUSIC private msg
    RCSP_CBK_MSG_MUSIC_PREV_DEV = 0x30,
    RCSP_CBK_MSG_MUSIC_NEXT_DEV,
    RCSP_CBK_MSG_MUSIC_FF,
    RCSP_CBK_MSG_MUSIC_FR,
    RCSP_CBK_MSG_MUSIC_SET_DEV,
    RCSP_CBK_MSG_MUSIC_SET_BYNO,
    RCSP_CBK_MSG_MUSIC_DEL_CUR_FILE,

    RCSP_CBK_MSG_MUSIC_BS_SET_EXT,
    RCSP_CBK_MSG_MUSIC_BS_GET_INFO,

    //RADIO private msg
    RCSP_CBK_MSG_RADIO_SEARCH_ALL = 0x40,
    RCSP_CBK_MSG_RADIO_SEARCH_BACK,
    RCSP_CBK_MSG_RADIO_SEARCH_FORWARD,

    RCSP_CBK_MSG_RADIO_PREV_STEP,
    RCSP_CBK_MSG_RADIO_NEXT_STEP,
    RCSP_CBK_MSG_RADIO_SAVE_FREQ,
    RCSP_CBK_MSG_RADIO_DEL_CHL,
    RCSP_CBK_MSG_RADIO_SEL_CHL,
    RCSP_CBK_MSG_RADIO_SEL_FREQ,

    //LINEIN private msg
    RCSP_CBK_MSG_LINEIN_RES = 0x50,

    //USOUND private msg
    RCSP_CBK_MSG_USOUND_RES = 0x60,

    //USOUND private msg
    RCSP_CBK_MSG_LIGHT_MODE = 0x70,
    RCSP_CBK_MSG_LIGHT_BRIGHTNESS,
    RCSP_CBK_MSG_LIGHT_EFFECT,

    //NO msg
    RCSP_CBK_NO_MSG = 0xFF,

}rcsp_cbk_msg_e;

typedef enum
{
    RCSP_OP_ERR_NONE =0,
    RCSP_OP_ERR_NO_APP,
    RCSP_OP_ERR_MALLOC,
    RCSP_OP_ERR_PARAM,
    RCSP_OP_ERR_WORDMODE,

    RCSP_OP_ERR_REC_DATA = 0x10,
    RCSP_OP_ERR_WAIT_CSW,
    RCSP_OP_ERR_SEND_SPPDATA,
    RCSP_OP_ERR_NO_CMD,
    RCSP_OP_ERR_CSW_CHECK,
    RCSP_OP_ERR_CSW_CSWTAG,
    RCSP_OP_ERR_SEM_PEND,

    RCSP_OP_ERR_DIR_PATH,
    RCSP_OP_ERR_FS_OPT,
    RCSP_OP_ERR_DIR_DEPTH,

}rcsp_op_err_e;

///CBW 操作结果状态
typedef enum
{
    CSW_SUCC = 0x00,
    CSW_ERR_OPT,
    CSW_ERR_RESS,
    CSW_ERR_CRC,
    CSW_ERR_TASK_MODE,
    CSW_ERR_SYS_BUSY,
    CSW_ERR_NO_ACK,
    CSW_ERR_MAX,

        //--------------------------
    CSW_BT_ERR_RES    = 0x10,

    CSW_MUSIC_ERR_NO_DEV = 0x20,
    CSW_MUSIC_ERR_NO_APIC = 0x21,
    CSW_MUSIC_ERR_READ_DEV = 0x22,
    CSW_MUSIC_ERR_FS_BS = 0x23,

}cbw_opt_status_e;

typedef enum
{
    RCSP_EVENT_CBK_CMD_CHECK = 0,
    RCSP_EVENT_CBK_CMD_SEND,
}app_send_event_e;


//CBW命令
#define CBW_CMD_GET_VERSION             0x80
#define CBW_CMD_VOL_UP                  0x81
#define CBW_CMD_VOL_DOWN                0x82
#define CBW_CMD_VOL_SET                 0x83
#define CBW_CMD_CHANGE_MODE             0x84
#define CBW_CMD_GET_MODE_INFO           0x85
#define CBW_CMD_VERSION_ERR             0x86
#define CBW_CMD_SET_TIMEOUT             0x87
#define CBW_CMD_SET_EQ                  0x88
#define CBW_CMD_LIGHT_ST                0x89
#define CBW_CMD_BT_UPDATA               0x8a
#define CBW_CMD_CHECK_CYCLE             0x90
#define CBW_CMD_SET_CHECK_TIME          0x91

#define CBW_CMD_TASK_SECTION            0x01

#define FW_TYPECMD_SEND_STATUS       0
#define FW_TYPECMD_GET_STATUS        1


//rcsp_report_status_flag
#define RCSP_REPORT_STATUS_CONTROL       (1<<0)
#define RCSP_REPORT_VOL_VALUE            (1<<1)
#define RCSP_REPORT_EQ_VALUE             (1<<2)
// #define RCSP_REPORT_START_STATE          (1<<3)
#define RCSP_REPORT_START_CONFIG         (1<<4)
#define RCSP_REPORT_EQ_CONFIG            (1<<5)

#define RCSP_REPORT_PLAYMODE_VALUE       (1<<8)
#define RCSP_REPORT_MUSIC_FILEINFO       (1<<9)
#define RCSP_REPORT_RADIO_TABLE          (1<<10)
#define RCSP_REPORT_RADIO_CHANNEL        (1<<11)
#define RCSP_REPORT_MUSIC_PLAYTIME       (1<<12)
#define RCSP_REPORT_LIGHT_INFO           (1<<13)
#define RCSP_REPORT_ID3_INFO             (1<<14)
#define RCSP_REPORT_LIGHT_STATE          (1<<15)
#define RCSP_REPORT_WORKMODE_ACTION      (1<<16)

typedef struct
{
    u8 id_str[4];
    u8 reserved[1];
    u8 code_type;
    u16 value_len;
    u8 *value_buf;
}task_frame_parm_t;


typedef u32 (*comsend_cbk_t)(u8 *data,u16 len);
typedef u32 (*task_subcmd_cbk_t)(u8 *data,u16 len);
typedef u32 (*rcsp_apptask_cbk_t)(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len);

// #define P_FUNCTION    printf("@-%s \n",__FUNCTION__);
#define P_FUNCTION    

#define CHECK_MALLOC(a,b)   \
		do { \
			if((u32)(a) == 0){ \
				printf("MALLOC-FAILD, func= %s, LINE = %d\n",__FUNCTION__,__LINE__); \
				while(b); \
			} \
		}while(0)



#if SUPPORT_APP_RCSP_EN
#define Post_msg_to_rcsp_Task        Post_msg_to_rcsp_Task_do
#define Update_rcsp_Task_State       Update_rcsp_Task_State_do
#define rcsp_app_start               rcsp_apptask_start_deal_do
#define rcsp_report_status(x)        do{rcsp_com_report_flag |= x;}while(0)
#define rcsp_set_task_status         rcsp_set_task_status_do
#define rcsp_device_change           rcsp_device_change_do
#define rcsp_update_cur_device       rcsp_update_cur_device_do
#define rcsp_set_music_bs_op_api     rcsp_set_music_bs_op_api_do
#else
#define Post_msg_to_rcsp_Task(...)
#define Update_rcsp_Task_State(...)
#define rcsp_app_start(...)
#define rcsp_report_status(...)
#define rcsp_set_task_status(...)
#define rcsp_device_change(...)
#define rcsp_update_cur_device(...)
#define rcsp_set_music_bs_op_api(...)
#endif

///#define rcsp_set_event_buf_idle(buf_addr)  *(u32*)(buf_addr) = RCSP_BUFF_IDLE_FLAG


extern void rcsp_set_task_status_do(u8 status);
extern u32 rcsp_com_report_flag;
extern u8  rcsp_cur_task_status;
extern u8  rcsp_music_curr_device;

extern void rcsp_event_com_start(u8 type);
extern void rcsp_event_com_stop(void);
extern tbool rcsp_com_start(u8 type);
extern tbool rcsp_com_stop(void);
extern tbool rcsp_register_comsend(comsend_cbk_t cbk);
extern void  rcsp_comdata_recieve(u8 *data, u16 len);
// extern void rcsp_register_support_app(u8 app_cnt, u8 *app_name_group);
extern void rcsp_set_music_bs_op_api_do(void *m_op_api);


extern void rcsp_sppdata_recieve(u8 type , u16 channel_id , u8 *data, u16 len);
extern u32 bt_spp_send_data(u8 *data,u16 len);
extern void bt_spp_disconnect(void);

extern u8 Post_msg_to_rcsp_Task_do(u32 msg, u32 param1, u32 param2);
extern u8 Update_rcsp_Task_State_do(int msg,u8 reserved);


extern u32 rcsp_main_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len);
extern u32 rcsp_bt_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len);
extern u32 rcsp_music_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len);
extern u32 rcsp_radio_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len);
extern u32 rcsp_linein_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len);
extern u32 rcsp_udisk_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len);
extern u32 rcsp_light_callback(rcsp_cbk_msg_e pkg_type,u8 *data,u16 len);

extern void rcsp_apptask_start_deal_do(const char *name);
extern u8 rcsp_check_app_connection(void);
extern void rcsp_register_msg_cbk(void *cbk);

extern void rcsp_update_cur_device_do(u8 dev_id);
extern void rcsp_device_change_do(u8 *phydev_name);

extern void post_playfile_id3_info(void *hdl);

extern u32 rcsp_cbk_answer_apic_send(void);

extern void rcsp_main_task_msg_deal_before(int *msg);
extern void rcsp_main_task_msg_deal_after(int *msg);
extern void rcsp_bt_task_msg_deal_before(int *msg);
extern void rcsp_bt_task_msg_deal_after(int *msg);
extern void rcsp_music_task_msg_deal_before(int *msg);
extern void rcsp_music_task_msg_deal_after(int *msg);
extern void rcsp_fm_task_msg_deal_before(int *msg);
extern void rcsp_fm_task_msg_deal_after(int *msg);
extern void rcsp_linein_task_msg_deal_before(int *msg);
extern void rcsp_linein_task_msg_deal_after(int *msg);
extern void rcsp_pc_task_msg_deal_before(int *msg);
extern void rcsp_pc_task_msg_deal_after(int *msg);
extern void rcsp_light_task_msg_deal_before(int *msg);
extern void rcsp_light_task_msg_deal_after(int *msg);
extern void rcsp_fm_scan_channel_deal(int *msg);
extern void rcsp_music_play_api_before(ENUM_DEV_SEL_MODE dev_sel,u32 dev_let);
extern void rcsp_music_play_api_after(ENUM_DEV_SEL_MODE dev_sel,u32 dev_let,u32 reslut);

extern u32 rcsp_fs_set_ext_analysis(u8 *data,u16 len);
extern u32 fw_report_support_app_info(char *app_mode_string,u8 support_appid_cnt);
extern void rcsp_set_cmd_callback(void *public_cmd_cbk,void *submode_cmd_cbk,void *get_cmd_send);
extern u32 fw_report_ctl_state_send(u8 *OperationData);
extern u32 fw_report_light_state_send(u32 light_sw_state);
extern void rcsp_cmd_register_callback(void);
extern void rcsp_set_light_appid(u8 light_appid);
extern u16 cal_crc16_deal(u8 *ptr, u16 len);
extern void rcsp_set_fs_file_ext(u8 *ext_name);

#endif /* #ifndef __RCSP_INTERFACE_H__ */



