#ifndef __FILE_OP_H__
#define __FILE_OP_H__

#include "file_op_err.h"
#include "logic_dev_list.h"
//------file op cmd------------------
typedef enum
{
    FOP_INIT=0,
    FOP_FIND_UPDATE_FILE=1,
    FOP_GET_TOTALFILE_NUM,
    FOP_OPEN_FILE,
    FOP_OPEN_FILE_BYPATH,
    FOP_OPEN_FILE_BYSCLUCT,
    FOP_READ_BPINFO,
    FOP_SAVE_BPINFO,
	FOP_CLEAR_BPINFO,
	FOP_READ_FLACBPINFO,
    FOP_SAVE_FLACBPINFO,
    FOP_SAVE_LASTOP_DEV,
    FOP_FIND_SPEC_LGDEV,
    FOP_ALLFILE_ERR_LGDEV,
    FOP_DEV_STATUS,
    FOP_CLOSE_LOGDEV,
    FOP_GET_PHYDEV_INFO,
    FOP_GET_FOLDER_INFO,
    FOP_GET_FILE_NAME,
    FOP_MAX,
}ENUM_FILE_CMD;

//------play mode------------------
typedef enum
{
    REPEAT_ALL = FOP_MAX+1,                   ///<全部循环
    REPEAT_ONE_LGDEV,                       ///<单个逻辑设备循环
    REPEAT_ONE,                             ///<单曲循环
    REPEAT_RANDOM,                          ///<单设备随机播放
    REPEAT_FOLDER,                          ///<文件夹循环
    
//    REPEAT_ONE_PHDEV,                       ///<单个物理设备循环
    MAX_PLAY_MODE,
}ENUM_PLAY_MODE;

//-----file sel mode----------------
typedef enum							//播放文件的方式定义
{
    PLAY_NEXT_FILE = MAX_PLAY_MODE+1,       ///<播放下一首
    PLAY_AUTO_NEXT,                         ///<播放下一首，播放结束时，自动下一首控制
    PLAY_FIRST_FILE,  		                ///<从第一个文件播放
    PLAY_BREAK_POINT,                       ///<从记忆文件和位置开始播放
    PLAY_LAST_FILE,				            ///<从最后一个文件播放
    PLAY_PREV_FILE,                         ///<播放上一首
    PLAY_SPEC_FILE,                         ///<输入指定文件序号
    PLAY_FILE_BYPATH   ,                    ///<输入包含文件名的路径(无文件名时，打开此路径下的第一个文件)
    PLAY_NEXT_FOLDER,                       ///<下一个文件夹
    PLAY_PRE_FOLDER,                        ///<上一个文件夹
	PLAY_AB_REPEAT,							///AB断点
    MAX_FILE_SEL_MODE,
} ENUM_FILE_SELECT_MODE;

//-----dev sel mode-----------------
typedef enum
{
    DEV_SEL_CUR = MAX_FILE_SEL_MODE+1,   ///<当前活动设备
    DEV_SEL_NEXT,                        ///<下一个设备
    DEV_SEL_PREV,                        ///<上一个设备
    DEV_SEL_FIRST,                       ///<第一个设备
    DEV_SEL_LAST,                        ///<最后一个设备
    DEV_SEL_SPEC,                        ///<指定设备
    MAX_DEV_MODE, //0x26
}ENUM_DEV_SEL_MODE;

typedef struct _FILE_OPERATE_INIT_
{
    ENUM_DEV_SEL_MODE      cur_dev_mode;        ///<设备选择模式
    ENUM_PLAY_MODE         cur_play_mode;       ///<文件播放模式
    ENUM_FILE_SELECT_MODE  cur_sel_mode;        ///<文件选择模式
    u32                    dev_let;             ///<指定设备
    u32 *                  filenum;             ///<文件编号
    u8 *                   filepath;            ///<文件路径
    u32                    filesclust;          ///<文件簇
}FILE_OPERATE_INIT;

typedef struct _FILE_OPERATE_
{
    FILE_OPERATE_INIT *fop_init;        ///<文件操作器操作文件属性
    lg_dev_info *cur_lgdev_info;        ///<当前文件操作器，对应的逻辑设备信息
}FILE_OPERATE;

u32 file_operate_get_total_phydev();
u32 file_operate_ctl(u32 cmd,void *parm,void *input,void *output);
#endif
