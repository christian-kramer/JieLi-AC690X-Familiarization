#include "sdk_cfg.h"
#include "fm_prompt.h"
#include "play_sel/play_sel.h"
#include "fm/fm_radio.h"
#include "rtos/os_api.h"
#include "fm/fm_api.h"
#include "common/msg.h"
#include "dac/dac_api.h"

#ifdef FM_PROMPT_EN

u8 fm_prompt_break = 0;

/*----------------------------------------------------------------------------*/
/**@brief  FM语音提示函数
   @param  file_name：语音文件号
   @return NULL
   @note   void fm_prompt_play(void * file_name)
*/
/*----------------------------------------------------------------------------*/
void fm_prompt_play(void * file_name)
{
    if(play_sel_busy())
	{
		play_sel_stop();
		fm_prompt_break = 1;
	}
    fm_module_mute(1);
	clear_dac_buf(1);
    fm_save_info();
    tone_play_by_name(FM_TASK_NAME,1,file_name);
}

#endif
