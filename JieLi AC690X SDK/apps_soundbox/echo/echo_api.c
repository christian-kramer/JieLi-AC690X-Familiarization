#include "common/app_cfg.h"
#include "rtos/os_api.h"
#include "rtos/os_cfg.h"
#include "common/error.h"
#include "common/msg.h"
#include "rtos/task_manage.h"
#include "dac/dac_api.h"
#include "play_sel/play_sel.h"
#include "ui/led/led.h"
#include "dac/dac_api.h"
#include "dac/ladc.h"
#include "echo/echo_api.h"
#include "echo/echo_deal.h"
#include "echo_ui.h"
#include "echo.h"


#if ECHO_EN

#define ECHO_DEF_DEEP_VAL  724 //默认深度 0-1024	//300ms
#define ECHO_DEF_VOL_VAL   128  //默认强度 0-128

static const EF_REVERB_PARM2 ef_parm = 
{
	/* unsigned short deepval; 设置后，reverb_init初始化值会被覆盖*/
	ECHO_DEF_DEEP_VAL,

	/* unsigned short decayval; 设置后，reverb_init初始化值会被覆盖*/
	ECHO_DEF_VOL_VAL,

	/* unsigned short gainval;  音量增益 最大值：4096*/
    4000,

	/* unsigned short rs_mode;  1:高精度变采样    0:低精度变采样 */
	0x101,

	/* unsigned short filter_mode;  1:使用低通滤波器  0:不使用滤波器*/
	1,

	/* Low_pass_para  lp_para; (reserved)*/
	{
		0,0,0,0,0
	}
};



//agc：可设置范围(-27~76)
//-12db		-27~0
//normal	0~63
//x2		64~76
static const MIC_VAR_SET mic_parm = 
{
	/* u8 use_magc;//agc开关 */
    1,

	/* s8 max_gain;//agc最大增益(-27~76) && > min_gain*/
	30,

	/* s8 min_gain;//agc最小增益(-27~76) && < max_gain*/
	0,

	/* reserved */
	0,

	/* s32 tms;//爆掉之后的音量抑制多少ms */
	400,

	/* 破音阀值：s32 target  lg(coeff*32768)*20480 (建议值：80000-90000)*/
    85000,
};



void echo_switch(void **mode_reverb, u8 on)
{
    if(on)
    {
        if(*mode_reverb)//not init
        {
            //already init
            puts("already init echo mic\n");
        }
        else
        {
            //printf("%s\n",get_echo_lib_ver());
			reg_mic_var((MIC_VAR_SET *)&mic_parm);
			reg_ef_reverb_parm2((EF_REVERB_PARM2 *)&ef_parm);

			//0-250: 啸叫抑制程度，值越大抑制效果越大，但会影响声音频谱，建议值120。
			howlingsuppress_suppression_val(120);

            howlingsuppress_sw(HOWLING_SUPPRESSION_EN);

            puts("reverb_init\n");
            *mode_reverb = (void*)reverb_init(ECHO_DEF_DEEP_VAL,ECHO_DEF_VOL_VAL);

            if(*mode_reverb)
            {
				/* echo_set_mic_vol(gain, 1);//*/

				ladc_pga_gain(2, 0);//2:mic_channel, 0:gain

                puts("init echo mic succ\n");
            }
            else
            {
                puts("init echo mic err\n");
            }
        }
    }
    else
    {
        puts("reverb_stop\n");
        reverb_stop(*mode_reverb);
        *mode_reverb = NULL;
    }
}

void echo_msg_deal(void **mode_reverb, int *msg)
{
    int flag = 1;

//    printf("echo-msg:%x\n",msg[0]);

    switch(msg[0])
    {
    /* case MSG_ECHO_PP: */
        /* if(*mode_reverb) */
        /* { */
            /* echo_switch(mode_reverb,0); */
        /* } */
        /* else */
        /* { */
            /* echo_switch(mode_reverb,1); */
        /* } */
        /* break; */

    case MSG_ECHO_START:
        if(*mode_reverb)
       	{
            puts("echo already running\n");
        }
        else
       	{
            echo_switch(mode_reverb,1);
        }

        break;

    case MSG_ECHO_STOP:
		if(*mode_reverb)
		{
			echo_switch(mode_reverb,0);
		}
		else
		{
			puts("echo not run\n");
		}

		break;

    default:
        flag = 0;
        break;
    }

    if(flag != 0)
    {
        UI_menu(MENU_REFRESH);
    }
}

void echo_exit(void **mode_reverb)
{
    reverb_stop(*mode_reverb);
    *mode_reverb = NULL;
}
#endif
