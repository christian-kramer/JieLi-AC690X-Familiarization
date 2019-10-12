/*********************************************************************************************
    *   Filename        : sdk_cfg.h

    *   Description     : Config for Sound Box Case

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-12-01 15:36

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef _CONFIG_
#define _CONFIG_

#include "includes.h"


///任务堆栈大小配置测试
#define VM_TASK_STACK_SIZE          (1024 * 1)
#define MAIN_TASK_STACK_SIZE        (1024 * 2) //(1024 * 1)可行
#define MUSIC_TASK_STACK_SIZE       (1024 * 4)
#define MUSIC_PHY_TASK_STACK_SIZE   (1024 * 4)
#define TONE_TASK_STACK_SIZE        (1024 * 4)
#define TONE_PHY_TASK_STACK_SIZE    (1024 * 2)
#define UI_TASK_STACK_SIZE          (1024 * 2)

//配置对箱
///< 蓝牙对箱使能,对箱暂时不开放插sd\插u.fm,rtc.ble,后台和app,只支持linein.
#define    BT_STEREO                     0   
///对箱从机开机是否进入被连接还是回链状态
#define    STEREO_SLAVE_WAIT_CON         1  
///对箱主从是否一起关机
#define    BT_STEREO_POWEROFF_TOGETHER   0 

/********************************************************************************/
/*
 *          --------调试类配置
 */
////<开启系统打印调试功能
 //#define __DEBUG         

///<LED指示使能
#define LED_EN                  1       
///<串口升级
#define UART_UPDATA_EN          0      
///<电量监测
#define SYS_LVD_EN              1       

#if (BT_STEREO == 1)
   #define BT_KTV_EN			0
#else
///<蓝牙K歌宝
#define BT_KTV_EN				0
#endif
///<啸叫抑制
#define HOWLING_SUPPRESSION_EN	0

/********************************************************************************/

/********************************************************************************/
/*
 *           --------手机app控制 配置 
 */
#if (BT_STEREO == 1)
   #define SUPPORT_APP_RCSP_EN    0 
   #define RCSP_LIGHT_HW	      0	
#else
///配置是否支持手机app功能，  (1-支持,0-不支持)
   #define SUPPORT_APP_RCSP_EN   0 
///配置是否支持蓝牙灯硬件控制 (1-支持,0-不支持)
   #define RCSP_LIGHT_HW	     0	
#endif

/********************************************************************************/

/********************************************************************************/
/*
 *           --------电源类配置
 */
///   0:  no change   
#define    PWR_NO_CHANGE        0     
///   1:  LDOIN 5v -> VDDIO 3.3v -> DVDD 1.2v
#define    PWR_LDO33            1     
///   2:  LDOIN 5v -> LDO   1.5v -> DVDD 1.2v, support bluetooth
#define    PWR_LDO15            2     
///   3:  LDOIN 5v -> DCDC  1.5v -> DVDD 1.2v, support bluetooth
#define    PWR_DCDC15           3     

///可选配置：PWR_NO_CHANGE/PWR_LDO33/PWR_LDO15/PWR_DCDC15
#define PWR_MODE_SELECT         PWR_LDO15

///充电是否关机
#define CHARGE_POWER_OFF 	    0               
///蓝牙无连接自动关机计时，u16类型，0表示不自动关机
#define AUTO_SHUT_DOWN_TIME     1200               

///<按键双击功能
#define KEY_DOUBLE_CLICK        1  
/********************************************************************************/

/********************************************************************************/
/*
 *           --------音效类配置
 */
///<EQ模块开关
#define EQ_EN			        0  
//软件EQ,置1开启，置0关闭，对系统时钟要求高，一般要192M，
//开启后不兼容以前的eq文件，用新的EQ工具，注意eq文件名改为"cfg_eq.bin"
//由于软件eq速度要求高，不支持无损格式，不支持对箱
#if (BT_STEREO == 1)
	#define USE_SOFTWARE_EQ   		0   
#else
	#define USE_SOFTWARE_EQ   		0   
