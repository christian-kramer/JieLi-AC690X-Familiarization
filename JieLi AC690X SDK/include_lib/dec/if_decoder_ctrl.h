
/*******************************************************************************************

Copyright (C), 2014 -2014, JIELI Tech. Co., Ltd.
File name:if_decoder_ctrl.h
Author:    Version:    Date: // 作者、版本及完成日期
邓玉林  1.0             2014-06-19   09:02:00上午
Description:

Others:    // 其它内容的说明
Function List:


1. ....
History:


1. Date: Author: Modification:
2. ...

*************************************************/
#ifndef __if_decoder_ctrl_h
#define __if_decoder_ctrl_h






#include "typedef.h"
//#define DECODE_NORMAL  0x00
//#define DECODE_FF      0x01
//#define DECODE_FR      0x02
//#define DECODE_STOP    0x03

#define PLAY_MOD_NORMAL   0x00
#define PLAY_MOD_FF   0x01
#define PLAY_MOD_FB   0x02


//play control

//PLAY_NEXT
#define AUDIO_BK_EN

struct if_decoder_io{
    void *priv ;
	int (*input)(void *priv ,u32 addr , void *buf, int len,u8 type);
	/*
	priv -- 私有结构体，由初始化函数提供。
	addr -- 文件位置
	buf  -- 读入地址
	len  -- 读入长度 512 的整数倍
	type -- 0 --同步读（等到数据读回来，函数才返回） ，1 -- 异步读（不等数据读回来，函数就放回）

	*/
	int (*check_buf)(void *priv ,u32 addr , void *buf);
	void (*output)(void *priv  ,void *data, u32 len);
	u32 (*get_lslen)(void *priv);
	u32 (*store_rev_data)(void *priv,u32 addr ,int len);
};

typedef struct if_decoder_io IF_DECODER_IO;
typedef struct decoder_inf
{
	u16 sr ;            ///< sample rate
	u16 br ;            ///< bit rate
	u32 nch ;           ///<声道
	u32 total_time;     ///<总时间
}dec_inf_t ;


typedef struct __audio_decoder_ops {
	char *name ;                                                            ///< 解码器名称
	u32 (*open)(void *work_buf, const struct if_decoder_io *decoder_io, u8 *bk_point_ptr);  ///<打开解码器

	u32 (*format_check)(void *work_buf);					///<格式检查

	u32 (*run)(void *work_buf, u32 type);					///<主循环

	dec_inf_t* (*get_dec_inf)(void *work_buf) ;				///<获取解码信息
	u32 (*get_playtime)(void *work_buf) ;					///<获取播放时间
	u32 (*get_bp_inf)(void *work_buf);						///<获取断点信息;返回断点信息存放的地址

	//u32 (*need_workbuf_size)() ;							///<获取整个解码所需的buffer
	u32 (*need_dcbuf_size)() ;		                        ///<获取解码需要的buffer
	u32 (*need_rdbuf_size)();                               ///<获取解码读数buf的读文件缓存buf的大小
	u32 (*need_bpbuf_size)() ;				                ///<获取保存断点信息需要的buffer的长度

	//void (*set_dcbuf)(void* ptr);			                ///<设置解码buffer
	//void (*set_bpbuf)(void *work_buf,void* ptr);			///<设置断点保存buffer

	void (*set_step)(void *work_buf,u32 step);				///<设置快进快进步长。
	void (*set_err_info)(void *work_buf,u32 cmd,u8 *ptr,u32 size);		///<设置解码的错误条件
	u32 (*dec_confing)(void *work_buf,u32 cmd,void*parm);
}audio_decoder_ops,decoder_ops_t;




#endif


