#ifndef __MCPWM_API_H__
#define __MCPWM_API_H__

#include "typedef.h"

#define MCPWM_USE_DENOMINATOR 100

enum 
{
	MCPWM_KEPP_TMRCNT_MODE,
	MCPWM_INCREASE_MODE,
	MCPWM_INCREASE_REDUCE_MODE,
	MCPWM_EXTERNAL_CONTROL_MODE,
};

enum 
{
	//aim at sys clk
	MCPWM_CLK_DIV1 = 0,
	MCPWM_CLK_DIV2,
	MCPWM_CLK_DIV4,
	MCPWM_CLK_DIV8,
	MCPWM_CLK_DIV16,
	MCPWM_CLK_DIV32,
	MCPWM_CLK_DIV64,
	MCPWM_CLK_DIV128,
};

#define MCPWMCH0_H       BIT(0)
#define MCPWMCH0_L       BIT(1)
#define MCPWMCH1_H       BIT(2)
#define MCPWMCH1_L       BIT(3)
#define MCPWMCH2_H       BIT(4)
#define MCPWMCH2_L       BIT(5)
#define MCPWMCH3_H       BIT(6)
#define MCPWMCH3_L       BIT(7)
#define MCPWMCH4_H       BIT(8)
#define MCPWMCH4_L       BIT(9)
#define MCPWMCH5_H       BIT(10)
#define MCPWMCH5_L       BIT(11)
#define MCPWMCH_ALL      0xffff

#define MCPWMCH0_H_OPPOSITE       BIT(0)
#define MCPWMCH0_L_OPPOSITE       BIT(1)
#define MCPWMCH1_H_OPPOSITE       BIT(2)
#define MCPWMCH1_L_OPPOSITE       BIT(3)
#define MCPWMCH2_H_OPPOSITE       BIT(4)
#define MCPWMCH2_L_OPPOSITE       BIT(5)
#define MCPWMCH3_H_OPPOSITE       BIT(6)
#define MCPWMCH3_L_OPPOSITE       BIT(7)
#define MCPWMCH4_H_OPPOSITE       BIT(8)
#define MCPWMCH4_L_OPPOSITE       BIT(9)
#define MCPWMCH5_H_OPPOSITE       BIT(10)
#define MCPWMCH5_L_OPPOSITE       BIT(11)


/*----------------------------------------------------------------------------*/
/**@brief open mcpwm module 
   @param pwm_md : 产生pwm方式  1:递增模式   2:递增-递减循环模式
		  sys_clk : 系统时钟
		  div : 系统时钟分频,pwm timer的clk为sys_clk/(2^n)
		  deno : 占空比的分母 
   @return NULL
   @note must init before use mcpwm module
 */
/*----------------------------------------------------------------------------*/
void mcpwm_module_on(u8 pwm_md , u32 sys_clk , u8 div , u16 deno);

/*----------------------------------------------------------------------------*/
/**@brief close mcpwm module 
   @param ch : BIT 0~1 对应 MCPWMCH0_H MCPWMCH0_L
			   BIT 2~3 对应 MCPWMCH1_H MCPWMCH1_L
			   BIT 4~5 对应 MCPWMCH2_H MCPWMCH2_L
		       BIT 6~7 对应 MCPWMCH3_H MCPWMCH3_L
               BIT 8~9 对应 MCPWMCH4_H MCPWMCH4_L
               BIT 10~11 对应 MCPWMCH5_H MCPWMCH5_L
			   0xffff : close all mcpwm
   @return NULL
   @note 
 */
/*----------------------------------------------------------------------------*/
void mcpwm_module_off(u16 ch);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm0 init 
   @param opposite_ctl : BIT 0~1 对应 MCPWMCH0_H MCPWMCH0_L反向控制 1:enable  0:disable
		  ch : BIT 0~1 对应 MCPWMCH0_H MCPWMCH0_L  1:enable  0:disable
   @return NULL
   @note must init before use mcpwm0 
 */