#endif
///<EQ uart online debug
#define EQ_UART_BDG	    		0       
///<dac声道合并
#define DAC_SOUNDTRACK_COMPOUND 0       
///<自动mute
#if BT_KTV_EN
	#define DAC_AUTO_MUTE_EN		0       
#else
	#define DAC_AUTO_MUTE_EN		0       
#endif
///<按键音
#define KEY_TONE_EN     	    1       
///<非0表示使用默认音量
#define SYS_DEFAULT_VOL         23
///<人声消除
#define VOCAL_REMOVER			0


/********************************************************************************/


/********************************************************************************/
/*
 *           --------DAC VCOMO 配置
 */
///是否选择VCMO直推耳机
#define VCOMO_EN 	            0	
///可选配置：DAC_L_R_CHANNEL/DAC_L_CHANNEL/DAC_R_CHANNEL  
#define DAC_CHANNEL_SLECT       DAC_L_R_CHANNEL

/********************************************************************************/

/********************************************************************************/
/*
 *           --------外设类配置
 */
 #if (BT_STEREO == 1)
	#define SDMMC0_EN           0
	#define SDMMC1_EN           0
	#define USB_DISK_EN         0
	#define USB_PC_EN           0
#else
    #define SDMMC0_EN           1
	#define SDMMC1_EN           0
	#define USB_DISK_EN         1
	#define USB_PC_EN           1
#endif

//usb_sd引脚复用，需要测试兼容性
#define USB_SD0_MULT_EN     0	//<需要测试兼容性
#define USB_SD1_MULT_EN		0	//<需要测试兼容性

#if(USB_SD0_MULT_EN == 1)||(USB_SD1_MULT_EN == 1)
	#undef USB_PC_EN
	#define USB_PC_EN       0
#endif

/********************************************************************************/

/********************************************************************************/
/*
 *           --------蓝牙类配置
 */
#include "bluetooth/bluetooth_api.h"

#define NFC_EN          0  ///<NFC ENABLE

///可选配置：NORMAL_MODE/TEST_BQB_MODE/TEST_FCC_MODE/TEST_FRE_OFF_MODE/TEST_BOX_MODE
#define BT_MODE             NORMAL_MODE

#if (BT_MODE == TEST_FCC_MODE)
  #undef UART_UPDATA_EN
  #define UART_UPDATA_EN    0
#endif

///模拟配置
#define BT_ANALOG_CFG       0
#define BT_XOSC             0//

///<蓝牙后台
#if (SUPPORT_APP_RCSP_EN == 1)
#define BT_BACKMODE         1             
#else
#define BT_BACKMODE         0             
#endif
///dependency
#if (BT_BACKMODE == 0)
    ///<HID拍照的独立模式只支持非后台
    #define BT_HID_INDEPENDENT_MODE  0    
#endif

///<来电报号
#define BT_PHONE_NUMBER     1          


/*
 *           --------蓝牙低功耗设置 
 */
///可选配置：SNIFF_EN/SNIFF_TOW_CONN_ENTER_POWERDOWN_EN
#define SNIFF_MODE_CONF     0//SNIFF_EN 

///可选配置：BT_POWER_DOWN_EN/BT_POWER_OFF_EN
#define BT_LOW_POWER_MODE   0//BT_POWER_DOWN_EN

#define BT_OSC              0
#define RTC_OSCH            1
#define RTC_OSCL            2

///可选配置：BT_OSC/RTC_OSCH/RTC_OSCL
#define LOWPOWER_OSC_TYPE   BT_OSC

///可选配置：32768L//24000000L
#define LOWPOWER_OSC_HZ     24000000L



///可选配置：BT_BREDR_EN/BT_BLE_EN/(BT_BREDR_EN|BT_BLE_EN)
#if (BT_STEREO == 1)
	#define BLE_BREDR_MODE      (BT_BREDR_EN)
#elif (BT_KTV_EN == 1)
	#define BLE_BREDR_MODE      (BT_BREDR_EN)
