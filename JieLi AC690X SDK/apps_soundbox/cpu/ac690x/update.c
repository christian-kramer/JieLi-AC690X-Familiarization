#include "sdk_cfg.h"
#include "includes.h"
#include "rtos/os_api.h"
#include "common/app_cfg.h"
#include "common/common.h"
#include "rtos/task_manage.h"
#include "key_drv/key.h"
#include "dac/dac_api.h"
#include "sys_detect.h"
#include "ui/led/led.h"
#include "key_drv/key_voice.h"
#include "dac/dac.h"
#include "update.h"

#define UPDATE_LED_REMIND
#define UPDATE_VOICE_REMIND

#define UPDATE_SUCCESS_END 0x88
#define DEVICE_UPDATE_KEY_ERR   BIT(30) 
#define DEVICE_FIRST_START  BIT(31) 

//called by dev_updata_cfg()
void led_update_start(void)
{
#ifdef UPDATE_LED_REMIND
    puts("led_update_start\n");
    JL_PORTB->DIR &= ~BIT(11);
    JL_PORTB->OUT &= ~BIT(11);
#endif
}

void led_update_finish(void)
{
#ifdef UPDATE_LED_REMIND
    puts("led_update_finish\n");
    JL_PORTB->DIR &= ~BIT(11);
    JL_PORTB->OUT |=  BIT(11);
#endif
}

u32 update_flag = 0;
void update_check(u32 res)
{
    update_flag = res;
}
bool device_is_first_start()
{   
    printf("update_flag=0x%x\n",update_flag );
    if((update_flag&DEVICE_FIRST_START)||(update_flag&DEVICE_UPDATE_KEY_ERR))
    {

        puts("\n=================device_is_first_start=========================\n");
        return true;
    }
    return false;
        
}
bool device_is_updata_over()
{
	if((update_flag&0xff) == UPDATE_SUCCESS_END) 
	{
        puts("\n=================device_is_updata_over=========================\n");
        return true;
			
	}
    return false;
}

/*----------------------------------------------------------------------------*/
/**@brief   update deal 
   @param   option=UPDATE_OK_RUN:system run directly when update successfully
                  =UPDATE_OK_WHILE:keep remainding when update successfully 
   @return  void
   @note    You can select remaind modes between led_display and key_tone, or 
            define any other ways you prefer.
 */
/*----------------------------------------------------------------------------*/
static u8 key_voice_cnt = 0;
void update_deal(u8 option)
{
	if(update_flag&DEVICE_UPDATE_KEY_ERR)
	{
        puts("\n----------------waring update key err not update---------------- \n");
		goto UPDATA_KEY_ERR_DEAL;	
	}
    if((update_flag&0xff) != UPDATE_SUCCESS_END) {
        puts("Normal PowerUp\n");
        return;
    }
    
    puts("\n==========================================\n");
    puts("   Update Success    ");
    puts("\n==========================================\n");

    if(option == UPDATE_OK_RUN) {
        puts("update run\n");
        return;
    }
UPDATA_KEY_ERR_DEAL:
#ifdef UPDATE_VOICE_REMIND
    audio_init();
    dac_channel_on(MUSIC_CHANNEL,FADE_ON);
    set_sys_vol(30,30,FADE_ON);
#endif

#ifdef UPDATE_LED_REMIND
    led_update_finish();
#endif

    while(1)
    {
        clear_wdt();
        key_voice_cnt++;
#ifdef UPDATE_VOICE_REMIND
	if((update_flag&DEVICE_UPDATE_KEY_ERR))
	{
        send_key_voice(10000);
   	    delay(0xeFFFFF);
	}
	else
	{
        send_key_voice(500);
   	    delay(0xAFFFFF);
	}
        
#endif
        if(key_voice_cnt > 5)
        {
            key_voice_cnt = 0;
            soft_power_ctl(PWR_OFF);
        }
    }
} 
