#include "pc_prompt.h"
///#include "play_sel.h"
///#include "voice_prompt.h"
#include "usb_device.h"

#if USB_PC_EN

volatile bool pc_prompt_playing = 0;

void pc_prompt_play(void * file_name)
{
#ifdef PC_PROMPT_EN
    if(pc_prompt_playing)
    {
        puts("pc_prompt_playing\n");
        return;
    }
    pc_prompt_playing = 1;
    tone_play_by_name(PC_TASK_NAME,1,file_name);
#endif
}

#endif
