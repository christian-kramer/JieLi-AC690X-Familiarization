#include "bluetooth/avctp_user.h"
#include "aec/aec_api.h"
#include "common/app_cfg.h"
#include "dac/dac_api.h"
#include "vm/vm_api.h"
#include "common/msg.h"
#include "sdk_cfg.h"
#include "clock.h"
#include "ui/led/led.h"
#include "key_drv/key_drv_ad.h"
#include "dac/dac.h"
#include "fcc_test.h"
#include "dev_manage/dev_ctl.h"
#include "dac/dac.h"
#include "dac/eq.h"
#include "rtc/rtc_api.h"
#include "play_sel/play_sel.h"
#include "rcsp/rcsp_interface.h"
#include "uart_param.h"

extern volatile u8 low_power_flag;

static void in_low_pwr_port_deal(u8 mode) sec(.poweroff_text);
static void in_low_pwr_port_deal(u8 mode)
{
	if(mode == 2)   //enter sleep mode
	{
		JL_PORTA->DIR = 0xffff;
		JL_PORTA->PU = 0;
		JL_PORTA->PD = 0;
		JL_PORTA->DIE = 0;

		/* JL_PORTA->DIR |= 0xffdf; */
		/* JL_PORTA->PU &= ~0xffdf; */
		/* JL_PORTA->PD &= ~0xffdf; */
		/* JL_PORTA->DIE &= ~0xffdf; */
		JL_PORTB->DIR |= 0xfdff;
		JL_PORTB->PU &= ~0xfdff;
		JL_PORTB->PD &= ~0xfdff;
		JL_PORTB->DIE &= ~0xfdff;
		
		//JL_PORTB->OUT |= BIT(9);  //close led
	}
	else
	{
		JL_PORTA->DIR |= 0xffe7;
		JL_PORTA->PU &= ~0xffe7;
		JL_PORTA->PD &= ~0xffe7;
		JL_PORTA->DIE &= ~0xffe7;

		JL_PORTA->OUT &= ~BIT(4);  //close led
	}

  //  JL_PORTB->DIR = 0xffff;
  //  JL_PORTB->PU  = 0;
  //  JL_PORTB->PD  = 0;
   // JL_PORTB->DIE = 0;

	JL_PORTB->DIR |= 0xfdff;
	JL_PORTB->PU &= ~0xfdff;
	JL_PORTB->PD &= ~0xfdff;
	JL_PORTB->DIE &= ~0xfdff;
	
//	JL_PORTB->OUT |= BIT(9);  //close led

    JL_PORTC->DIR = 0xffff;
    JL_PORTC->PU  = 0;
    JL_PORTC->PD  = 0;
    JL_PORTC->DIE = 0;
	

	/*注意spi flash的几个脚不能关*/
	JL_PORTD->DIR |= 0xfff0;
	JL_PORTD->PU  &= ~0xfff0;
	JL_PORTD->PD  &= ~0xfff0;
	JL_PORTD->DIE &= ~0xfff0;

    /////////////////usb///////////////////
    USB_DP_DIR(1); //DP设置为输入
    USB_DM_DIR(1); //DP设置为输入
    USB_DP_PD(0);
    USB_DM_PD(0);
    USB_DM_PU(0);
    USB_DP_PU(0);
    JL_USB->CON0 = 0;
}

