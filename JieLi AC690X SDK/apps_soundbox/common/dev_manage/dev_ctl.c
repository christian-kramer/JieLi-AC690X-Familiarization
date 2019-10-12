#include "sdk_cfg.h"
#include "dev_manage/dev_ctl.h"
#include "dev_manage/drv_manage.h"
#include "sdmmc/sd_host_api.h"
#include "dev_manage/mango_dev_cache.h"
#include "common/app_cfg.h"
#include "file_operate/logic_dev_list.h"
#include "linein/linein.h"
#include "usb_device/dev_pc.h"
#include "sys_detect.h"
#include "linein/dev_linein.h"
#include "irq_api.h"
#include "crc_api.h"
#include "update.h"
#include "key_drv/key_drv_ad.h"
#define UDISK_READ_PAGE    512

extern tbool syd_test(void *p_hdev);
extern const struct DEV_IO * dev_reg_usb(void *parm);
extern const struct DEV_IO * dev_reg_usb_mult(void *parm);
//extern OS_EVENT  sys_clk_chg_sem;

DEV_HANDLE volatile sd0;        ///>设备句柄
DEV_HANDLE volatile sd1;        ///>设备句柄
DEV_HANDLE volatile usb;        ///>设备句柄
DEV_HANDLE volatile cache;      ///>设备句柄

#if SDMMC0_EN
///sd0 io检测函数
u8 sd0_io_check(void)
{
    return TRUE;  //检测到
    return FALSE; //没检测到
}
#endif


#if SDMMC1_EN
///sd1 io检测函数
u8 sd1_io_check(void)
{
    return TRUE;  //检测到
    return FALSE; //没检测到
}
#endif

#if 0
	SD控制器配置
 	第0组SD卡SD0控制器A出口SD0_IO_A: 
参数配置: sd0_api_set.controller_io = SD0_IO_A;
IO口分配:(DAT/CMD/CLK): PA11/PA12/PA13(开发板上默认的SD Card0卡座)

 	第0组SD卡SD0控制器B出口SD0_IO_B:
参数配置:  sd0_api_set.controller_io = SD0_IO_B;
IO口分配: (DAT/CMD/CLK): PB10/PB11/PB12(开发板上要飞线)

 	第1组SD卡SD1控制器A出口SD1_IO_A:
参数配置:  sd1_api_set.controller_io = SD1_IO_A;
IO口分配: (DAT/CMD/CLK): PC3/PC4/PC5(开发板上要飞线，还注意将其他复合的IO口设为高阻)
		
 	第1组SD卡SD1控制器B出口SD1_IO_B:
参数配置:  sd1_api_set.controller_io = SD1_IO_B;
IO口分配: (DAT/CMD/CLK): PB0/PB1/PB2(开发板上默认的SD Card1卡座)