#else
    #define BLE_BREDR_MODE      (BT_BREDR_EN)//(BT_BREDR_EN|BT_BLE_EN)
#endif 


/********************************************************************************/

/********************************************************************************/
/*
 *           --------芯片封装配置 
 */
///RTCVDD口没有绑出来要置1,目前对应封装芯片AC6905
#define RTCVDD_TYPE              0          
#define BTAVDD_TYPE              0          

/********************************************************************************/


/********************************************************************************/
/*
 *           --------MUSIC MACRO
 */
//SMP加密文件支持
#define MUSIC_DECRYPT_EN 		0	
//AB断点支持
#define MUSIC_AB_RPT_EN 		0	

///<MP3
#define DEC_TYPE_MP3_ENABLE     1
///<SBC
#define DEC_TYPE_SBC_ENABLE     1
///<AAC
#define DEC_TYPE_AAC_ENABLE		0

///<3K_code_space
#define DEC_TYPE_WAV_ENABLE     1
///<5K_code_space
#define DEC_TYPE_FLAC_ENABLE    1
///<8K_code_space
#define DEC_TYPE_APE_ENABLE     1
///<30K_code_space
#define DEC_TYPE_WMA_ENABLE     1
///<30K_code_space
#define DEC_TYPE_F1A_ENABLE     0

/********************************************************************************/


/********************************************************************************/
/*
 *           --------FM MACRO
 */
///<标准SDK
#if (BT_STEREO == 1)
	#define FM_RADIO_EN         0 
#else
    #define FM_RADIO_EN         1
#endif 
      
///dependency
#if (FM_RADIO_EN == 1)
    ///<RDA5807FM
    #define RDA5807                 0       
    ///<BK1080FM
    #define BK1080                  0       
    ///<QN8035FM
    #define QN8035                  0       
    ///<芯片内部FM
    #define FM_INSIDE               1       
#endif

/********************************************************************************/

/********************************************************************************/
/*
 *           --------RTC MACRO
 */
///<标准SDK RTC时钟模式
 #if (BT_STEREO == 1)
    #define RTC_CLK_EN          0       
#else
    #define RTC_CLK_EN          0       
#endif
///dependency
#if (RTC_CLK_EN == 1)
    ///<RTC闹钟模式
    #define RTC_ALM_EN          1       
#endif

/********************************************************************************/

/********************************************************************************/
/*
 *           --------ECHO MACRO
 *			开启混响，必须关闭	DAC_AUTO_MUTE_EN
 */
///dependency
#if (BT_BACKMODE == 1)
    ///<不支持与蓝牙后台开启混响
    #define ECHO_EN             0       
    ///<混响模式 标准SDK
    #define ECHO_TASK_EN        0       
#else
	#if BT_KTV_EN
    	///<混响功能 KTV_SDK
    	#define ECHO_EN             1
	#else
    	///<混响功能 标准SDK
    	#define ECHO_EN             0
	#endif

    ///<混响模式 标准SDK
    #define ECHO_TASK_EN        0       
#endif

/********************************************************************************/

/********************************************************************************/
/*
 *           --------REC MACRO
 */
///dependency
#if (BT_BACKMODE == 1)
    ///<不支持与蓝牙后台开启录音
    #define REC_EN             0    
#else
    ///<标准SDK录音功能
    #define REC_EN             0    
#endif

#if (REC_EN == 1)
    ///<蓝牙录音使能
	#define BT_REC_EN		1       
    ///<MIC录音使能
	#define MIC_REC_EN		1       
    ///<FM录音使能
	#define FM_REC_EN		1       
    ///<AUX录音使能
	#define AUX_REC_EN		1       
#endif
/*******************************zhgq2016-10-31**********************************/
#define PA_EN                   1    ///<功放mute控制
#if PA_EN
#define UMUTE()             	JL_PORTC->DIR &= ~BIT(4);JL_PORTC->OUT &= ~BIT(4);
#define MUTE()            JL_PORTC->DIR &= ~BIT(4);JL_PORTC->OUT |= BIT(4);
#endif