static u32 sniff_time_cnt =0;
/*不属于用户接口，协议栈回调函数*/
extern void io_key_init(void);
extern void ad_key0_init_io();
extern u8 is_pwrtimer_en();
static void out_low_pwr_deal(u32 time_ms)
{
	/* printf("%d\n",time_ms); */

#if KEY_IO_EN
    io_key_init();
#endif

#if KEY_AD_EN
	ad_key0_init_io();
#endif


	if(is_pwrtimer_en())
	{
		return;
	}

	sniff_time_cnt += time_ms;
	/* printf("%d\n",sniff_time_cnt); */
	if(sniff_time_cnt>=3000)
	{
		sniff_time_cnt =0;
	}
	else
	{
	}

   // printf("sniff_time_cnt:%d  time_ms:%d\n",sniff_time_cnt,time_ms);
}
static void bt_updata_run()
{
	puts("-------bt_updata_run\n");
	os_taskq_post_msg("MainTask",3,MSG_UPDATA,BT_UPDATA, NULL);
}
static void low_pwr_deal(u8 mode,u32 timer_ms) sec(.poweroff_text);
static void low_pwr_deal(u8 mode,u32 timer_ms)
{
	if(mode)
    {
		in_low_pwr_port_deal(mode);
	}
	else
	{
		out_low_pwr_deal(timer_ms);
	}
}

///---sdp service record profile- 用户选择支持协议--///
#define USER_SUPPORT_PROFILE_SPP    0
#define USER_SUPPORT_PROFILE_HFP    1
#define USER_SUPPORT_PROFILE_A2DP   1
#define USER_SUPPORT_PROFILE_AVCTP  1
#define USER_SUPPORT_PROFILE_HID    0
static void bt_profile_select_init(void)
{
    u8 support_profile;
    support_profile = SPP_CH | HFP_CH | A2DP_CH | AVCTP_CH | HID_CH | AVDTP_CH;
#if (USER_SUPPORT_PROFILE_HFP==0)
    support_profile &= ~HFP_CH;
#endif
#if (USER_SUPPORT_PROFILE_AVCTP==0)
    support_profile &= ~AVCTP_CH;
#endif
#if (USER_SUPPORT_PROFILE_A2DP==0)
    support_profile &= ~A2DP_CH;
    support_profile &= ~AVCTP_CH;
    support_profile &= ~AVDTP_CH;
#endif
#if (USER_SUPPORT_PROFILE_SPP==0)
    support_profile &= ~SPP_CH;
#endif
#if (USER_SUPPORT_PROFILE_HID== 0)
    support_profile &= ~HID_CH;
#endif

    bt_cfg_default_init(support_profile);/*外部设置支持什么协议*/

#if BT_HID_INDEPENDENT_MODE
    set_hid_independent_info();//要在 bt_cfg_default_init()设置后
#endif

}

//保存对箱做主机还是做从机的信息到vm
static void bt_save_stereo_info(u8 stereo_type)
{
	u8 stereo_info;
    stereo_info = stereo_type;
    vm_write_api(VM_BT_STEREO_INFO,&stereo_info );
	printf("------stereo_info=0x%x\n",stereo_info );
}

///对箱主连接成功之后，同步系统音量到从机
static void stereo_sys_vol_sync()
{
     stereo_host_cmd_handle(MSG_VOL_STEREO,dac_ctl.sys_vol_l);
#if EQ_EN
     os_taskq_post_msg("btmsg", 1, MSG_BT_SYNC_STEREO_EQ);
#endif
}

///对箱命令处理
static void stereo_deal_cmd(u8 msg,u8 value)
{
    if(msg < MSG_MAIN_MAX)
    {
         os_taskq_post_msg("MainTask", 2, msg,value);
    }
    else if(keymsg_task_name)
    {
        os_taskq_post_msg(keymsg_task_name, 2, msg,value);
    }
}


#if(BT_MODE == NORMAL_MODE)
	static const char host_name[]="AC69-Classic";
#else
	static const char host_name[]="AC69-RAM";
#endif
static const char hid_name[]="AC69_HID";
extern void __bt_set_background_conn_type_flag(u8 flag);

