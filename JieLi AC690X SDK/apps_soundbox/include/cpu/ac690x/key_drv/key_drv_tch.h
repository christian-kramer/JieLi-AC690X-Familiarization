#ifndef _KEY_DRV_TCH_H_	
#define _KEY_DRV_TCH_H_

#include "includes.h"
#include "key_drv/key.h"

#if KEY_TCH_EN

//通道选择
#define TCH_PB0      BIT(0)
#define TCH_PB1      BIT(1)
#define TCH_PB2      BIT(2)
#define TCH_PB3      BIT(3)
#define TCH_PB4      BIT(4)
#define TCH_PB10     BIT(5)
#define TCH_PB11     BIT(6)
#define TCH_PB12     BIT(7)
#define TCH_PA1      BIT(8)
#define TCH_PA2      BIT(9)
#define TCH_PA3      BIT(10)
#define TCH_PA4      BIT(11)
#define TCH_PA5      BIT(12)
#define TCH_PA6      BIT(13)
#define TCH_PA7      BIT(14)
#define TCH_PA8      BIT(15)

///*****选择的IO口********///
#define PITCH_ON_TCH_IO  (TCH_PA8|TCH_PA7|TCH_PB4)



//返回的通道值
#define PB0_CHANNEL  0
#define PB1_CHANNEL  1
#define PB2_CHANNEL  2
#define PB3_CHANNEL  3
#define PB4_CHANNEL  4
#define PB10_CHANNEL  5
#define PB11_CHANNEL  6
#define PB12_CHANNEL  7
#define PA1_CHANNEL  8
#define PA2_CHANNEL  9
#define PA3_CHANNEL  10
#define PA4_CHANNEL  11
#define PA5_CHANNEL  12
#define PA6_CHANNEL  13
#define PA7_CHANNEL  14
#define PA8_CHANNEL  15


/*---CTM_CON0 sfr Setting----*/
#define CTM_EN(x)           CTM_CON0 = ((CTM_CON0 & ~BIT(0)) | (x & 0x1))
#define CTM_IE(x)           CTM_CON0 = ((CTM_CON0 & ~BIT(1)) | ((x & 0x1)<<1))
#define CTM_GTSEL(x)        CTM_CON0 = ((CTM_CON0 & ~(BIT(2)|BIT(3)) | ((x & 0x3)<<2)))
#define CTM_PSEL(x)         CTM_CON0 = ((CTM_CON0 & ~(BIT(4)|BIT(5)|BIT(6)|BIT(7))) | ((x & 0xf)<<4))
#define CTM_PRD(x)          CTM_CON0 = ((CTM_CON0 & 0x00ff) | ((x & 0xff)<<8))

/*---CTM_CON1 sfr Setting----*/
#define CTM_CH()           (CTM_CON1 & (BIT(0)|BIT(1)|BIT(2)|BIT(3)))
#define CTM_ISEL(x)         CTM_CON1 = ((CTM_CON1 & ~(BIT(8)|BIT(9)|BIT(10))) | ((x & 0x7)<<8))
#define CTM_CPND(x)         CTM_CON1 |= BIT(6)
#define CTM_CH_EN(x)        CTM_CON1 = ((CTM_CON1 & 0x0000ffff) | (x & 0xffff)<<16)


//这个不能改变
#define TOUCH_KEY_CH_MAX	16

typedef struct _CTM_KEY_VAR
{
    s32 touch_release_buf[TOUCH_KEY_CH_MAX]; 		//按键释放值滤波器buffer
    u16 touch_cnt_buf[TOUCH_KEY_CH_MAX];			//按键计数值滤波器buffer
    s16 FLT1CFG1;					//滤波器1配置参数1
    s16 FLT1CFG2;					//滤波器1配置参数2, 等于(-RELEASECFG0)<<FLT1CFG0
    s16 PRESSCFG;					//按下判决门限
    s16 RELEASECFG0;				//释放判决门限0
    s16 RELEASECFG1;				//释放判决门限1
    s8  FLT0CFG;					//滤波器0配置参数(0/1/2/3)
    s8  FLT1CFG0;					//滤波器1配置参数0
    u16 touch_key_state;			//按键状态标志，随时可能被中断改写，按键处理程序需要将此标志复制出来再行处理
    u8  touch_init_cnt[TOUCH_KEY_CH_MAX];				//初始化计数器，非0时进行初始化
} sCTM_KEY_VAR;

tu8 get_tchkey_value(void);
void tch_key_init(void);


#endif

#endif  /*  _TOUCH_KEY_API_H_  */


