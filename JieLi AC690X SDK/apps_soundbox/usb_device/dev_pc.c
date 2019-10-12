#include "usb_device.h"
#include "sdmmc/sd_host_api.h"
#include "dev_pc.h"
#include "common/msg.h"
#include "dev_manage/device_driver.h"
#include "dac/dac.h"
#include "dac/dac_api.h"
#include "vm/vm_api.h"
#include "usb/usb_slave_api.h"
#include "sdk_cfg.h"
#include "dac/ladc.h"

#if USB_PC_EN

#define USB_PC_PROTECT_EN	0

#undef 	NULL
#define NULL				0

#define USB_SLAVE_DEV_IO_REG() \
    usb_slave_dev_io *dev_io = (usb_slave_dev_io *)(&susb_io);

#define __this(member) (dev_io->member)
/* #define __this(member) \ */
/* 		( dev_io->member = \ */
/* 		  (dev_io->member == NULL)? io_member_err_msg_output : dev_io->member \ */
/* 		) */

/* static void io_member_err_msg_output(void) */
/* { */
/* 	puts("susb io member not reg"); */
/* 	while(1); */
/* } */

#if USB_PC_PROTECT_EN

#define PROTECT_NUM		1
usb_pc_protect *protect_p = NULL;
static void usb_pc_protect_open(void)
{
	USB_SLAVE_DEV_IO_REG();

	u32 need_buf = sizeof(usb_pc_protect)+(PROTECT_NUM*2*sizeof(u32));
	printf("---need_buf = %d\n",need_buf);

	protect_p = malloc(need_buf);
	printf("protect_p = %x\n",protect_p);
	if(protect_p)
	{
		protect_p->gc_ProtectFATNum = PROTECT_NUM;
		protect_p->ProtectedFATSectorIndex[0] = 0x00;
		protect_p->ProtectedFATSectorIndex[1] = 0x04;
		//printf_buf(protect_p,3*4);
		__this(dev_ioctrl)((void*)protect_p, USB_SLAVE_MD_PROTECT);
	}
	else
	{
		puts("usb_pc_protect_malloc_err\n");
	}
}

static void usb_pc_protect_close(void)
{
	USB_SLAVE_DEV_IO_REG();
	__this(dev_ioctrl)((void*)NULL, USB_SLAVE_MD_PROTECT);

	if(protect_p)
	{
		free(protect_p);
		protect_p = NULL;
	}
}
#endif

/*----------------------------------------------------------------------------*/
/**@brief  PC 模式静音状态设置
   @param  mute_status：mute状态控制；fade_en：淡入淡出设置
   @return 无
   @note   void pc_dac_mute(bool mute_status, u8 fade_en)
*/
/*----------------------------------------------------------------------------*/
void pc_dac_mute(bool mute_status, u8 fade_en)
{
	dac_mute(mute_status, fade_en);
}

/*----------------------------------------------------------------------------*/
/**@brief  PC 模式DAC通道开启和音量设置
   @param  无
   @return 无
   @note   void pc_dac_on(void)
*/
/*----------------------------------------------------------------------------*/
void pc_dac_channel_on(void)
{
    dac_channel_on(UDISK_CHANNEL,FADE_ON);
//    set_sys_vol(dac_var.cur_sys_vol_l, dac_var.cur_sys_vol_r, FADE_ON);
}

/*----------------------------------------------------------------------------*/
/**@brief  PC 模式DAC通道关闭
   @param  无
   @return 无
   @note   void pc_dac_off(void)
*/
/*----------------------------------------------------------------------------*/
void pc_dac_channel_off(void)
{
	set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);//resume sys vol
    dac_channel_off(UDISK_CHANNEL, FADE_ON);
//    dac_var.bMute = 0;

#if USB_PC_PROTECT_EN
	usb_pc_protect_close();	//close at last
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief  PC 模式AUDIO音量设置消息钩子函数
   @param  msg：需要投递的消息
   @return 无
   @note   void hook_susb_msg(u32 msg)
*/
/*----------------------------------------------------------------------------*/
static void hook_susb_msg(u32 msg)
{
    //pc_puts("PC_VOL_POST_MSG\n");
    os_taskq_post_msg(PC_TASK_NAME, 1, msg);
}

/*----------------------------------------------------------------------------*/
/**@brief  从VM获取记忆音量值
   @param  none
   @return 记忆值
   @note   u8 get_usb_slave_volume_from_vm(void)
*/
/*----------------------------------------------------------------------------*/
static u8 get_usb_slave_volume_from_vm(void)
{
    u8 	pc_vol;

	//取出VM保存的上次PC音量值
    if(VM_PC_VOL_LEN != vm_read_api(VM_PC_VOL, &pc_vol))
    {
        pc_vol = MAX_SYS_VOL_L;
    }
    pc_printf("vm_pc_vol=%d\n", pc_vol);
    return pc_vol;
}