static void bt_function_select_init()
{
    /*蓝牙功能流程选择配置*/
#if(BT_MODE==NORMAL_MODE)
    u8 debug_addr[6] = {0x88, 0x67, 0x66, 0x89, 0x33, 0x11};
	__set_use_default_value(0);
#else
    u8 debug_addr[6] = {0x11, 0x22, 0x33, 0x33, 0x22, 0x11};
	__set_use_default_value(1);
#endif
    __set_host_name(host_name,sizeof(host_name));
    __set_hid_name(hid_name,sizeof(hid_name));
    __set_pin_code("0000");
    __set_bt_mac_addr(debug_addr);

    bt_profile_select_init();

    __bt_set_sniff(SNIFF_MODE_CONF,SNIFF_CNT_TIME);/*设置进入sniff是进入poweroff还是powerdown，设置空闲多少s进入sniff*/

    __bt_set_update_battery_time(10000); /*设置电量显示发送更新的周期时间，为0表示关闭电量显示功能，单位毫秒，u32, 不能小于5000ms*/
    __bt_set_a2dp_auto_play_flag(0); /*高级音频连接完成自动播放歌曲使能, 0不使能，1使能*/
    __set_simple_pair_flag(1);       /*提供接口外部设置配对方式,1使能简易配对。0使用pin code, 会使用配置文件的值*/
#if BT_STEREO
	__set_sbc_cap_bitpool(38); 
    __set_user_ctrl_conn_num(2);     /*用户设置支持连接的个数，1 或 2*/
    __set_auto_conn_device_num(2);   /*该接口用于设置上电回连需要依次搜索设备的个数。0表示上电不回连。大于20无效，直到连上一个*/
    __set_stereo_device_indicate("MO");/*设置对箱搜索标识,support两个字符标识，inquiry时候用,搜索到相应的标识才允许连接*/
    __set_searche_timeout_value(12000); /*设置蓝牙搜索的时间。ms单位。蓝牙搜索用*/
    __set_connect_stereo_timeout_value(0); /*设置回连对箱超时时间，0为不超时，直到连接上对箱为止*/
    __set_start_connet_stereo(1);          /*设置开机是否回连对箱*/
    __set_soundtrack_compound(1);          /*设置soundtrack,对箱左右声道分离使能*/
    __set_indicate_name_cmp_en(0);      /*对箱相同蓝牙名字不进行配使能*/
#else
	__set_sbc_cap_bitpool(53); 
	__set_user_ctrl_conn_num(1);     /*用户设置支持连接的个数，1 或 2*/
    __set_auto_conn_device_num(1);   /*该接口用于设置上电回连需要依次搜索设备的个数。0表示上电不回连。大于20无效，直到连上一个*/
#endif
    __set_page_timeout_value(5000); /*回连搜索时间长度设置,可使用该函数注册使用，ms单位,u16*/
#if BT_HID_INDEPENDENT_MODE
    __set_hid_auto_disconn_en(0);    /*是否自动断开手机的HID连接,1会断开，0是保留HID连接*/
#else
    __set_hid_auto_disconn_en(1);    /*是否自动断开手机的HID连接,1会断开，0是保留HID连接*/
#endif
    __set_test_masetr_enable(1);     /*接口配置是测试盒模式还是蓝牙SDK*/
    __set_super_timeout_value(8000); /*回连时超时参数设置。ms单位。做主机有效*/
    __set_connect_scan_timeout_value(0);/*//设置开启蓝牙可发现可连接的时间，为了省电，设置一个自动关闭可连接的时间。ms单位。为 0 时不使用自动关闭*/
    //__set_connect_scan_with_key(0);     /*连接上一台之后，连接第二台是否通过按键开启connect_scan*/
	__bt_set_music_back_play_flag(1);
    __set_music_break_in_flag(1);  /* 音频抢断开关，0：不抢断，1：抢断*/
    __set_hfp_switch(1);             /*通话抢断开关，0：不抢断，1：抢断*/
#if BT_STEREO
	__bt_set_background_conn_type_flag(0);
#else
	__bt_set_background_conn_type_flag(1);
#endif
	//__change_hci_class_type(BD_CLASS_WEARABLE_HEADSET);/*改变蓝牙设备类型，可以改变连接成功显示图标*/
}

 /*
    以下是一些库的钩子函数注册，一般用于改变参数。
    为了兼容不同客户对不同参数的需求，所有钩子函数不注册将使用默认参数。
    下面给的一些回调函数接口，用户都不能主动调用，】
    回调函数属于协议栈获取需要值或者及时反馈的接口
 */
