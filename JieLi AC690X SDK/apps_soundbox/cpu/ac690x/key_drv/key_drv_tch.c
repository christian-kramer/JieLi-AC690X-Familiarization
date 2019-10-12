/*******************************************************************************************
File Name: 	key_drv_tch.c

Version: 	1.00

Discription AC5100 Toutch Key API

Author:     bingquan Cai

Email:      bingquan_cai@zh-jieli.com

Date:		2013.05.09

Copyright (c) 2010 - 2012 JieLi. All Rights Reserved.
*******************************************************************************************/
#include "key_drv/key_drv_tch.h"
#include "includes.h"
#include "key_drv/key.h"
#include "cpu/timer_api.h"
#include "timer.h"
#include "irq_api.h"
#include "clock_api.h"


#if KEY_TCH_EN

///******使用说明*******///
//1.通道选择
//修改 key_drv_tch.h文件里的宏即可，现选择两个通道 PA8与PA7如下如示,通道数量不用设置
//#define PITCH_ON_TCH_IO  (TCH_PA8|TCH_PA7)
//
//2.返回值设置
//PA7 与 PA8 分别是 14通道和15通道，它们表示着第0个key,第1个key
//在 tu8 get_tchkey_value(void)函数里修改成这样即可
//        case PA7_CHANNEL:
//            return 0;
//        case PA8_CHANNEL:
//            return 1;
//
//        case PB0_CHANNEL:
//        case PB1_CHANNEL:
//        case PB2_CHANNEL:
//        case PB3_CHANNEL:
//        case PB4_CHANNEL:
//        case PB10_CHANNEL:
//        case PB11_CHANNEL:
//        case PB12_CHANNEL:
//        case PA1_CHANNEL:
//        case PA2_CHANNEL:
//        case PA3_CHANNEL:
//        case PA4_CHANNEL:
//        case PA5_CHANNEL:
//        case PA6_CHANNEL:
//            return NO_KEY;
//        default:
//            return NO_KEY;


///CTM模块
///*////////////////////////////////////////////////////////
///*////////////////////////////////////////////////////////

sCTM_KEY_VAR ctm_key_value;
sCTM_KEY_VAR *ctm_key_var;


static void ctm_key_var_init(sCTM_KEY_VAR *ptr)
{
    ctm_key_var = ptr;
}


tu8 get_tchkey_value(void)
{
    u8 key;
    u8 i;

    for (i = 0; i < TOUCH_KEY_CH_MAX; i++)
    {
        if (!(ctm_key_value.touch_key_state & (BIT(i))))
            break;
    }
    key = (i < TOUCH_KEY_CH_MAX) ? i : NO_KEY;

    //以上返回的是通道号，下面要把通道号转换成 key号(第几个按键)
    switch(key)
    {
        case PA7_CHANNEL:
            //puts("key 0\n");
            return 0;
        case PA8_CHANNEL:
            //puts("key 1\n");
            return 1;
        case PB4_CHANNEL:
             //puts("key 2\n");
            return 2;

        case PB0_CHANNEL:
        case PB1_CHANNEL:
        case PB2_CHANNEL:
        case PB3_CHANNEL:
        case PB10_CHANNEL:
        case PB11_CHANNEL:
        case PB12_CHANNEL:
        case PA1_CHANNEL:
        case PA2_CHANNEL:
        case PA3_CHANNEL:
        case PA4_CHANNEL:
        case PA5_CHANNEL:
        case PA6_CHANNEL:
            return NO_KEY;
        default:
            return NO_KEY;
    }
}


static void ctm_irq(u8 channel)
{
    u16 temp_u16_0, temp_u16_1,ctm_res_value;
    s16 temp_s16_0, temp_s16_1;
    s32 temp_s32_0;
//..............................................................................................
//取计数值/通道判断
//..............................................................................................

    ctm_res_value = CTM_RES;

//    if(channel==15)
//    {
//       put_u16hex(ctm_res_value);
//    }
    //put_u8hex(channel);

    if(ctm_key_var->touch_init_cnt[channel])
    {
        ctm_key_var->touch_init_cnt[channel]--;
//		touch_cnt_buf[channel] = rvalue << FLT0CFG;
//		touch_release_buf[channel] = (long)(rvalue) << FLT1CFG0;
        ctm_key_var->touch_cnt_buf[channel] = (u32)ctm_res_value<<ctm_key_var->FLT0CFG;
        ctm_key_var->touch_release_buf[channel] = (u32)ctm_res_value<<ctm_key_var->FLT1CFG0;
    }

//..............................................................................................
//当前计数值去抖动滤波器
//..............................................................................................
    temp_u16_0 = ctm_key_var->touch_cnt_buf[channel];
    temp_u16_1 = temp_u16_0;
    temp_u16_1 -= (temp_u16_1 >> ctm_key_var->FLT0CFG);
    temp_u16_1 += ctm_res_value;//temp_u16_1 += rvalue;
    ctm_key_var->touch_cnt_buf[channel] = temp_u16_1;
    temp_u16_0 += temp_u16_1;
    temp_u16_0 >>= (ctm_key_var->FLT0CFG + 1);


//..............................................................................................
//各通道按键释放计数值滤波器
//..............................................................................................
    temp_s32_0 = ctm_key_var->touch_release_buf[channel];
    temp_u16_1 = temp_s32_0 >> ctm_key_var->FLT1CFG0;	//获得滤波器之后的按键释放值
    temp_s16_0 = temp_u16_0 - temp_u16_1;	//获得和本次检测值的差值，按下按键为负值，释放按键为正值
    temp_s16_1 = temp_s16_0;

//    	if(channel == 0)
//    	{
//    		printf("ch%d: %d  %d", (short)ch, temp_u16_0, temp_s16_1);
//    	}

    if(ctm_key_var->touch_key_state & BIT(channel))		//如果本通道按键目前是处于释放状态
    {
        if(temp_s16_1 >= 0)		//当前计数值大于低通值，放大后参与运算
        {
            if(temp_s16_1 < (ctm_key_var->FLT1CFG2 >> 3))
            {
                temp_s16_1 <<= 3;	//放大后参与运算
            }
            else
            {
                temp_s16_1 = ctm_key_var->FLT1CFG2;	//饱和，防止某些较大的正偏差导致错判
            }
        }
        else if(temp_s16_1 >= ctm_key_var->FLT1CFG1)	//当前计数值小于低通值不多，正常参与运算
        {
        }
        else				//当前计数值小于低通值很多，缩小后参与运算 (有符号数右移自动扩展符号位???)
        {
            temp_s16_1 >>= 3;
        }
    }
    else			//如果本通道按键目前是处于按下状态, 缓慢降低释放计数值
    {
        if(temp_s16_1 <= ctm_key_var->RELEASECFG1)
        {
            temp_s16_1 >>= 3;		//缩小后参与运算
        }
        else
        {
            temp_s16_1 = 0;
        }
    }

    temp_s32_0 += (s32)temp_s16_1;
    ctm_key_var->touch_release_buf[channel] = temp_s32_0;

//..............................................................................................
//按键按下与释放检测
//..............................................................................................
    if(temp_s16_0 <= ctm_key_var->PRESSCFG)				//按键按下
    {
//        printf("P=%d\n",(0 - temp_s16_0));
        ctm_key_var->touch_key_state &= ~BIT(channel);
    }
    else if(temp_s16_0 >= ctm_key_var->RELEASECFG0)		//按键释放
    {
        ctm_key_var->touch_key_state |= BIT(channel);
    }
}

