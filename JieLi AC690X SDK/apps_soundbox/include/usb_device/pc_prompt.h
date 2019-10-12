#ifndef _PC_PROMPT_H_
#define _PC_PROMPT_H_

#include "sdk_cfg.h"

//#define PC_PROMPT_EN

extern volatile bool pc_prompt_playing;
// void pc_prompt_play(u8 file_num);
void pc_prompt_play(void * file_name);

#endif