/*电量等级变化*/
#define POWER_FLOOR_LEVEL   32
static int bt_get_battery_value()
{   //只支持1-9直接的值，9个等级
    u16 battery_level;
    if(get_battery_level() <= POWER_FLOOR_LEVEL)
        battery_level = 1;
    else
        battery_level = get_battery_level() - POWER_FLOOR_LEVEL;
    return battery_level;
}

/*电话时改变音量的状态值，注意是电话音量操作接口使用后会有反馈*/
static void call_vol_change(int vol)
{
	//iphone(0-15level)
    printf("vol %d",vol);
}
/*用接口读取名字时会从该回调函数返回，默认连接上就有一次回调，
要使用远端设备名字的可以copy保存*/
static void bt_read_remote_name(u8 *name)
{
    puts("\n remote name : ");
    puts((char *)name);
    puts("\n");
}

/*返回一个标志，有需要可以添加操作，*/
static void bt_is_auto_conn(u8 auto_conn_flag)
{   //区分回连成功还是被连成功
    if(auto_conn_flag)
    {
        //回连成功
    }
	else
	{
        //被连成功
    }
}

extern int spp_mutex_init(void);
extern int spp_mutex_del(void);
static void spp_data_deal(u8 packet_type, u16 channel, u8 *packet, u16 size)
{
    //因为修改了user_send_cmd_prepare接口发送串口数据会peng着发完才返回。
    //但是该回调指针是属于协议栈循环调用的，不能用user_send_cmd_prepare接口发数。
    //要发数用可以直接int spp_data_try_send(const u8* buf, u16 len)接口
    switch(packet_type)
    {
        case 1:
             //连接成功
             puts("spp connect\n");
    		 spp_mutex_init();
             break;
        case 2:
             //连接断开
             puts("spp disconnect\n");
			 spp_mutex_del();
             break;
        case 7://DATA
            ///puts("SP ");
		if(packet[0]=='9')
		{
		    //os_taskq_post_msg(keymsg_task_name, 1,MSG_BT_SPP_UPDATA);
		}
#if EQ_EN
            if(packet[0]=='E'&&packet[1]=='Q')
             {
                    update_ef_info(packet+4,size-4,packet[2]);
                    break;
             }
#endif // EQ_EN
             break;
    }
}

#if SUPPORT_APP_RCSP_EN
extern void ble_enter_mutex(void);
extern void ble_exit_mutex(void);
static void spp_rcsp_data_deal(u8 packet_type, u16 channel, u8 *packet, u16 size)
{
    //因为修改了user_send_cmd_prepare接口发送串口数据会peng着发完才返回。
    //但是该回调指针是属于协议栈循环调用的，不能用user_send_cmd_prepare接口发数。
    //要发数用可以直接int spp_data_try_send(const u8* buf, u16 len)接口
    switch(packet_type)
    {
        case 1:
             //连接成功
             puts("spp connect\n");
    		 spp_mutex_init();
#if (BLE_BREDR_MODE&BT_BLE_EN)
 			 ble_enter_mutex();
#endif
 			 rcsp_event_com_start(RCSP_APP_TYPE_SPP);
             rcsp_register_comsend(bt_spp_send_data);
             break;
        
		case 2:
             //连接断开
             puts("spp disconnect\n");
             rcsp_event_com_stop();
#if (BLE_BREDR_MODE&BT_BLE_EN)
             ble_exit_mutex();
#endif
			 spp_mutex_del();
             break;
        
		case 7://DATA
            ///puts("SP ");
            rcsp_comdata_recieve(packet,size);
             break;
    }
}
#endif