#define AB_D_EN                 1    //功放ABD类使能
#if AB_D_EN
#define ABD_IO_INIT()   {JL_PORTC->PU &= ~BIT(5);JL_PORTC->PD &= ~BIT(5);JL_PORTC->DIR &= ~BIT(5);}
#define ABD_HIGH()		JL_PORTC->OUT |= BIT(5);
#define ABD_LOW()		JL_PORTC->OUT &= ~BIT(5);
#endif

#define JL_SDK                 0       ///杰里按键
#define PHONE_CHECK_EN          0     //耳机检测1有效
#define AUX_IO_DET              0     //AUX IO  DET
#define AUX_AD_DET              1     //AD检测AUX
#define AUX_PHONE_ADDET         0     //AUX/PHONE  adk检测
//#define POWER_VOICE                            //开机提示音
#define MUSIC_USB_SD_VOICE         //MUSIC 模式SD_USB  提示音使能
//#define SYS_POWER_KEY_EN      ///软开关机
#define RESTRICT_NOTICE_VOL

#define  MAX_VOL_VOICE          1     //最大音量提示音
#define MUSIC_LED_PLAYONE_FLASH
/*****************SD0  复合外挂收音FM  ***********************/
//#define SD0_IIC_MULT_EN

//#define ADK_VOL_EN    ///电位器调音量
#ifdef ADK_VOL_EN
#define mainVOL_step			8
#endif

//#define CODER_WHELL ///编码器调节
#ifdef CODER_WHELL
#define     CODER_INIT()        JL_PORTC->DIE|=(BIT(10)|BIT(11));JL_PORTC->DIR|=(BIT(10)|BIT(11));JL_PORTC->PU|=(BIT(10)|BIT(11))
#define     CODER_READ_LEFT()   (JL_PORTC->&BIT(10))
#define     CODER_READ_RIGHT()  (JL_PORTC->&BIT(11))
#endif

//#define MEMERY_MODE	//MODE记忆功能
#ifdef MEMERY_MODE
#define  BLUE_MODE     0
#define  MUSIC_MODE    1
#if FM_RADIO_EN
#define  FM_MODE       2
#endif
#define  AUX_MODE      3
#define  PC_MODE       4
#endif
/********************************************************************************/

/********************************************************************************/
/*
 *           --------UI MACRO
 */
///<UI_显示
#if (RCSP_LIGHT_HW == 1)
#define UI_ENABLE                0     
#else
#define UI_ENABLE                0     
#endif
///dependency
#if (UI_ENABLE == 1)
    #define LCD_128X64_EN        0      ///<lcd 支持
    #define LED_7_EN             0      ///<led 支持
	#define LCD_SEG_EN			 0	///<LCD支持
#else
    ///都不支持
    #define LCD_128X64_EN        0
    #define LED_7_EN             0
#endif
#if ((LCD_128X64_EN == 1) && (LED_7_EN == 1)&&(LCD_SEG_EN == 1))
#error  "UI driver support only one"
#endif

#if (LCD_128X64_EN == 1)
    ///菜单显示
    #define LCD_SUPPORT_MENU     1       
    ///LRC歌词显示
    #define LRC_LYRICS_EN        1       
#else
    #define LCD_SUPPORT_MENU     0
    #define LRC_LYRICS_EN        0
#endif

/********************************************************************************/

 #if (BT_STEREO == 1)
///<蓝牙对箱使能,对箱暂时不开放插sd\插u.fm,rtc.ble,后台和app.
  #if ((SDMMC0_EN) || (SDMMC0_EN)||(USB_DISK_EN)||(USB_PC_EN)||(BT_KTV_EN)||(FM_RADIO_EN)||(SUPPORT_APP_RCSP_EN)||(RTC_CLK_EN)||(BLE_BREDR_MODE&BT_BLE_EN))
  #error  "-------stereo not support this config ,please check sdk_cfg.h------------"
  #endif
#endif

#endif