/*----------------------------------------------------------------------------*/
/**@brief  get left volume from vm
   @param  none
   @return left volume
   @note   u8 get_usb_slave_audio_left_volume(void)
*/
/*----------------------------------------------------------------------------*/
static u8 get_usb_slave_audio_left_volume(void)
{
    u8 spk_vol;

	spk_vol = get_usb_slave_volume_from_vm();
    spk_vol = (spk_vol >= MAX_SYS_VOL_L) ? 0xff : spk_vol << 3;

	return spk_vol;
}

/*----------------------------------------------------------------------------*/
/**@brief  get right volume from vm
   @param  none
   @return right volume
   @note   u8 get_usb_slave_audio_right_volume(void)
*/
/*----------------------------------------------------------------------------*/
static u8 get_usb_slave_audio_right_volume(void)
{
    u8 spk_vol;

	spk_vol = get_usb_slave_volume_from_vm();
    spk_vol = (spk_vol >= MAX_SYS_VOL_R) ? 0xff : spk_vol << 3;

	return spk_vol;
}

/*----------------------------------------------------------------------------*/
/**@brief  reset class config
   @param  dev_io:usb slave driver io
   @return none
   @note   void usb_slave_class_config_reset(usb_slave_dev_io *dev_io)
*/
/*----------------------------------------------------------------------------*/
static void usb_slave_class_config_reset(usb_slave_dev_io *dev_io)
{
	u32 parm = CLASS_CONFIG;

	__this(dev_ioctrl)(&parm, USB_SLAVE_CLASS_CONFIG_RESET);
}

