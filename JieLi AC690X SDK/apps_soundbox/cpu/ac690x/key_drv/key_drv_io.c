#include "includes.h"
#include "key_drv/key.h"
#include "key_drv/key_drv_io.h"


#if KEY_IO_EN

/*----------------------------------------------------------------------------*/
/**@brief   io按键初始化
   @param   void
   @param   void
   @return  void
   @note    void io_key_init(void)
*/
/*----------------------------------------------------------------------------*/
void io_key_init(void)
{
    KEY_INIT();
}

/*----------------------------------------------------------------------------*/
/**@brief   获取IO按键电平值
   @param   void
   @param   void
   @return  key_num:io按键号
   @note    tu8 get_iokey_value(void)
*/
/*----------------------------------------------------------------------------*/
tu8 get_iokey_value(void)
{
    //key_puts("get_iokey_value\n");
    tu8 key_num = NO_KEY;
	static tu8 keyFlag=0x00;
	if(IS_KEY0_DOWN())
	{
		//key_puts("KEY0\n");
		if(keyFlag)
			return 0;
	}
	else
	{
		keyFlag=0x01;
	}
#if 0
    if(IS_KEY1_DOWN())
	{
        key_puts(" KEY1 ");
        return 1;
    }
    if(IS_KEY2_DOWN())
	{
        key_puts(" KEY2 ");
        return 2;
    }
    if(IS_KEY3_DOWN())
	{
		key_puts(" KEY3 ");
        return 3;
    }
#endif
//    KEY1_OUT_L();
//    if(IS_KEY2_DOWN())//
//    {
//        PORTA_OUT |= BIT(13);
//        return 4;
//    }
    return key_num;
}

#endif/*KEY_IO_EN*/
