#ifndef reverb_api_h__
#define reverb_api_h__

#define ECHO_PARM   0   ///0：回声，1:混响(暂不支持)

#define   REVERB_KIND0       0
#define   REVERB_KIND1       1

typedef struct _reverb_audio_info_
{
	unsigned short sr ;            ///< bit rate
	unsigned short nch ;           ///<声道
}reverb_audio_info ;


typedef struct _REVERB_IO_
{
	void *priv;
	void (*output)(void *priv,short *outdata,short len);               //len是多少个byte
}REVERB_IO;


typedef struct _EF_REVERB_PARM_
{
	unsigned char predelay;             //0-128
	unsigned char roomscale;            //0-128
	unsigned short  damping;            //0-7000
	unsigned short  reverberance;       //0-8192
	unsigned short  wet_gain;            //0-256
	unsigned short  dry_gain;            //0-256
}EF_REVERB_PARM;

typedef struct _Low_pass_para_
{
	int A0;
	int A1;
	int A2;
	int B1;
	int B2;
}Low_pass_para;

typedef struct _EF_REVERB_PARM2_
{
	unsigned short deepval;
	unsigned short decayval;
	unsigned short gainval;        //音量增益
	unsigned short rs_mode;
	unsigned short filter_mode;
	Low_pass_para  lp_para;
}EF_REVERB_PARM2;


typedef struct _REVERB_PARM_SET_
{
	unsigned char reverb_kind;          //混响类型
	unsigned char qualityn;             //不同模式申请buf的时候，预防buf不够留的接口，先忽略
	unsigned short max_sr;
	unsigned short max_predelay;
	unsigned char *bufcal;
	EF_REVERB_PARM ef_reverb_parm;
	EF_REVERB_PARM2 ef_reverb_parm_2;
	unsigned short sr_out;
	unsigned char rs_enable;
	unsigned char shift_enable;          //是否要移频

}REVERB_PARM_SET;


#define  REVERB_H_PARM_STRUCT REVERB_PARM_SET

/*open 跟 run 都是 成功 返回 RET_OK，错误返回 RET_ERR*/
/*魔音结构体*/
typedef struct __REVERB_FUNC_API_ {
	unsigned int  (*need_buf)();
	unsigned int  (*need_buf2)(unsigned char *ptr,REVERB_PARM_SET *reverb_parm);
	int   (*open)(unsigned char *ptr,REVERB_PARM_SET *reverb_parm,reverb_audio_info *audioinfo,REVERB_IO *rv_io);
	int  (*init)(unsigned char *ptr,REVERB_PARM_SET *reverb_parm,reverb_audio_info *audioinfo);
	int  (*run)(unsigned char *ptr,short *inbuf ,short len);
	int  (*change_parm)(unsigned char *ptr,unsigned short sample_rate,unsigned short nch);
}REVERB_FUNC_API;

/*
typedef struct _REVERB_API_STRUCT_
{
	REVERB_PARM_SET *reverb_parm_obj;  //参数
	reverb_audio_info *audio_info;
	unsigned char *ptr;                   //运算buf指针
	REVERB_FUNC_API *func_api;            //函数指针
	REVERB_IO *func_io;                   //输出函数接口

}REVERB_API_STRUCT;
*/

extern REVERB_FUNC_API  *get_reverb_func_api();

#endif // reverb_api_h__