/*set_device_volume,get_dac_vol是高级音频同步手机音量相关的，注意跟电话时候的区分
用户不使用*/
static void set_device_volume(int volume )
{
	u8 dac_max_vol = get_max_sys_vol(1);
    dac_ctl.sys_vol_l  = volume * dac_max_vol / 0x7F;
    dac_ctl.sys_vol_r = dac_ctl.sys_vol_l  ;
    if(is_dac_mute()) {
   		dac_mute(0,1);
    }
#if BT_STEREO
	if(!is_check_stereo_slave())
	{
		stereo_host_cmd_handle(MSG_VOL_STEREO,dac_ctl.sys_vol_r);
			
	}
#endif
    set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_ON);
	printf("vol_sync,dac = %d\n",dac_ctl.sys_vol_l);
}

static int get_dac_vol()
{
	int dac_max_vol = get_max_sys_vol(1);
	int phone_vol = dac_ctl.sys_vol_l * 0x7F / dac_max_vol;
	printf("vol_sync,phone = %d\n",phone_vol);
    /*The value 0x0 corresponds to 0%. The value 0x7F corresponds to 100%.*/
    return phone_vol;
}

//phone_talking_end,save phone_vol and recover dac_vol
static void phone_sync_sys_dac_vol(u8 vol,u8 phone_vol)
{
   dac_ctl.sys_vol_l = vol;
   dac_ctl.sys_vol_r = vol;
   dac_ctl.phone_vol = phone_vol;
   vm_write_api(VM_PHONE_VOL,&dac_ctl.phone_vol);
   printf("phone_vol:%d ,sys_vol:%d\n",dac_ctl.phone_vol,vol);
}

//phone_talking_start,set phone_vol with vm_phone_vol
#define PHONE_VOL_REMEMBER
static u8 phone_sync_phone_vol(u8 phone_vol_max)
{
	u8 phone_vol = 0;
    //vm_read_api(VM_PHONE_VOL,&phone_vol);
  	printf("phone_vol:%d,max_vol:%d\n",dac_ctl.phone_vol,phone_vol_max);
	if(dac_ctl.phone_vol < 10)
		dac_ctl.phone_vol = phone_vol_max;

#ifndef	PHONE_VOL_REMEMBER
	dac_ctl.phone_vol = phone_vol_max;
#endif

   	dac_ctl.sys_vol_l = dac_ctl.phone_vol;
   	dac_ctl.sys_vol_r = dac_ctl.phone_vol;
	return dac_ctl.phone_vol;
}

extern u8 get_key_invalid_flag(void);

/*检测是否可以进入低功耗*/
/*1 : 可以进lowpower   0: 不可以进lowpower*/
static u32 check_enter_lowpower(void)
{


	//非静音模式不进lowpower模式
    if(!is_auto_mute())
	{
		return 0;
	}

	if(play_sel_busy())
	 {
		//puts("play_sel_busy\n");
		return 0;
	}

	//playing key_voice,don't enter lowpower
	if(dac_ctl.sniff_check) 
	{
		return 0;
	}

	//mute dac module
	if(!is_dac_mute())
	 {
		puts("---off----\n");
		os_taskq_post(MAIN_TASK_NAME,1,MSG_DAC_OFF);
		return 0;
	}

#if SYS_LVD_EN
	if(low_power_flag)
		return 0;
#endif

	return 1;
}

/*默认没有注册，在读取了配置文件后会调用。用户需要对地址做特殊处理的可以参照例子进行修改*/
static void bt_addr_deal(char *addr)
{
    /*地址指针，地址只有6个byte，注意越界访问会有问题, 例子*/
    puts("bt_addr_deal\n");
    addr[0] = 0x1A;
    addr[1] = 0x2A;
    addr[2] = 0x3A;
//    addr[3] = 0x1A;
//    addr[4] = 0x2A;
//    addr[5] = 0x3A;
}

static u8 bt_aec_priority()
{
	return (u8)TaskBtAecPrio;
}