/*----------------------------------------------------------------------------*/
void mcpwm0_init(u16 opposite_ctl , u16 ch);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm0 set 
   @param fre : pwm fre 
		  duty : 占空比设置,实际占空比为duty/deno
   @return NULL
   @note  
 */
/*----------------------------------------------------------------------------*/
void set_mcpwm0(u32 fre ,u32 duty);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm1 init 
   @param opposite_ctl : BIT 2~3 对应 MCPWMCH1_H MCPWMCH1_L反向控制 1:enable  0:disable
		  ch : BIT 2~3 对应 MCPWMCH1_H MCPWMCH1_L  1:enable  0:disable
   @return NULL
   @note must init before use mcpwm1 
 */
/*----------------------------------------------------------------------------*/
void mcpwm1_init(u16 opposite_ctl , u16 ch);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm1 set 
   @param fre : pwm fre 
		  duty : 占空比设置,实际占空比为duty/deno
   @return NULL
   @note  
 */
/*----------------------------------------------------------------------------*/
void set_mcpwm1(u32 fre ,u32 duty);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm2 init 
   @param opposite_ctl : BIT 4~5 对应 MCPWMCH2_H MCPWMCH2_L反向控制 1:enable  0:disable
		  ch : BIT 4~5 对应 MCPWMCH2_H MCPWMCH2_L  1:enable  0:disable
   @return NULL
   @note must init before use mcpwm2 
 */
/*----------------------------------------------------------------------------*/
void mcpwm2_init(u16 opposite_ctl , u16 ch);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm2 set 
   @param fre : pwm fre 
		  duty : 占空比设置,实际占空比为duty/deno
   @return NULL
   @note  
 */
/*----------------------------------------------------------------------------*/
void set_mcpwm2(u32 fre ,u32 duty);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm3 init 
   @param opposite_ctl : BIT 6~7 对应 MCPWMCH3_H MCPWMCH3_L反向控制 1:enable  0:disable
		  ch : BIT 6~7 对应 MCPWMCH3_H MCPWMCH3_L  1:enable  0:disable
   @return NULL
   @note must init before use mcpwm3 
 */
/*----------------------------------------------------------------------------*/
void mcpwm3_init(u16 opposite_ctl , u16 ch);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm3 set 
   @param fre : pwm fre 
		  duty : 占空比设置,实际占空比为duty/deno
   @return NULL
   @note  
 */
/*----------------------------------------------------------------------------*/
void set_mcpwm3(u32 fre ,u32 duty);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm4 init 
   @param opposite_ctl : BIT 8~9 对应 MCPWMCH4_H MCPWMCH4_L反向控制 1:enable  0:disable
		  ch : BIT 8~9 对应 MCPWMCH4_H MCPWMCH4_L  1:enable  0:disable
   @return NULL
   @note must init before use mcpwm4 
 */
/*----------------------------------------------------------------------------*/
void mcpwm4_init(u16 opposite_ctl , u16 ch);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm4 set 
   @param fre : pwm fre 
		  duty : 占空比设置,实际占空比为duty/deno
   @return NULL
   @note  
 */
/*----------------------------------------------------------------------------*/
void set_mcpwm4(u32 fre ,u32 duty);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm5 init 
   @param opposite_ctl : BIT 10~11 对应 MCPWMCH5_H MCPWMCH5_L反向控制 1:enable  0:disable
		  ch : BIT 10~11 对应 MCPWMCH5_H MCPWMCH5_L  1:enable  0:disable
   @return NULL
   @note must init before use mcpwm5 
 */
/*----------------------------------------------------------------------------*/
void mcpwm5_init(u16 opposite_ctl , u16 ch);

/*----------------------------------------------------------------------------*/
/**@brief mcpwm5 set 
   @param fre : pwm fre 
		  duty : 占空比设置,实际占空比为duty/deno
   @return NULL
   @note  
 */
/*----------------------------------------------------------------------------*/
void set_mcpwm5(u32 fre ,u32 duty);

#endif   //__MCPWM_API_H__