#endif
static void dev_reg_ctrl(void)
{

    u32 dev_parm=0;

#if SDMMC0_EN
    sSD_API_SET sd0_api_set;
    memset(&sd0_api_set, 0x0, sizeof(sSD_API_SET));
    sd0_api_set.controller_io = SD0_IO_B;        //SD0_IO_A：SD0控制器B出口，SD0_IO_B：SD0控制器B出口
    sd0_api_set.online_check_way = SD_CLK_CHECK; //CMD检测方式：SD_CMD_CHECK，CLK检测方式：SD_CLK_CHECK，IO检测方式：SD_IO_CHECK
    sd0_api_set.max_data_baud = 5;               //数据传输波特率(0为最快速度)
    sd0_api_set.hc_mode = SD_1WIRE_MODE;         //1线模式：SD_1WIRE_MODE，4线模式：SD_4WIRE_MODE，高速模式：SD_HI_SPEED
                                                    //--(SD_1WIRE_MODE|SD_HI_SPEED 表示1线高速模式)
    if(SD_IO_CHECK == sd0_api_set.online_check_way)
    {
        sd0_api_set.io_det_func = sd0_io_check;  //传入io检测函数
    }
#if USB_SD0_MULT_EN
	DEVICE_REG(sd0_mult,&sd0_api_set);                ///<注册sd0_usb_复用到系统
    sd0 = dev_open("sd0mult", 0);
#else
	DEVICE_REG(sd0,&sd0_api_set);                ///<注册sd0到系统
	sd0 = dev_open("sd0", 0);
#endif

#endif

#if SDMMC1_EN
    sSD_API_SET sd1_api_set;
    memset(&sd1_api_set, 0x0, sizeof(sSD_API_SET));

	sd1_api_set.controller_io = SD1_IO_B;        //SD1_IO_A：SD1控制器B出口，SD1_IO_B：SD1控制器B出口
	sd1_api_set.online_check_way = SD_CLK_CHECK; //CMD检测方式：SD_CMD_CHECK，CLK检测方式：SD_CLK_CHECK，IO检测方式：SD_IO_CHECK
    sd1_api_set.max_data_baud = 5;               //数据传输波特率(0为最快速度)
    sd1_api_set.hc_mode = SD_1WIRE_MODE;         //1线模式：SD_1WIRE_MODE，4线模式：SD_4WIRE_MODE，高速模式：SD_HI_SPEED
                                                    //--(SD_1WIRE_MODE|SD_HI_SPEED 表示1线高速模式)
    if(SD_IO_CHECK == sd1_api_set.online_check_way)
    {
        sd1_api_set.io_det_func = sd1_io_check;  //传入io检测函数
    }
#if USB_SD1_MULT_EN
	DEVICE_REG(sd1_mult,&sd1_api_set);                ///<注册sd0_usb_复用到系统
    sd1 = dev_open("sd1mult", 0);
#else
	DEVICE_REG(sd1,&sd1_api_set);                ///<注册sd1到系统
    sd1 = dev_open("sd1", 0);
#endif
#endif

#if USB_DISK_EN

#if(USB_SD0_MULT_EN == 1)||(USB_SD1_MULT_EN == 1)
	dev_parm = UDISK_READ_PAGE;
    DEVICE_REG(usb_mult, &dev_parm); ///<注册usb到系统
    usb = dev_open("usbmult", 0);
#else
	dev_parm = UDISK_READ_PAGE;
	DEVICE_REG(usb, &dev_parm); ///<注册usb到系统
	usb = dev_open("usb", 0);
#endif

#endif

    DEVICE_REG(cache, NULL);
    cache = dev_open("cache", 0);

#if(USB_SD0_MULT_EN == 1)||(USB_SD1_MULT_EN == 1)
	usb_sd_mult_init();
#endif
}



u32 dev_detect_fun(u32 info)
{
#if SD_DADA_MULT
	sd_data_multiplex();
#endif
	
#if AUX_DETECT_EN
#if AUX_IO_DET
    aux_check_api(info); //linein检测
#else
	aux_Ad_check_api(info);
#endif
#endif
#if PHONE_CHECK_EN
	phone_check_api(info);
#endif
#if AUX_PHONE_ADDET
	aux_phone_check_api(info);
#endif
#if(BT_MODE==NORMAL_MODE)
#if USB_PC_EN
    pc_check_api(info);  //PC检测
#endif
#endif

#if(USB_SD0_MULT_EN == 1)||(USB_SD1_MULT_EN == 1)
	usb_sd_detect_mult_api();
#endif
    return 0;
}

extern u32 lg_dev_list_init();
void dev_ctrl_sys_init(void *parm)
{
    ///逻辑设备链表初始化
    lg_dev_list_init();

    ///物理设备链表初始化
    phy_dev_list_init();

    ///注册设备(所有设备先在此处注册)
	dev_reg_ctrl();

    ///启动设备管理器线程
    dev_init(TaskDevCheckPrio,TaskDevMountPrio,(void*)dev_detect_fun);

//    os_mutex_create(&sys_clk_chg_sem);

}

u32 dev_updata_cfg(void)
{
    JL_AUDIO->DAC_CON = 0;
    irq_global_disable();
    irq_clear_all_ie();
    clr_PINR_ctl();
	led_update_start();
    return 1; //非软复位升级
}


