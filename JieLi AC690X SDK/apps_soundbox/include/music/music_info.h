#ifndef MUSIC_INFO_H
#define MUSIC_INFO_H


#include "typedef.h"
#include "dec/music_api.h"
#include "fat/tff.h"

void * music_get_file_id3(u32 len);
void music_file_info(MUSIC_OP_API *m_op_api);
u8 get_floder_name(char* ptr,u32 filecount);
void get_floder_totalfile(u32 filecount);

//extern FS_DISP_NAME_T *music_play_display;
//extern char *music_play_tpath;

extern FS_DISP_NAME music_name_buf;
extern u32 music_playfile_total_time;
extern u32 music_cur_filenum,music_cur_filetotal;


#endif
