#include "dec/decoder_phy.h"
#include "dec/music_api.h"
#include "music_prompt.h"
#include "play_sel/play_sel.h"

u8 music_prompt_break = 0;//提示音打断标志
u32 prompt_op_msg = 0;    //prompt related operation
extern bool music_pp_flag;
#ifdef MUSIC_USB_SD_VOICE
//////////////////////语音区分 start//////////////////////////////////
#define MUSIC_DEVICE_USD	2   //continue play after prompt play end
//////////////////////语音区分 end//////////////////////////////////
#endif
extern u32 curr_device_using;
/*----------------------------------------------------------------------------*/
/**@brief  Music语音播放函数
   @param  file_name：语音文件号
           m_op_api：解码接口
           msg : prompt related operation
   @return NULL
   @note   u32 music_prompt_play(MUSIC_OP_API *m_op_api,void * file_name ,u32 msg)
*/
/*----------------------------------------------------------------------------*/
u32 music_prompt_play(MUSIC_OP_API *m_op_api,void *file_name,u32 msg)
{
    if(play_sel_busy())
    {
        music_prompt_break = 1;
        play_sel_stop();
    }
	
	prompt_op_msg = msg;
    m_op_api->dop_api->dec_api.save_brk = 1;//提示音结束断点播放
    music_stop_decoder(m_op_api);
    tone_play_by_name(MUSIC_TASK_NAME,1,file_name);
    return 1;
}

/*----------------------------------------------------------------------------*/
/**@brief  Music prompt play end 
   @param  
   @return res = 0:stop 
   		   res = 1:play 
   @note   u32 prompt_play_end()
*/
/*----------------------------------------------------------------------------*/
#define MUSIC_STOP	0   //stop after prompt play end
#define MUSIC_PLAY	1   //continue play after prompt play end
u32 prompt_play_end(void)
{
	u32 res = MUSIC_STOP;
	switch(prompt_op_msg)
	{
		case MSG_PROMPT_PLAY:
		case MSG_LOW_POWER:
			res = MUSIC_PLAY;
			puts("prompt_play_end:MSG_LOW_POWER\n");
			break;
		case MSG_MUSIC_PLAY:
			res = MUSIC_PLAY;
			puts("prompt_play_end:MSG_MUSIC_PLAY\n");
			break;
		case MSG_MUSIC_PAUSE:
			res = MUSIC_STOP;
			puts("prompt_play_end:MSG_MUSIC_PAUSE\n");
			break;
		case MSG_MAX_VOL:
			if(music_pp_flag)
				break;
			res = MUSIC_PLAY;
			break;
		case MSG_LOW_POWER_VOICE:
			if(music_pp_flag)
				break;
			res = MUSIC_PLAY;
			break;
//#ifdef MUSIC_USB_SD_VOICE
//////////////////////语音区分 start//////////////////////////////////
       case MSG_MUSIC_U_SD:
            res = MUSIC_DEVICE_USD;
            break;
		case MSG_USB_MP3:
		case MSG_SD_MP3:
            res = 2;
			break;
//////////////////////语音区分 end//////////////////////////////////
//#endif
		default:
			res = MUSIC_PLAY;
			puts("prompt_play_end:default\n");
			break;
	}
	return res;
}
