#ifndef _MUSIC_PROMPT_H_
#define _MUSIC_PROMPT_H_

#include "sdk_cfg.h"

#define MUSIC_PROMPT_EN

u8 music_prompt_break;//提示音打断标志
u32 music_prompt_play(MUSIC_OP_API *m_op_api,void *file_name,u32 msg);
u32 prompt_play_end(void);

#endif // _MUSIC_PROMPT_H_