//该函数主要是因为有一些宏的值可能会被用户的修改影响到，所以要通过接口获取。
///*特别注意返回值的范围*///
#define BT_TRIM_ALWAYS 1
#define BT_TRIM_ONCE   0
static u8 bt_get_macro_value(BT_STACK_MACRO type)
{
    //be careful :   MAXRO return value in the function must between 1-254;
    switch(type)
    {
        case BT_MACRO_AEC_SD_PRIO:
            return TaskBtAecSDPrio;
        case BT_MACRO_SBC_PRIO:
            return TaskSbcPrio;
        case BT_MACRO_BTLMP_PRIO:
            return TaskBtLmpPrio;
        case BT_MACRO_BTSTACK_PRIO:
            return TaskBtStackPrio;
		case BT_MACRO_BTESCO_PRIO:
			return TaskBtEscoPrio;
        case BT_MACRO_RESOURCE_PRIO:
            return TaskResoursrPrio;
        case BT_MACRO_MAX_SYS_VOL_L:
            return MAX_SYS_VOL_L;
        case BT_MACRO_MAX_SYS_VOL_R:
            return MAX_SYS_VOL_R;
        case BT_MACRO_SYSCFG_REMOTE_DB:
            return SYSCFG_REMOTE_DB;
        case BT_MACRO_SYSCFG_REMOTE_DB_HID:
            return SYSCFG_REMOTE_DB_HID;
        case BT_MACRO_MSG_LAST_WORKMOD:
            return MSG_LAST_WORKMOD;
        case BT_MACRO_SYS_EVENT_TASK_RUN_REQ:
            return SYS_EVENT_TASK_RUN_REQ;
        case BT_MACRO_MSG_BT_STACK_STATUS_CHANGE:
            return MSG_BT_STACK_STATUS_CHANGE;
        case BT_TRIM_MODE:
#if(BT_MODE == NORMAL_MODE)
            return BT_TRIM_ONCE;//BT_TRIM_ALWAYS
#else 
			return BT_TRIM_ALWAYS;
#endif
		case BT_ESCO_FILTER_LEVEL:
			return 0;
		case BT_MACRO_A2DP_AAC_EN:
			return 0;
       case BT_MACRO_STEREO_SYSCFG_REMOTE_DB:
            return SYSCFG_REMOTE_DB_STEREO;

        default:
            printf("check the lib,0x%x\n",type);
            return 0xff;
    }
}

static void low_pwr_dac_on_control()
{
	os_taskq_post(MAIN_TASK_NAME,1,MSG_DAC_ON);
}

static void low_pwr_dac_off_control()
{
	os_taskq_post(MAIN_TASK_NAME,1,MSG_DAC_OFF);
}

u32 pwr_timer_in(void)
{
	/* putchar('I'); */
	return 1;
}

u8 led_flag = 0;
u32 pwr_timer_out(void)
{
	/* putchar('O'); */
#if KEY_IO_EN
    io_key_init();
#endif



	return 1;
}

