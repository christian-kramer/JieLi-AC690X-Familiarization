#include "key_drv/key_drv_ir.h"
#include "includes.h"
#include "timer.h"
#include "key_drv/key.h"
#include "clock.h"

#if KEY_IR_EN


#define IR_FF00	 0xff00
IR_CODE  ir_code;       ///<红外遥控信息
u16 timer1_pad;
__timer_handle *timer1_hl;



/*user code is 0xFF00*/
const u8 IRTabFF00[] =
{
    NKEY_00, NKEY_01, NKEY_02, NKEY_03, NKEY_04, NKEY_05, NKEY_06, IR_06, IR_15, IR_08, NKEY_0A, NKEY_0B, IR_12, IR_11, NKEY_0E, NKEY_0F,
    NKEY_10, NKEY_11, NKEY_12, NKEY_13, NKEY_14, IR_07, IR_09, NKEY_17, IR_13, IR_10, NKEY_1A, NKEY_1B, IR_16, NKEY_1D, NKEY_1E, NKEY_1F,
    NKEY_20, NKEY_21, NKEY_22, NKEY_23, NKEY_24, NKEY_25, NKEY_26, NKEY_27, NKEY_28, NKEY_29, NKEY_2A, NKEY_2B, NKEY_2C, NKEY_2D, NKEY_2E, NKEY_2F,
    NKEY_30, NKEY_31, NKEY_32, NKEY_33, NKEY_34, NKEY_35, NKEY_36, NKEY_37, NKEY_38, NKEY_39, NKEY_3A, NKEY_3B, NKEY_3C, NKEY_3D, NKEY_3E, NKEY_3F,
    IR_04, NKEY_41, IR_18, IR_05, IR_03, IR_00, IR_01, IR_02, NKEY_48, NKEY_49, IR_20, NKEY_4B, NKEY_4C, NKEY_4D, NKEY_4E, NKEY_4F,
    NKEY_50, NKEY_51, IR_19, NKEY_53, NKEY_54, NKEY_55, NKEY_56, NKEY_57, NKEY_58, NKEY_59, IR_17, NKEY_5B, NKEY_5C, NKEY_5D, IR_14, NKEY_5F,
};