//触摸中断
void ctm_irq_isr(void)
{
    u8 channel;
    CTM_CPND(1);
    channel = CTM_CH();
    ctm_irq(channel);
}

IRQ_REGISTER(IRQ_CTM_IDX,ctm_irq_isr);



/** \brief
 *
 * \param  ch 选中的channel
 * \return 无
 *
 */
void tch_key_init(void)
{
	u16 ch = PITCH_ON_TCH_IO;

    ctm_key_var_init(&ctm_key_value);

    my_memset((u8 *)&ctm_key_value, 0x0, sizeof(sCTM_KEY_VAR));
    /*触摸按键参数配置*/
    ctm_key_value.FLT0CFG = 1;
    ctm_key_value.FLT1CFG0 = 7;
    ctm_key_value.FLT1CFG1 = -80;
    ctm_key_value.FLT1CFG2 = (-(-10)) << 7; //1280

    ///调节灵敏度的主要参数
    ctm_key_value.PRESSCFG = -50;    //按下灵敏度
    ctm_key_value.RELEASECFG0 = -10; //释放灵敏度0
    ctm_key_value.RELEASECFG1 = -100;//释放灵敏度1

    my_memset((u8 *)&(ctm_key_value.touch_init_cnt[0]), 0x10, TOUCH_KEY_CH_MAX);//计数16次

    ctm_key_value.touch_key_state = 0xffff; //<按键默认释放

    //PB口设置 th0~th4  PB0~PB4 5个通道 th5~th7 PB10~PB12 3个通道
    JL_PORTB->DIR |=   (ch&0x001f)|((ch&0x00e0)<<5);
    JL_PORTB->PU  &= ~((ch&0x001f)|((ch&0x00e0)<<5));
    JL_PORTB->PD  &= ~((ch&0x001f)|((ch&0x00e0)<<5));
    JL_PORTB->DIE &= ~((ch&0x001f)|((ch&0x00e0)<<5));

    //PA口设置  th8~th15 PA1~PA8
    JL_PORTA->DIR |=   (ch&0xff00)>>7;
    JL_PORTA->PU  &= ~((ch&0xff00)>>7);
    JL_PORTA->PD  &= ~((ch&0xff00)>>7);
    JL_PORTA->DIE &= ~((ch&0xff00)>>7);

    CTM_CH_EN(ch);//打开对应通道


    /*振荡器电流选择，根据电容配置，工作频率保持在1MHz*/
    CTM_ISEL(5);


    //一般来说有两档 2.5ms 及 1.25ms
    //则每个通道的采样间隔时间分别为（不使用的通道也要占用时间）
    //2.5*16=40ms  1秒25次
    //1.25*16=20ms 1秒50次
    //不需快速响应推荐使用2.5ms ，需要快速响应的推荐1.25ms

    //用PLL8M作时钟源 8M 8000000
    //闸门时间 = GTSEL选中驱动时钟周期 * PSEL分频倍数N * （CTM_PRD+1）
    //*闸门时间 = 1/8000000 * (155 + 1) * 128*/


    CTM_GTSEL(3);//选中PLL8M时钟 0011

    //1.25ms
    //CTM_PRD(77);
    //CTM_PSEL(0x7);

    //2.5ms
    CTM_PRD(155);
    CTM_PSEL(0x7);

    CTM_EN(1);   //模块使能
    CTM_CPND(1); //清pending
    CTM_IE(1);   //中断使能

    printf("CTM_CON0 : %04x\n", CTM_CON0);
    printf("CTM_CON1 : %04x\n", CTM_CON1);

    //注册触摸中断
    IRQ_REQUEST(IRQ_CTM_IDX,ctm_irq_isr,3);

}

///CTM模块
///*////////////////////////////////////////////////////////
///*////////////////////////////////////////////////////////
#endif
