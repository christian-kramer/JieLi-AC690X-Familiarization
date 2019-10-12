#include "linein_prompt.h"
#include "play_sel/play_sel.h"
#include "dac/dac.h"
#include "dac/dac_api.h"
#include "linein.h"

u8 linein_prompt_break = 0;//提示音打断标志
/*----------------------------------------------------------------------------*/
/**@brief  Linein提示音播放函数
   @param  file_name提示音文件号
   @return NULL
   @note   void linein_prompt_play(void *file_name )
*/
/*----------------------------------------------------------------------------*/
void linein_prompt_play(void * file_name)
{
#ifdef LINEIN_PROMPT_EN
    if(play_sel_busy())
    {
        play_sel_stop();
        linein_prompt_break = 1;
        puts("------------play_sel_stop----------\n");
    }
    puts("linein------------play_sel----------\n");
    aux_dac_channel_off();
    dac_channel_on(MUSIC_CHANNEL, FADE_ON);
    tone_play_by_name(LINEIN_TASK_NAME,1,file_name);
    while(play_sel_busy())
	{
		os_time_dly(2);
	}
    dac_channel_off(MUSIC_CHANNEL, FADE_ON);
//    aux_dac_channel_on();
#endif
}