/*----------------------------------------------------------------------------*/
/**@brief   ad按键初始化
   @param   void
   @param   void
   @return  void
   @note    void ad_key0_init(void)
*/
/*----------------------------------------------------------------------------*/
void ir_timeout(void *param)
{
    ir_code.boverflow++;
    if (ir_code.boverflow > 56) //56*2ms ~= 112ms
    {
        //puts("ir_to\n");
        ir_code.bState = 0;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   time1红外中断服务函数
   @param   void
   @param   void
   @return  void
   @note    void timer1_ir_isr(void)
*/
/*----------------------------------------------------------------------------*/
void timer1_isr(void)
{
    u16 bCap1;
    u8 cap = 0;
//    TMR1_CON |= BIT(14);
//    bCap1 = TMR1_PRD;
//    TMR1_CNT =0;
    bCap1=timer_ctrl(timer1_hl,TIMER_CMD_GET_PRDNUM,0,NULL);
    timer_ctrl(timer1_hl,TIMER_CMD_SET_CNT,0,NULL);
    cap = bCap1 /timer1_pad; ///ms

    if (cap <= 1)
    {
        ir_code.wData >>= 1;
        ir_code.bState++;
        ir_code.boverflow = 0;
    }
    else if (cap == 2)
    {
        ir_code.wData >>= 1;
        ir_code.bState++;
        ir_code.wData |= 0x8000;
        ir_code.boverflow = 0;
    }
    /*13ms-Sync*/
    /*
    else if ((cap == 13) || (cap < 8) || (cap > 110))
    {
        ir_code.bState = 0;
    }
    else
    {
        ir_code.boverflow = 0;
    }
    */

    else if((cap == 13)&&(ir_code.boverflow < 8))
    {
        ir_code.bState = 0;
    }
    else if((cap < 8)&&(ir_code.boverflow < 5))
    {
        ir_code.bState = 0;
    }
    else if((cap >110)&&(ir_code.boverflow > 53))
    {
        ir_code.bState = 0;
    }
    else if((cap >20)&&(ir_code.boverflow > 53)) //溢出情况下 （12M 48M）
    {
        ir_code.bState = 0;
    }
    else
    {
        ir_code.boverflow = 0;
    }

    if (ir_code.bState == 16)
    {
        ir_code.wUserCode = ir_code.wData;
        //printf("%x\n",ir_code.wData);
    }
    if (ir_code.bState == 32)
    {
        //printf("%x\n",ir_code.wData);
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   ir按键初始化
   @param   void
   @param   void
   @return  void
   @note    void ir_key_init(void)
*/
/*----------------------------------------------------------------------------*/
s32 ir_key_init(void)
{
    //timer1
    s32 ret = 0;
    key_puts("ir key init\n");

    __timer_param timer_param;
    timer1_hl = timer_open(TIMER1,TIMER_MAX_ISR_FUN);
    if(NULL==timer1_hl)
    {
        return TIMER_DRV_OPEN_ERR;
    }
    timer_param.work_mode=TIMER_WORK_MODE_CAPTURE_IODN;
    timer_param.tick_time=1000;//1ms
    ret = timer_init_api(timer1_hl,&timer_param);
    if(ret != TIMER_NO_ERR)
        return ret;

    ret=timer_ctrl(timer1_hl,TIMER_CMD_IRFLT_SET,IR_PORTA(14),NULL);///IRFLT IO SEL
    if(ret!=TIMER_NO_ERR)
        return ret;

    timer1_pad=timer_ctrl(timer1_hl,TIMER_CMD_GET_START_PRDNUM,0,NULL);

    ret=timer_reg_isr_callback_fun(timer1_hl,4,timer1_isr);
    if(ret!=TIMER_NO_ERR)
        return ret;

    ret=timer_start(timer1_hl);
    if(ret!=TIMER_NO_ERR)
        return ret;

    ret = timer_reg_isr_fun(timer0_hl,1,ir_timeout,NULL);
	if(ret != TIMER_NO_ERR)
        return ret;
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   获取ir按键值
   @param   void
   @param   void
   @return  void
   @note    void get_irkey_value(void)
*/
/*----------------------------------------------------------------------------*/
tu8 get_irkey_value(void)
{
    tu8 tkey = 0xff;
    if (ir_code.bState != 32)
    {
        return tkey;
    }

    if ((((u8*)&ir_code.wData)[0] ^ ((u8*)&ir_code.wData)[1]) == 0xff)
    {
        if (ir_code.wUserCode == 0xFF00)
        {
            tkey = IRTabFF00[(u8)ir_code.wData];
        }
    }
    else
    {
        ir_code.bState = 0;
    }
    return tkey;
}

static s32 ir_key_ck_reset(void)
{
    s32 ret = 0;
    __timer_param timer_param;
	puts("ir_clk_reset\n");

    timer_param.work_mode=TIMER_WORK_MODE_CAPTURE_IODN;
    timer_param.tick_time=1000;//1ms

    ret = timer_init_api(timer1_hl,&timer_param);
    if(ret != TIMER_NO_ERR)
	{
		printf("timer_init err = %x\n",ret);
        return ret;
	}

    ret=timer_start(timer1_hl);
    if(ret!=TIMER_NO_ERR)
	{
		printf("timer_start err = %x\n",ret);
        return ret;
	}

    ret=timer_ctrl(timer1_hl,TIMER_CMD_IRFLT_CLK_SET,0,NULL);///IRFLT IO SEL
    if(ret!=TIMER_NO_ERR)
	{
		printf("irft err = %x\n",ret);
        return ret;
	}

    timer1_pad=timer_ctrl(timer1_hl,TIMER_CMD_GET_START_PRDNUM,0,NULL);
	puts("ir_clk_ok\n");
	return TIMER_NO_ERR;
}

static struct clock_switch clock_switch_key;

CLOCK_SWITCH_USER_REGISTER(CLOCK_SWITCH_EXIT, &clock_switch_key, ir_key_ck_reset, "IR_KEY");

#else

SET(interrupt(""))void timer1_isr(void)
{

}
#endif/*KEY_IR_EN*/