extern u8 get_esco_st(u8 sw);
/*
蓝牙库用到的一系列可设置参数配置，通过handle的形式。
这样用户就不用考虑调用配置函数的具体位置。需要改变可以注册，不需要设置有默认参数
*/
static void bredr_handle_register()
{
    register_edr_init_handle();
	aec_get_macro_handle_register( bt_aec_priority);    /*must set*/
    set_bt_info_callback(bt_function_select_init);/*相当于默认设置，要在协议栈初始化前设置*/
    get_battery_value_register(bt_get_battery_value);   /*电量显示获取电量的接口*/
    call_vol_change_handle_register(call_vol_change); /*通话过程中调节音量会有反馈的回调函数*/
    read_remote_name_handle_register(bt_read_remote_name);  /*连接成功后读取手机名字从该函数返回*/
    discon_complete_handle_register(bt_discon_complete_handle);/*断开或者连接上会调用的函数，给客户反馈信息*/

#if (BT_MODE != NORMAL_MODE)
	fcc_test_handle_register(fcc_uart_init,test_fcc_default_info);/*fcc测试函数注册*/
#endif
#if SUPPORT_APP_RCSP_EN
	spp_data_deal_handle_register(spp_rcsp_data_deal);    /*简单串口*/
#else
	spp_data_deal_handle_register(spp_data_deal);    /*简单串口*/
#endif

	hook_phone_number_register(hook_hfp_incoming_phone_number);   /*获取解释到电话号码返回*/
    phone_sync_sys_vol_handle_register(phone_sync_sys_dac_vol,phone_sync_phone_vol );  /*通话时同步系统音量*/
    music_vol_change_handle_register(set_device_volume, get_dac_vol );  /*同步手机音乐音量*/
    //bt_addr_spec_deal_handle_register(bt_addr_deal);   /*可以个性化设置地址*/
	bt_auto_conn_handle_register(bt_is_auto_conn);
    bt_low_pwr_handle_register(low_pwr_deal,low_pwr_dac_on_control,low_pwr_dac_off_control);
    bt_updata_run_handle_register(bt_updata_run);/*蓝牙无线升级*/
    bt_set_low_power(LOWPOWER_OSC_TYPE,LOWPOWER_OSC_HZ,BT_LOW_POWER_MODE,1,SYS_Hz/1000000L,check_enter_lowpower,NULL); /*蓝牙低功耗设置*/
	bt_chip_io_type_setting(RTCVDD_TYPE , BTAVDD_TYPE);
	bt_set_noconnect_lowpower_fun(1,pwr_timer_in,pwr_timer_out);/*设置没有连接的时候是否进入低功耗模式*/
    bt_set_noconnect_lowpower_led_fun(1);
	esco_handle_register(get_esco_st);
#if BT_STEREO	
	bt_stereo_register(bt_save_stereo_info,stereo_sys_vol_sync,stereo_deal_cmd);  /*对箱接口*/
#else
	bt_stereo_register(NULL,NULL,NULL);  /*对箱接口*/
#endif
}
static void ble_handle_register()
{
    /* ble_init_config(); */
    register_ble_init_handle();
}
static void stereo_handle_register()
{
  
	register_stereo_init_handle();
}
/*不属于用户接口，协议栈回调函数*/
extern void bt_osc_internal_cfg(u8 sel_l,u8 sel_r);
extern void app_server_init(void);
#if BT_STEREO
static u8 rx_mem[10*1024] sec(.bt_classic_data);
static u8 tx_mem[5*1024] sec(.bt_classic_data);
#else
static u8 rx_mem[5*1024] sec(.bt_classic_data);
static u8 tx_mem[3*1024] sec(.bt_classic_data);
#endif
void bt_app_cfg()
{
    u8 poweroff = 0;
    poweroff = bt_power_is_poweroff_post();
    cfg_test_box_and_work_mode(NON_TEST,BT_MODE);
	cfg_ble_work_mode(BT_MODE);
#if (BT_MODE == TEST_BQB_MODE)
	ble_RF_test_uart_para_init(UART2_HARDWARE_NAME,57600,UART_TXPC4_RXPC5);
#endif
	cfg_bt_pll_para(OSC_Hz,SYS_Hz,BT_ANALOG_CFG,BT_XOSC);
 	cfg_bredr_mem(rx_mem,sizeof(rx_mem),tx_mem,sizeof(tx_mem));
	/*internal capacitor range:0x00~0x1F*/
	bt_osc_internal_cfg(0x11,0x11);

    bt_get_macro_handle_register(bt_get_macro_value); /*must set*/

    __set_ble_bredr_mode(BLE_BREDR_MODE);    /*bt enble BT_BLE_EN|BT_BREDR_EN */

#if BT_STEREO
    __set_stereo_mode(1);
    stereo_handle_register();
#else
    __set_stereo_mode(0);
#endif // BT_STEREO

	set_bt_config_check_callback(bt_info_init);   //    bt_info_init();[>读取flash的配置文件<]
#if (BLE_BREDR_MODE&BT_BREDR_EN)
    bredr_handle_register();
#endif   //SUPPORT BREDR
#if (BLE_BREDR_MODE&BT_BLE_EN)
    ble_handle_register();
	app_server_init();
#endif   //SUPPORT BLE
}
