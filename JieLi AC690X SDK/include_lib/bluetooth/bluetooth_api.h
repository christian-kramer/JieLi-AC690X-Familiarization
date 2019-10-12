#ifndef _BLUETOOTH_API_H_
#define _BLUETOOTH_API_H_

#include "typedef.h"
#include "uart.h"
//#include "common/printf.h"

//#define UART_BT_LIB_DEG

#ifdef UART_BT_LIB_DEG
    #define bt_puts                 puts
    #define bt_put_u32d             put_u32d
    #define bt_put_u32hex0          put_u32hex0
    #define bt_put_u32hex           put_u32hex
    #define bt_printf               printf

    #define bt_printf_without_lock  printf_without_lock
    #define bt_puts_without_lock    puts_without_lock
    #define bt_put_u16hex           put_u16hex
    #define bt_put_u8hex0           put_u8hex0
    #define bt_put_u8hex            put_u8hex
    #define bt_put_buf              put_buf
    #define bt_printf_buf           printf_buf
    #define bt_putchar              putchar
     #define bt_malloc_stats             malloc_stats
#else
    #define bt_puts(...)
    #define bt_put_u32d(...)
    #define bt_put_u32hex0(...)
    #define bt_put_u32hex(...)
    #define bt_printf(...)

    #define bt_printf_without_lock(...)
    #define bt_puts_without_lock(...)
    #define bt_put_u16hex(...)
    #define bt_put_u8hex0(...)
    #define bt_put_u8hex(...)
    #define bt_put_buf(...)
    #define bt_printf_buf(...)
    #define bt_putchar(...)
    #define bt_malloc_stats()
#endif

//配置测试盒测试功能
#define NON_TEST         0          ///<没频偏和距离测试
#define FRE_OFFSET_TEST  BIT(0)     ///<频偏测试
#define DISTANCE_TEST    BIT(1)     ///<距离测试


#define NORMAL_MODE         0
#define TEST_BQB_MODE       1       ///<测试bqb认证
#define TEST_FCC_MODE       2       ///<测试fcc认证
#define TEST_FRE_OFF_MODE   3       ///<测试频偏(使用频谱分析仪-手提测试仪-中心频率默认2422M)
#define TEST_PERFOR_MODE    4       ///<指标性能测试(使用MT8852A仪器测试,测试芯片性能的时候使用)
#define TEST_BOX_MODE       5       ///<测试盒测试

//配置Low power mode
#define SNIFF_EN                            BIT(0)  ///<SNIFF使能
#define SNIFF_TOW_CONN_ENTER_POWERDOWN_EN   BIT(3)  ///<SNIFF 等待两台都连接才进powerdown
#define SNIFF_CNT_TIME                      10      ///<空闲10S之后进入sniff模式

//配置Low power mode
#define BT_POWER_OFF_EN                   BIT(1)  ///<SNIFF 进入poweroff
#define BT_POWER_DOWN_EN                  BIT(2)  ///<SNIFF 进入powerdown



#define BT_BREDR_EN  BIT(0)
#define BT_BLE_EN    BIT(1)

extern bool get_resume_flag();
extern bool is_bt_in_background_mode();
extern void delete_stack_task(void);
extern void no_background_suspend();
extern void background_suspend();
extern bool get_suspend_flag();
extern void hook_btstack_suspend();
extern void hook_btstack_resume(u8 suspend_flag);
extern bool get_auto_suspend_flag();
extern void clean_auto_suspend_flag();
extern void set_bt_config_check_callback(void (*fun)(void));
extern void set_bt_info_callback(void (*fun)(void));
extern void register_edr_init_handle();

extern void ble_register_init_config(void (*handler)(void));
extern void register_ble_init_handle();
extern void register_stereo_init_handle();

/*******************************************************************/
/*
 *-------------------LE READ PARAMETER
 */
void ble_controller_suspend(void);

void ble_controller_resume(void);


#endif
