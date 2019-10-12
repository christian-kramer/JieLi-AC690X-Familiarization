/*******************************************************************************************
File Name: 	SD_Host_API.h

Version: 	1.00

Discription SD Host 接口函数

Author: 	Caibingquan

Email:  	change.tsai@gmail.com

Date:		2013.01.02

Copyright (c) 2010 - 2012 JieLi. All Rights Reserved.
*******************************************************************************************/
#ifndef _SD0_Host_API_H_
#define _SD0_Host_API_H_

#include "typedef.h"
#include "dev_manage/device_driver.h"

/**---------------------外部声明----------------------**/
/**---------------------------------------------------**/
extern const struct DEV_IO * dev_reg_sd0(void *parm);
extern const struct DEV_IO * dev_reg_sd1(void *parm);

extern const struct DEV_IO * dev_reg_sd0_mult(void *parm);
extern const struct DEV_IO * dev_reg_sd1_mult(void *parm);

//----------------------------------
// IO CONTROL CMD
//----------------------------------
enum
{
	SDMMC_GET_VAR_ADDR = 1,
	SDMMC_GET_CTRLER_STATUS,	//获取SD控制器繁忙状态
	SDMMC_GET_CTRLER_FLAG,
	SDMMC_GET_CMD_LOCK,			//设置CMD线是否锁定
	SDMMC_GET_BUSY,				//获取SD BUSY状态
	SDMMC_MEM_FREE,				//释放SD申请的变量空间
	SDMMC_CLOSE_CONTROLLER,		//suspend SD卡并关闭SD控制器(不再占用IO口)
	SDMMC_RELEASE_CONTROLLER,	//重新打开SD控制器和占据相应IO口
};

/*SD四线模式设置*/
typedef enum
{
    SD_4WIRE_MODE = 0,
    SD_1WIRE_MODE = BIT(0),
    SD_HI_SPEED = BIT(1),
}SD_NWRITE_SPEED_MODE;

/*SD控制器对应IO出口*/
typedef enum
{
    SD0_IO_A = 1, //sd0控制器A出口
    SD0_IO_B,     //sd0控制器B出口

    SD1_IO_A,     //sd1控制器A出口
    SD1_IO_B,     //sd1控制器B出口
}SD_CONTROLLER_IO;

/*SD在线检测方式*/
typedef enum
{
    SD_CMD_CHECK = 1, //SD卡命令检测
    SD_CLK_CHECK,     //SD卡CLK检测
    SD_IO_CHECK,      //SD卡IO检测
}SD_CHECK_WAY;

typedef struct _SD_API_SET
{
    u8  controller_io;     //<控制器IO出口
    u8  online_check_way;  //<sd卡在线检测方式：cmd和clk
    u8  max_data_baud;     //<数据传输最大波特率
    u8  hc_mode;           //<sd卡四线模式和高速模式设置
    u8  (*io_det_func)(void); //<io口检测函数
    u8  rw_way;               //读写函数选择：默认选择“等待读写完函数”
}sSD_API_SET __attribute__((aligned(4)));


/**---------------------enum类----------------------**/
/**---------------------------------------------------**/
/*sd调用失败列表*/
enum
{
    SD_SUCC = 0x0,
    SD_ERR_NOT_MOUNTED,             ///<设备未挂载
    SD_ERR_OVER_CAPACITY,           ///<读容量超出范围
    SD_ERR_UNKNOWN_CLASS,           ///<设备未知 Class
    SD_ERR_DEV_BUSY,                ///<控制器繁忙

    SD_ERR_READ = 0x10,             ///<读出错
    SD_ERR_READ_MULTIPLE_BLOCK,

    SD_ERR_WRITE = 0x20,            ///<写出错
    SD_ERR_WRITE_MULTIPLE_BLOCK,

    SD_ERR_TIMEOUT = 0x30,          ///<传输超时
    SD_ERR_TIMEOUT_COMMAND,         ///<命令回应超时
    SD_ERR_TIMEOUT_DATA,            ///<数据传输超时
    SD_ERR_TIMEOUT_READ,            ///<传输超时 读数据阶段
    SD_ERR_TIMEOUT_WRITE,           ///<传输超时 写数据阶段

    SD_ERR_OFFLINE = 0x40,          ///<设备不在线
    SD_ERR_OFFLINE_READ,
    SD_ERR_OFFLINE_WRITE,

    SD_ERR_CRC16 = 0x50,
    SD_ERR_CRC16_COMMAND,
    SD_ERR_CRC16_DATA,
    SD_ERR_CRC16_READ,
    SD_ERR_CRC16_WRITE,

    SD_STOP_ERR = 0x60,            ///<暂停
    SD_STOP_ERR_READ,
    SD_STOP_ERR_WRITE,
    SD_IDENTIFICATION_RETRY_ERR,   ///<发送无效 控制传输阶段
};

#endif/*_SD0_Host_API_H_*/
