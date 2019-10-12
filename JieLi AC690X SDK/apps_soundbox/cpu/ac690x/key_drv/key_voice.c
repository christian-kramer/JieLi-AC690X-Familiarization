#include "dac/dac.h"
#include "key_drv/key_voice.h"
#include "sdk_cfg.h"
#include "cpu/dac_param.h"
#include "dac/dac_api.h"

///key voice table for 44.1KHz, 16 * 16bit
#define KEY_VOICE_44K_TAB_MAX	16
const s16 key_voice_44100Hz_tab[KEY_VOICE_44K_TAB_MAX] =
{
    0,
	10922,
	20181,
	26367,
	28540,
	26367,
	20181,
	10922,
	0,
	-10922,
	-20181,
	-26367,
	-28540,
	-26367,
	-20181,
	-10922,
};

///key voice table for 48KHz, 18 * 16bit
#define KEY_VOICE_48K_TAB_MAX	18
const s16 key_voice_48000Hz_tab[KEY_VOICE_48K_TAB_MAX] =
{
    0,
	9761,
	18345,
	24716,
	28106,
	28106,
	24716,
	18345,
	9761,
	0,
	-9761,
	-18345,
	-24716,
	-28106,
	-28106,
	-24716,
	-18345,
	-9761
};
//key voice table for 32KHz, 12 * 16bit
#define KEY_VOICE_32K_TAB_MAX	12
const s16 key_voice_32000Hz_tab[KEY_VOICE_32K_TAB_MAX] =
{
//    0,
//    10837,
//    21675,
//    32512,
//    21675,
//    10837,
//    0,
//    -10837,
//    -21675,
//    -32512,
//    -21675,
//    -10837
    16384,
    28378,
    32767,
    28378,
    16384,
    0,
    -16384,
    -28378,
    -32767,
    -28378,
    -16384,
    -1
};

_key_voice key_voice;

void key_voice_init(void)
{
    memset(&key_voice,0,sizeof(_key_voice));
    key_voice.vol = 2000;
}
u16 key_voice_busy(void)
{
    return key_voice.tab_cnt;
}

void set_key_voice_sr(u16 sr)
{
	key_voice.dac_rate = sr;
}

void send_key_voice(u16 cnt)
{
#if KEY_TONE_EN
    u8 step = 0;

	dac_ctl.keyvoice_check = 0;
	digit_auto_mute_set(AUTO_MUTE_ENABLE,-1, -1,-1);
	if(dac_ctl.toggle == 0) {
		dac_ctl.sniff_check = 1;
		dac_on_control();
	}
	else {
		dac_ctl.sniff_check = 0;
	}

    if(key_voice.tab_cnt || is_dac_mute())
    {
		//printf("key_voice return,cnt:%d  mute:%d\n",key_voice.tab_cnt,is_dac_mute());
        return ;
    }
    switch(key_voice.dac_rate)
    {
        /**************************************/
        case 12000://step = 4
            step += 2;
        case 24000://step = 2
            step++;
        case 48000://step = 1
            step++;
            key_voice.sr_tab = key_voice_48000Hz_tab;
            key_voice.sr_tab_size = KEY_VOICE_48K_TAB_MAX;
            break ;
        /**************************************/
        case 11025:
            step += 2;
        case 22050:
            step++;
        case 44100:
            step++;
            key_voice.sr_tab = key_voice_44100Hz_tab;
            key_voice.sr_tab_size = KEY_VOICE_44K_TAB_MAX;
            break ;
        /**************************************/
        case 8000:
            step += 2;
        case 16000:
            step++;
        case 32000:
            step++;
            key_voice.sr_tab = key_voice_32000Hz_tab;
            key_voice.sr_tab_size = KEY_VOICE_32K_TAB_MAX;
            break ;
        default:
            puts("key_voice.dac_rate:");
            put_u32hex(key_voice.dac_rate);
            return ;
    }
    key_voice.step = step;
    key_voice.tab_cnt = cnt;
	dac_ctl.keyvoice_check = 1;
#endif
}


u32 add_key_voice(s16 *buff,u32 len)
{
#if KEY_TONE_EN
    if(key_voice.tab_cnt)
    {
        u32 i = 0;
        while(i < len)
        {
            s32 tmp32;
            s16 tmp16;

            tmp16 = buff[i];
            tmp32 = (key_voice.sr_tab[key_voice.point_cnt]*key_voice.vol)>>14;
            tmp32 += tmp16;

            if(tmp32 < -32768)
            {
                tmp32 = -32768;
            }
            else if(tmp32 > 32767)
            {
                tmp32 = 32767;
            }
            buff[i++] = tmp32;//tmp16;//

            key_voice.rl++;
            if(key_voice.rl > 1)
            {
                key_voice.rl = 0;
                key_voice.point_cnt += key_voice.step;
                if(key_voice.point_cnt >= key_voice.sr_tab_size)
                {
                    key_voice.tab_cnt--;
                    key_voice.point_cnt = key_voice.point_cnt%key_voice.sr_tab_size;
                    if(0 == key_voice.tab_cnt)
                    {
                        break;
                    }
                    //send_key_voice(key_voice.tab_cnt);
                }
            }
        }
		if(key_voice.tab_cnt == 0) {
			//puts("key_voice_end\n");
			digit_auto_mute_set(AUTO_MUTE_CFG,-1,-1,-1);
		}
    }
#endif // KEY_TONE_EN
    return 0;
}

void key_voice_dac_channel(void)
{
#if KEY_TONE_EN
    static u8 cnt = 0;
    static u8 statu = 0;//0:
    u16 channel = get_cur_dac_channel();

    if((channel&0xff) > DAC_DIGITAL_D_A_VOL)//analog channel
    {
        if(key_voice.tab_cnt)
        {
            cnt = 100;//È¥¶¶Ê±³¤£º2ms*100
        }
        else
        {
            if(cnt)
            {
                cnt--;
            }
        }

#if ECHO_EN==0
		if(statu == 0 && cnt >1)
		{
			statu = 1;
			/* DAC_DIGITAL_EN(1); */
			/* TRIM_EN(0); */
			dac_digital_en(1);
			dac_trim_en(0);
			puts("---DAC_DIGITAL_EN\n");
		}
		else if(statu == 1 && cnt == 0)
		{
			statu = 0;
			/* DAC_DIGITAL_EN(0); */
			/* TRIM_EN(1); */
			dac_digital_en(0);
			dac_trim_en(1);
			puts("---DAC_DIGITAL_DIS\n");
		}
#endif
    }
#endif // KEY_TONE_EN
}
