#ifndef _LINEIN_PROMPT_H_
#define _LINEIN_PROMPT_H_

#include "sdk_cfg.h"

#define LINEIN_PROMPT_EN

u8 linein_prompt_break;//提示音打断标志;
void linein_prompt_play(void * file_name);

#endif
