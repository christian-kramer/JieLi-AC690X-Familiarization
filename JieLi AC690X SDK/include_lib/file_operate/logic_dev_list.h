#ifndef __LOGIC_DEV_LIST_H__
#define __LOGIC_DEV_LIST_H__

#include "common/list.h"
#include "rtos/os_api.h"
enum
{
    DEV_TYPE_SPI =0,            ///<内部spi flash
    DEV_TYPE_MASS,              ///<外部大存储
};

typedef struct _lg_dev_hdl_
{
    void *fs_hdl;               ///<文件系统指针
    void *file_hdl;             ///<文件指针
    void *phydev_item;          ///<逻辑盘符对应物理设备节点
    void *ui_fhdl;             ///<文件指针
    void *lrc_fhdl;             ///<文件指针
}lg_dev_hdl;


typedef struct _lg_dev_info_
{
    OS_SEM del_sem;            ///<设备删除允许信号
    u32 total_file_num;          ///<逻辑设备中文件总数
    u32 last_op_file_num;        ///<上一次操作的文件编号
    lg_dev_hdl  *lg_hdl;         ///<逻辑设备的物理设备、文件系统、文件指针
    u8 fat_type;				 ///<文件系统类型
    u8 dev_let;				     ///<盘符
    u8 snum_phydev; 		     ///<逻辑盘在物理设备中序号
    u8 dev_type;				 ///<设备类型
    u8 allfileerr;              ///<标识该设备所有文件都不可解码
}lg_dev_info;


typedef struct _lg_dev_list_
{
    lg_dev_info cur_dev;
    struct list_head list;
}lg_dev_list;

#endif
