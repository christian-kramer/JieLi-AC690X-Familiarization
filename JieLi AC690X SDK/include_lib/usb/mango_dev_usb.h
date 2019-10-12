#ifndef _MANGO_DEV_USB_H_
#define _MANGO_DEV_USB_H_

#include "typedef.h"
#include "dev_manage/device_driver.h"

/**---------------------enum类----------------------**/
/**---------------------------------------------------**/
/*枚举失败列表*/
enum
{
    USB_SUCC = 0x0,
    USB_ERR_NOT_MOUNTED,            ///<设备未枚举
    USB_ERR_OVER_CAPACITY,          ///<读容量超出范围
    USB_ERR_UNKNOW_CLASS,                ///<设备非MassStorage类
    USB_ERR_UNIT_NOT_READY,         ///<设别逻辑单元未准备
    USB_ERR_LUN,
    USB_ERR_BULK_RPT30,
    USB_ERR_BULK_RECEIVE,

    USB_ERR_READ = 0x10,            ///<读出错

    USB_ERR_WRITE = 0x20,           ///<写出错
    USB_ERR_WRITE_SECTOR_SIZE,


    USB_ERR_TIMEOUT = 0x30,         ///<传输超时
    USB_ERR_TIMEOUT_CONTROL,        ///<传输超时 控制传输阶段
    USB_ERR_TIMEOUT_BULK_SEND,      ///<传输超时 批量传输阶段
    USB_ERR_TIMEOUT_BULK_RECEIVE,   ///<传输超时 批量传输阶段

    USB_ERR_OFFLINE = 0x40,         ///<设备不在线
    USB_ERR_OFFLINE_CONTROL,
    USB_ERR_OFFLINE_BULK_SEND,
    USB_ERR_OFFLINE_BULK_RECEIVE,

    USB_ERR_STALL = 0x50,           ///<传输无效
    USB_ERR_STALL_CONTROL,          ///<传输无效 控制传输阶段
    USB_ERR_STALL_BULK_SEND,        ///<传输无效 批量传输阶段
    USB_ERR_STALL_BULK_RECEIVE,

    USB_ERR3 = 0x60,                ///<发送无效
    USB_ERR3_CONTROL,               ///<发送无效 控制传输阶段
};


/*****************************
        Typedef
*****************************/
typedef struct _USB_DETECT
{
    volatile u8  bDevOnline;    //<设备在线标志	[0]:USB Host [1]:USB Slave
    u8  bOnlineCnt;             //<USB 在线计数器
    u8  bLastData;              //<USB 控制器已检测的值
    u8  bCurData;               //<USB 控制器当前检测的值
    u32 last_sta;               //<USB 上一次在线状态
}sUSB_DETECT;

typedef struct _USB_HOST_VAR
{
    /*-----设备公共属性-------*/
    u32 dwDevCapacity;          //<设备容量
    u32 dwDevCurLBA;            //<当前设备操作逻辑地址
    u16 wDevTimeOutMax;		    //<设备操作超时上限
    volatile u16 wDevTimeOut;   //<设备操作超时计数
    u8  bDevOperation;          //<设备操作状态
    u8  bDevStatus;			    //<设备状态
    volatile u8 bDevBusy;	    //<设备繁忙标记
    u8  bDevError;              //<设备操作错误

    /*-----设备特有属性-------*/
    u8  bDevSectorPerLBA;       //<设备逻辑地址Sector大小 Logical block sector size (SectorPerLBA * 512)
    u8  bDevCurOffset;          //<设备操作逻辑地址偏移 Logical Block Sector offset
    u8  bDevTransFail;          //<Bulk-Only 传输出错标志
    u8  bClass;                 //<[0]:MSD [1]:Audio [2]:HID
}sUSB_HOST_VAR;

typedef struct _USB_MSD_CLASS_INFO
{
    u16 wMaxPacketSize;
    u8  bInterfaceNumber;
    u8  bDevAddr;		        //<设备地址
    u8  bDevEpIn;               //<Bulk Only 传输输入端
    u8  bDevEpOut;              //<Bulk Only 传输输出端
}sUSB_MSD_CLASS_INFO;

typedef struct _USB_CONTROL_TRANSFER
{
    u8 bTokens;		    //<令牌包类型
    u8 bDatalen;	    //<数据阶段所需传输的字节数
    u8 bMaxPktSize;     //<设备接收最大数据包长度
    u8 bEpNum;          //<所使用的端点数（不包括端点0）

    u8 *pBuf;           //<接受EP0 FIFO buffer
}sUSB_CONTROL_TRANSFER;

typedef struct _USBH_REQUEST_BYTE	//<USB 事务
{
    u8 bmRequestType;	            //<[7]:传输方向 [6:5]:类型 [4:0]:接收方
    u8 bRequset;
    u8 wValueL;
    u8 wValueH;
    u8 wIndexL;
    u8 wIndexH;
    u8 wLengthL;
    u8 wLengthH;
}sUSBH_REQUEST_BYTE;

typedef union _USB_CONTROL
{
    sUSBH_REQUEST_BYTE SETUP_pkt; //
    u8  aStdReq[8];             //<Standard Request FIFO
}sUSB_CONTROL;

typedef struct _USB_BULK_ONLY_TRANSFER
{
    //u8 *pBuf;        //<Data 缓冲buffer
    u8 bMaxLUN;         //<
    u8 bCBWLUN;         //<
}sUSB_BULK_ONLY_TRANSFER;

/*****************************
        Typedef Mix
*****************************/
typedef struct _USB_VAR
{
    sUSB_DETECT             detect;
    sUSB_HOST_VAR           host_var;
    sUSB_MSD_CLASS_INFO     msd_class_info;
    sUSB_CONTROL            control_setup;
    sUSB_CONTROL_TRANSFER   control_trans;
    sUSB_BULK_ONLY_TRANSFER bulk_only_trans;
}sUSB_VAR;

typedef struct __HUSB_REG_VAR
{
    dev_io_t husb_io;
    OS_MUTEX husb_rwmutex;
}HUSB_REG_VAR;


extern const struct DEV_IO * dev_reg_usb(void *parm);

#endif //_MANGO_DEV_USB_H_

