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

#ifndef _LIGHT_HEAD_H_
#define _LIGHT_HEAD_H_

#include "typedef.h"
#include "rcsp_head.h"
#include "rcsp/rcsp_interface.h"

//light mode
typedef enum
{
    LGT_MD_CLOSE = 0,
    LGT_MD_WHITE,
    LGT_MD_COLOR,
    LGT_MD_COLOR_FLASH,
    LGT_MD_SCENE,
}LGT_MD_E;

typedef enum
{
    LGT_COLOR_WHITE = 0,
    LGT_COLOR_RED,
    LGT_COLOR_GREEN,
    LGT_COLOR_BLUE,
    LGT_COLOR_BRIGHT,
    LGT_COLOR_MAX,
}LGT_COLOR_E;

typedef enum
{
    LGT_SCENE_RED = 0,
    LGT_SCENE_GREEN,
    LGT_SCENE_BLUE,
    LGT_SCENE_FREQ,
    LGT_SCENE_UNIT_MAX,
}LGT_SCENE_E;

#pragma pack(1)
typedef struct
{
    u16 start_value;
    u16 end_value;
    u16 time_value;
}light_flash_param_t;

typedef struct
{
    u16 start_value;
    u16 end_value;
    u16 time_value;
    u16 time_order;
}light_flash_t;
#pragma pack()


typedef struct
{
    u8  button;
    u8  mode;
    u8  m_value[2];//mode's private data
    u16 color_value[LGT_COLOR_MAX];
}light_info_t;

#define FLASH_STRUCT_MAX  8
typedef struct
{
    u8 tab_cnt;
    u8 tab_cycle;//unit:s
    light_flash_t flash_struct[FLASH_STRUCT_MAX];
}light_disp_t;


extern light_info_t  light_info_tab;
extern light_disp_t  *light_sence_table;

extern u32 cbk_lightTask_frame_deal(u8 *data,u16 len);
extern void light_init(void);
extern void light_exit(void);


#endif /* #ifndef _LIGHT_HEAD_H_ */





