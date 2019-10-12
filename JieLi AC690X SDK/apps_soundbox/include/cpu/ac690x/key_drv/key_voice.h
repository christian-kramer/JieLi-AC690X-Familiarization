#ifndef _KEY_VOICE_H_
#define _KEY_VOICE_H_

#include "includes.h"

typedef struct __key_voice
{
    const s16 *sr_tab;
    u16 sr_tab_size;
    u16 step;
    s16 vol;
    u32 dac_rate;
    volatile u16 tab_cnt;
    volatile u8 point_cnt;
    volatile u8 rl;
}_key_voice;
extern _key_voice key_voice;

void key_voice_init(void);
u16 key_voice_busy(void);
void send_key_voice(u16 cnt);
u32 add_key_voice(s16 *buff,u32 len);
void key_voice_dac_channel(void);
void set_key_voice_sr(u16 sr);
#endif
