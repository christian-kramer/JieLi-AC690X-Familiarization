#ifndef _IIS_H_
#define _IIS_H_

#include "timer.h"
#include "common/msg.h"
#include "rtos/os_api.h"

//#define IIS_DEBUG
#ifdef IIS_DEBUG
    #define iis_puts(x)    puts(x)
#else
    #define iis_puts(...)
#endif

#define IIS_USED_PORT           1   ///>0:PORTA   1:PORTB

#define ALINK_ISR_PRIORITY      3   ///> alink 中断优先级

/*----IIS_FUN----*/

#define IIS_CHANNEL0_EN    0
#define IIS_CHANNEL1_EN    1
#define IIS_CHANNEL2_EN    0
#define IIS_CHANNEL3_EN    0

#define IIS_SEND_MODE       0   ///>IIS发送模式
#define IIS_RECEIVE_MODE    1   ///>IIS接受模式

/*----IIS_CHANNEL----*/
#define IIS_CHANNEL0    0
#define IIS_CHANNEL1    4
#define IIS_CHANNEL2    8
#define IIS_CHANNEL3    12

/*----ALINK_SFR----*/
#define MCKD_EXTERNAL()     JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<0))|(0x00<<0))
#define MCKD_SYSCLK()       JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<0))|(0x01<<0))
#define MCKD_OS_CLK()       JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<0))|(0x02<<0))
#define MCKD_PLL_ALNK_CLK() JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<0))|(0x03<<0))

#define MCKD_1DIV()       JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<2))|(0x00<<2))
#define MCKD_2DIV()       JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<2))|(0x01<<2))
#define MCKD_4DIV()       JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<2))|(0x02<<2))
#define MCKD_8DIV()       JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<2))|(0x03<<2))

#define LRCLK_EXTERNAL()  JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<4))|(0x00<<4))
#define LRCLK_256FS()     JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<4))|(0x01<<4))
#define LRCLK_384FS()     JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<4))|(0x02<<4))
#define LRCLK_512FS()     JL_IIS->CON0 = ((JL_IIS->CON0&=~(0x03<<4))|(0x03<<4))

#define DSP_EN(x)           JL_IIS->CON0 = ((JL_IIS->CON0 &=~(BIT(6)))|((x & 0x01)<<6))
#define SCLK_OUT_EN(x)      JL_IIS->CON0 = ((JL_IIS->CON0 &=~(BIT(7)))|((x & 0x01)<<7))
#define MCLK_OUT_EN(x)      JL_IIS->CON0 = ((JL_IIS->CON0 &=~(BIT(8)))|((x & 0x01)<<8))

#define F32_EN(x)           JL_IIS->CON0 = ((JL_IIS->CON0 &=~(BIT(9)))|((x & 0x01)<<9))
#define SCLKINV(x)          JL_IIS->CON0 = ((JL_IIS->CON0 &=~(BIT(10)))|((x & 0x01)<<10))
#define ALINK_EN(x)         JL_IIS->CON0 = ((JL_IIS->CON0 &=~(BIT(11)))|((x & 0x01)<<11))

enum
{
    MCLK_11M2896K = 0,
    MCLK_12M288K
};
enum
{
	IIS_CHANNEL0_SEL=0,
	IIS_CHANNEL1_SEL,
	IIS_CHANNEL2_SEL,
	IIS_CHANNEL3_SEL,
};

void audio_link_init(void);
void * get_iis_addr(u8 channel_sel);

#endif
