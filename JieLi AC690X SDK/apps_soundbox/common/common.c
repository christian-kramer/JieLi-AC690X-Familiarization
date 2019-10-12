/*--------------------------------------------------------------------------*/
/**@file     common.c
   @brief    常用公共函数
   @details
   @author
   @date    2011-3-7
   @note    CD003
*/
/*----------------------------------------------------------------------------*/
#include "typedef.h"
#include "common/common.h"

void delay(u32 d)sec(.poweroff_text);
void delay(u32 d)
{
    while(d--)
	{
        __builtin_pi32_nop();
    }
}




