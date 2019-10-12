#include "echo_prompt.h"
#include "play_sel/play_sel.h"
//#include "voice_prompt.h"

bool echo_prompt_stopped;
/*----------------------------------------------------------------------------*/
/**@brief  Linein语音播放函数
   @param  file_num：语音文件号
   @return NULL
   @note   void linein_prompt_play(u8 file_num)
*/
/*----------------------------------------------------------------------------*/
void echo_prompt_play(void * file_name)
{
#ifdef ECHO_PROMPT_EN
    if(!echo_prompt_stopped)
        return;
    echo_prompt_stopped = 0;
    tone_play_by_name("ECHO_TASK",1,file_name);
#endif
}