/*----------------------------------------------------------------------------*/
/**@brief  register card reader
   @param  dev_io:usb slave driver io
   @return none
   @note   void usb_slave_card_reader_reg(usb_slave_dev_io *dev_io)
*/
/*----------------------------------------------------------------------------*/
static void usb_slave_card_reader_reg(usb_slave_dev_io *dev_io)
{
	card_reader_parm *reg_parm = NULL;

	/* memset(reg_parm, 0x00, sizeof(card_reader_parm)); */
    /*注册读卡器的SD控制器接口*/
#if SDMMC0_EN
	/* card_reader_parm reset_parm; */
	/* reg_parm = &reset_parm; */
	/* reset_parm.card_r_parm.usb_get_dev_capacity 	= xxx; */
	/* reset_parm.card_r_parm.sDevName 				= xxx; */
	/* reset_parm.card_r_parm.bWriteProtectFlag		= xxx; */
	__this(dev_ioctrl)(reg_parm, USB_SLAVE_CARD_READER0_REG);
#endif

#if SDMMC1_EN
	__this(dev_ioctrl)(reg_parm, USB_SLAVE_CARD_READER1_REG);
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief  init usb slave parm
   @param  dev_io:usb slave driver io
   @return init status
   @note   s32 usb_slave_var_init(usb_slave_dev_io *dev_io)
*/
/*----------------------------------------------------------------------------*/
static s32 usb_slave_var_init(usb_slave_dev_io *dev_io)
{
	usb_slave_init_parm susb_init_parm;

	susb_init_parm.os_msg_post_init = (void (*)(void))hook_susb_msg;
	susb_init_parm.vol_left 		= get_usb_slave_audio_left_volume();
	susb_init_parm.vol_right 		= get_usb_slave_audio_right_volume();

	return __this(dev_init)(&susb_init_parm);
}

/*----------------------------------------------------------------------------*/
/**@brief  init usb slave moudle
   @param  none
   @return init status
   @note   s32 app_usb_slave_init(void)
*/
/*----------------------------------------------------------------------------*/
s32 app_usb_slave_init(void)
{
	s32 res;
	USB_SLAVE_DEV_IO_REG();

	/*usb slave var init*/
	res = usb_slave_var_init(dev_io);
	if(res == NULL)
	{
		puts("susb var malloc error\n");
		return NULL;
	}

	/*audio dac init*/
    //for speaker
	if(CLASS_CONFIG & SPEAKER_CLASS)
	{
		dac_set_samplerate(48000, 0);     //DAC采样率设置为48K
		pc_dac_channel_on(); //开PC DAC模拟通道
	}

    /*MASS STORAGE、AUDIO和HID功能重新设置:默认全开*/
	usb_slave_class_config_reset(dev_io);

	/*register card_reader 属性*/
	usb_slave_card_reader_reg(dev_io);

    /*挂载USB SLAVE设备*/
	__this(dev_open)(NULL);

    //for mic
	if(CLASS_CONFIG & MIC_CLASS)
	{
		ladc_reg_init(ENC_MIC_CHANNEL, LADC_SR48000);
	}

#if USB_PC_PROTECT_EN
	usb_pc_protect_open();
#endif

	return res;
}

/*----------------------------------------------------------------------------*/
/**@brief  close usb slave moudle
   @param  none
   @return close status
   @note   s32 app_usb_slave_close(void)
*/
/*----------------------------------------------------------------------------*/
s32 app_usb_slave_close(void)
{
	s32 res;
	USB_SLAVE_DEV_IO_REG();

	res =  __this(dev_close)(NULL);

	if(CLASS_CONFIG & SPEAKER_CLASS)
	{
		pc_dac_channel_off(); //关PC DAC通道
	}
	if(CLASS_CONFIG & MIC_CLASS)
	{
		ladc_close(ENC_MIC_CHANNEL);	//关闭ladc模块
	}

	return res;
}

/*----------------------------------------------------------------------------*/
/**@brief  run card reader moudle
   @param  none
   @return run status
   @note   s32 app_usb_slave_card_reader(u32 cmd)
*/
/*----------------------------------------------------------------------------*/
s32 app_usb_slave_card_reader(u32 cmd)
{
	USB_SLAVE_DEV_IO_REG();

	return __this(dev_ioctrl)(&cmd, USB_SLAVE_RUN_CARD_READER);
}

/*----------------------------------------------------------------------------*/
/**@brief  run hid moudle
   @param  none
   @return run status
   @note   s32 app_usb_slave_hid(u32 hid_cmd)
*/
/*----------------------------------------------------------------------------*/
s32 app_usb_slave_hid(u32 hid_cmd)
{
	USB_SLAVE_DEV_IO_REG();

	return __this(dev_ioctrl)(&hid_cmd, USB_SLAVE_HID);
}

/*----------------------------------------------------------------------------*/
/**@brief  PC 模式AUDIO音量设置
   @param  pc_mute_status：mute状态
   @return AUDIO音量
   @note   u8 app_pc_set_speaker_vol(u32 pc_mute_status)
*/
/*----------------------------------------------------------------------------*/
u8 app_pc_set_speaker_vol(u32 pc_mute_status)
{
    u32 spk_vol_l, spk_vol_r;
	USB_SLAVE_DEV_IO_REG();

	__this(dev_ioctrl)(&spk_vol_l, USB_SLAVE_GET_SPEAKER_VOL);
	spk_vol_l&=0xff;
    //pc_printf("api_vol=%x, %x\n", ep0_var->usb_class_info.bAudioCurL[0], ep0_var->usb_class_info.bAudioCurR[0]);
    if(spk_vol_l)
    {
        spk_vol_l >>= 3;
        spk_vol_l = (spk_vol_l == 0) ? 1 : spk_vol_l;
        spk_vol_l = (spk_vol_l >= MAX_SYS_VOL_L) ? MAX_SYS_VOL_L : spk_vol_l;
    }

	__this(dev_ioctrl)(&spk_vol_r, USB_SLAVE_GET_SPEAKER_VOL);
	spk_vol_r>>=8;
	spk_vol_r&=0xff;
    if(spk_vol_r)
    {
        spk_vol_r >>= 3;
        spk_vol_r = (spk_vol_r == 0) ? 1 : spk_vol_r;
        spk_vol_r = (spk_vol_r >= MAX_SYS_VOL_L) ? MAX_SYS_VOL_L : spk_vol_r;
    }

    pc_dac_mute(pc_mute_status, FADE_ON);

    set_sys_vol(spk_vol_l, spk_vol_r, FADE_ON);

    //vm_write_api(VM_PC_VOL, &spk_vol_l);
    vm_cache_write(VM_PC_VOL, &spk_vol_l, 2);

    return spk_vol_l;
}

/*----------------------------------------------------------------------------*/
/**@brief  get usb slave online status
   @param  none
   @return run online status
   @note   u32 app_usb_slave_online_status(void)
*/
/*----------------------------------------------------------------------------*/
u32 app_usb_slave_online_status(void)
{
    u32 online_status;
	USB_SLAVE_DEV_IO_REG();

    return __this(dev_ioctrl)(&online_status, USB_SLAVE_GET_ONLINE_STATUS);
}

/*----------------------------------------------------------------------------*/
/**@brief  PC 在线列表
   @note   const u8 pc_event_tab[]
*/
/*----------------------------------------------------------------------------*/
static const u8 pc_event_tab[] =
{
    SYS_EVENT_PC_OUT, //PC拔出
    SYS_EVENT_PC_IN,  //PC插入
};


/*----------------------------------------------------------------------------*/
/**@brief  PC 在线检测API函数
   @param  无
   @return 无
   @note   void pc_check_api(void)
*/
/*----------------------------------------------------------------------------*/
void pc_check_api(u32 info)
{
	USB_SLAVE_DEV_IO_REG();
	s32 dev_status;

	__this(dev_ioctrl)(&dev_status, USB_SLAVE_ONLINE_DETECT);
	if(dev_status != DEV_HOLD)
	{
		os_taskq_post_event(MAIN_TASK_NAME, 2, pc_event_tab[dev_status],info);
	}
}

#undef	NULL

#endif
