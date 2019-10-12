/*
 *********************************************************************************************************
 *                                                AC46
 *                                      fs browser select
 *                                             CODE
 *
 *                          (c) Copyright 2015-2016, ZHUHAI JIELI
 *                                           All Rights Reserved
 *
 * File : *
 * By   : jamin.li
 * DATE : 2015-10-15 build this file
 *********************************************************************************************************
 */

#ifndef _FILE_BS_DEAL_H_
#define _FILE_BS_DEAL_H_

#include "file_operate/file_op.h"
#include "file_op_err.h"
#include "fat/fs_io.h"
#include "fat/tff.h"
#include "vm/vm_api.h"
#include "dec/music_api.h"

#define BS_DIR_TYPE_FORLDER   0
#define BS_DIR_TYPE_FILE   1

#define BS_FNAME_TYPE_SHORT   0
#define BS_FNAME_TYPE_LONG   1

extern int file_comm_change_file_path(char *dest,char *src);
extern int file_comm_long_name_fix(u8 *str,u16 len);
extern int file_comm_display_83name(u8 *dest, u8 *src);
extern u32 file_bs_open_handle(MUSIC_OP_API *m_op_api,u8 *ext_name);
extern void file_bs_close_handle(MUSIC_OP_API *m_op_api);
extern u32 file_bs_entern_dir(MUSIC_OP_API *m_op_api,FS_DIR_INFO *dir_info);
extern u32 file_bs_exit_dir(MUSIC_OP_API *m_op_api);
extern u32 file_bs_get_dir_info(MUSIC_OP_API *m_op_api,FS_DIR_INFO *buf,u16 start_sn,u16 get_cnt);
extern void file_comm_change_display_name(char *tpath,LONG_FILE_NAME *disp_file_name,LONG_FILE_NAME *disp_dir_name);
//extern ;
//extern ;

#endif/*_FILE_BS_DEAL_H_*/
