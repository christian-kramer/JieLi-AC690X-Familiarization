#ifndef __FILE_OP_ERR_H__
#define __FILE_OP_ERR_H__

enum
{
///---------文件操作器错误
    FILE_OP_NO_ERR =0,
    FILE_OP_ERR_NOT_INIT,       ///<文件操作器没有初始化
    FILE_OP_ERR_INIT,           ///<文件操作器初始化错误
    FILE_OP_ERR_OP_HDL,         ///<文件操作器指针错误
    FILE_OP_ERR_NO_MEM,         ///<文件操作器申请内存失败
    FILE_OP_ERR_CMD,            ///<不能识别的操作命令

    FILE_OP_ERR_NO_FILE_ALLDEV, ///<所有设备都没有文件
    FILE_OP_ERR_NO_FILE_ONEDEV, ///<当前设备没有文件
    FILE_OP_ERR_OPEN_FILE,      ///<打开文件失败
    FILE_OP_ERR_OPEN_BPFILE,    ///<打开断点文件失败
    FILE_OP_ERR_NUM,            ///<打开文件序号错误
    FILE_OP_ERR_END_FILE,       ///<超过当前设备最大文件数
    FILE_OP_ERR_PRE_FILE,       ///<上一曲==0；

    FILE_OP_ERR_PTR,
    FILE_OP_ERR_LGDEV_NULL,      ///<没有设备
    FILE_OP_ERR_LGDEV_NO_FIND,   ///<没找到指定的逻辑设备
    FILE_OP_ERR_LGDEV_FULL,
    FILE_OP_ERR_LGDEV_MOUNT,
    FILE_OP_ERR_FS,


    ERR_MUSIC_API_NULL  = 0x20,
    ERR_MUSIC_NULL_OPT,
    ERR_MUSIC_START_DEC,
    SUCC_MUSIC_START_DEC
};

#define SUCC_MUSIC_DEC  FILE_OP_NO_ERR


#endif
