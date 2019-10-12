#ifndef __ENCODE_H__
#define __ENCODE_H__

#include "cbuf/circular_buf.h"
#include "file_operate/file_op.h"
#include "dac/ladc_api.h"
#include "dev_manage/device_driver.h"
#include "mp2_encode_api.h"
#include "adpcm_encode_api.h"
#include "common/app_cfg.h"

typedef struct _EN_FILE_IO_
{
	void *priv;
	u16 (*input_data)(void *priv,s16 *buf,u8 channel);
	void (*output_data)(void *priv,u8 *buf,u16 len);
}EN_FILE_IO;

typedef  struct   _ENC_DATA_INFO_
{
	u16 sr;     ///<sample rate
	u16 br;     ///<mp2的时候它是bitrate。adpcm的时候，它是blockSize,一般配成256/512/1024/2048，超过2048会被限制成2048
	u32 nch;

} ENC_DATA_INFO;

typedef struct _ENC_OPS
{
	u32  (*need_buf)();
	void (*open)(u8 *ptr, EN_FILE_IO *audioIO);
	void (*set_info)(u8 *ptr, ENC_DATA_INFO *data_info);
	u32  (*init)(u8 *ptr);
	u32  (*run)(u8 *ptr);
	void (*write_head)(u8 *ptr);
}ENC_OPS;

typedef enum
{
    UNUSED_FORMAT = 0,      //标记

#if ENCODE_MP2_EN
    MP2_FORMAT,
#endif

#if ENCODE_ADPCM_EN
    ADPCM_FORMAT,
#endif

    MAX_FORMAT,
}ENC_FORMAT;

typedef enum
{
    ENC_STOP = 0,
    ENC_STAR,
    ENC_PAUS,
}ENC_STA;       ///录音状态

typedef struct _ADPCM_HEAD
{
    u8 head_buf[512];
    u16 flag;
}ADPCM_HEAD;

// typedef struct _ENC_OPS_t
// {
	// u32  (*need_buf)();
	// void (*open)(u8 *ptr, EN_FILE_IO *audioIO);
	// void (*set_info)(u8 *ptr, ENC_DATA_INFO *data_info);
	// u32  (*init)(u8 *ptr);
	// u32  (*run)(u8 *ptr,short *buf);
	// void (*write_head)(u8 *ptr);
// }ENC_OPS_t;

typedef struct _REC_LOST_FRAME
{
    u32 front_lost_frame;
    u32 black_lost_frame;
}REC_LOST_FRAME;

typedef struct _REC_FILE_INFO
{
    u32 file_size;
    u32 file_del_size;
    u32 enc_frame_cnt;
    u32 enc_time_cnt;
    u32 cut_head_ms;	//单位0.1ms
    u32 cut_tail_ms;	//单位0.1ms
}REC_FILE_INFO;

typedef struct _REC_CTL
{
    LADC_CTL ladc;
    cbuffer_t *enc_c_buf;
    u8 *enc_buf;
    u8 *buf_ptr;
    ENC_DATA_INFO *enc_info;
    EN_FILE_IO    *en_io;
    ENC_OPS   *enc_ops;
    // ENC_OPS_t *enc_ops_t;
    ADPCM_HEAD *file_head_info;
    char * rec_folder_name;
    volatile ENC_STA enable;
    volatile bool rec_busy;
    volatile bool wf_busy;
    REC_FILE_INFO file_info;
    REC_LOST_FRAME lost_frame;
    u16 enc_fram_size;///>录音帧大小，单位（s16），用于FM录音
    u8  enc_format;
    u8 curr_device;
}REC_CTL;

REC_CTL *g_rec_ctl;

typedef struct _ENC_API_IO
{
    void *f_p;
    u16 (*f_write)(void *fp , u8 *buff, u16 len);
    u16 (*f_read)(void *fp , u8 *buff, u16 len);
    bool (*f_seek)(void *fp , u8 type, u32 offsiz);
    s16 (*f_close)(void *fp);
    s16 (*f_delete)(void *fp);
}FW_API_IO;

typedef struct _RECORD_OP_API_
{
    FILE_OPERATE *fop_api;
    FW_API_IO *fw_api;
    REC_CTL   *rec_ctl;
}RECORD_OP_API;


REC_CTL *init_enc(RECORD_OP_API *rec_op_api, u8 ch_sel, u16 br, u16 sr, ENC_FORMAT format);
u32 enc_info_init(RECORD_OP_API *rec_op_api, u16 ch_cnt, u16 br, u16 sr);
void stop_rec(RECORD_OP_API *rec_op_api);
DEV_ERR write2file(RECORD_OP_API *rec_op_api);
void write_head_api(RECORD_OP_API *rec_op_api);
void enc_run_creat(void);
FW_API_IO *fw_api_init(void);
FILE_OPERATE *enc_fop_api_init(void);
void free_fop(RECORD_OP_API * rec_op_api);
u32 get_enc_time(RECORD_OP_API * rec_op_api);   //获取时间
u32 updata_enc_time(RECORD_OP_API * rec_op_api);//建议半秒更新
void rec_msg_deal(RECORD_OP_API ** rec_api_p, int * msg);
void exit_rec(RECORD_OP_API **rec_api);
u32 reg2sr(u16 sr_reg);
void exit_rec_deal(void *p);

#if REC_EN
    #define exit_rec_api(x)         	exit_rec_deal(x)
    #define rec_msg_deal_api(x,y)    	rec_msg_deal(x,y)
#else
    #define exit_rec_api(...)
    #define rec_msg_deal_api(...)
#endif
#endif/*__ENCODE_H__*/
