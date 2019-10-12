#ifndef MUSIC_TASK_H
#define MUSIC_TASK_H

#include "typedef.h"
#include "echo/echo_api.h"

struct music_cfg{
	const char *dev;
};

extern const struct task_info music_task_info;

extern u8 play_mode;
extern u8 eq_mode;
extern void *music_reverb;
#endif
