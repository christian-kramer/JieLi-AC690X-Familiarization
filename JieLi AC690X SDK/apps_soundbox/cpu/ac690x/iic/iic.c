/*--------------------------------------------------------------------------*/
/**@file     iic_io.c
   @brief    IO模拟的IIC的驱动
   @details
   @author  zhiying
   @date   2013-3-26
   @note
*/
/*----------------------------------------------------------------------------*/
#include "iic.h"

/*----------------------------------------------------------------------------*/
/** @brief: IIC 模块初始化函数
    @param: void
    @return:void
    @author:Juntham
    @note:  void iic_init_io(void)
*/
/*----------------------------------------------------------------------------*/
void iic_init_io(void)
{
    iic_data_h();
    iic_data_out();         //SDA设置成输出
    iic_clk_h();
    iic_clk_out();         	//SCL设置成输出
}
/*----------------------------------------------------------------------------*/
/**@brief
   @param   无
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void iic_start(void)
{
    iic_init_io();
    iic_delay();
    iic_data_l();
    iic_delay();
    iic_clk_l();
    iic_delay();
    //iic_data_h();
}
/*----------------------------------------------------------------------------*/
/**@brief   STOP IIC
   @param   无
   @return  无
   @note    void iic_stop(void)
*/
/*----------------------------------------------------------------------------*/
void iic_stop(void)
{
    iic_data_out();
    iic_data_l();
    iic_delay();
    iic_clk_h();
    iic_delay();
    iic_data_h();
    iic_delay();
}
/*----------------------------------------------------------------------------*/
/**@brief   读取ACK
   @param   无
   @return  都会的ACK/NACK的电平
   @note    tbool r_ack(void)
*/
/*----------------------------------------------------------------------------*/
tbool r_ack(void)
{
    bool tnack;
    iic_data_in();
    iic_delay();
    iic_clk_h();
    iic_delay();
    iic_delay();
    iic_delay();
    iic_delay();
    iic_delay();
    tnack = iic_data_r();
    iic_clk_l();
    iic_delay();
    return tnack;
}
/*----------------------------------------------------------------------------*/
/**@brief   发送一个ACK信号的数据,
   @param   flag ：发送的ACK/nack的类型
   @return  无
   @note    void s_ack(u8 flag)
*/
/*----------------------------------------------------------------------------*/
void s_ack(u8 flag)
{
    iic_data_out();
    iic_clk_l();
    if (flag)
    {
        iic_data_h();
    }
    else
    {
        iic_data_l();
    }
    iic_delay();
    iic_clk_h();
    iic_delay();
    iic_clk_l();
}
/*----------------------------------------------------------------------------*/
/**@brief   从IIC总线接收一个BYTE的数据,
   @param   无
   @return  读取回的数据
   @note    u8 iic_revbyte_io( void )
*/
/*----------------------------------------------------------------------------*/
tu8 iic_revbyte_io( void )
{
    u8 tbyteI2C = 0X00;
    u8 i;
    iic_data_in();
    iic_delay();
    for (i = 0; i < 8; i++)
    {
        iic_clk_h();
        tbyteI2C <<= 1;
        iic_delay();
        if (iic_data_r())
        {
            tbyteI2C++;
        }
        iic_clk_l();
        iic_delay();
    }
    return tbyteI2C;
}
/*----------------------------------------------------------------------------*/
/**@brief   从IIC总线接收一个BYTE的数据,并发送一个指定的ACK
   @param   para ：发送ACK 还是 NACK
   @return  读取回的数据
   @note    u8 iic_revbyte( u8 para )
*/
/*----------------------------------------------------------------------------*/
tu8 iic_revbyte( u8 para )
{
    u8 tbyte;
    tbyte = iic_revbyte_io();
    s_ack(para);
	delay(30000);
    return tbyte;
}
/*----------------------------------------------------------------------------*/
/**@brief   向IIC总线发送一个BYTE的数据
   @param   byte ：要写的EEROM的地址
   @return  无
   @note    void iic_sendbyte_io(u8 byte)
*/
/*----------------------------------------------------------------------------*/
void iic_sendbyte_io(u8 byte)
{
    u8 i;
    iic_data_out();
    iic_delay();
    //printf("byte: %02x\n", byte);
    for (i = 0; i < 8; i++)
    {
        if (byte & BIT(7))
        {
            iic_data_h();  //最高位是否为1,为1则SDA= 1,否则 SDA=0
        }
        else
        {
            iic_data_l();
        }
        iic_delay();
        iic_clk_h();
        iic_delay();
        byte <<= 1;                   //数据左移一位,进入下一轮送数
        iic_clk_l();
    }
}
/*----------------------------------------------------------------------------*/
/**@brief   向IIC总线发送一个BYTE的数据,并读取ACK
   @param   byte ：要写的EEROM的地址
   @return  无
   @note    void iic_sendbyte(u8 byte)
*/
/*----------------------------------------------------------------------------*/
void iic_sendbyte(u8 byte)
{
    iic_sendbyte_io(byte);
    r_ack();
	delay(30000);
}





///------------------------------------------------------------------------------
///------------------------------------IIC_API-----------------------------------
///------------------------------------------------------------------------------


bool g_iic_busy = 0; ///<iic繁忙标记
/*----------------------------------------------------------------------------*/
/** @brief: IIC 模块初始化函数
    @param: void
    @return:void
    @author:Juntham
    @note:  void iic_init(void)
*/
/*----------------------------------------------------------------------------*/
void iic_init(void)
{
   // iic_data_out();
    iic_data_h();
    iic_data_out();
    //iic_clk_out();
    iic_clk_h();    ///<配置接口函数IO
    iic_clk_out();
}

/*----------------------------------------------------------------------------*/
/** @brief: eeprom 核实
    @param: void
    @return:void
    @author:Juntham
    @note:  void eeprom_verify(void)
*/
/*----------------------------------------------------------------------------*/
void eeprom_verify(void)
{
    puts("eeprom_verify 1\n");

    if ((read_eerom(0) != 0x55)
            ||(read_eerom(1) != 0xAA))
    {
        write_eerom(0, 0x55);
        write_eerom(1, 0xAA);
    }

    puts("eeprom_verify 2\n");
    if ((read_eerom(0) != 0x55)
            ||(read_eerom(1) != 0xAA))
    {
        puts("iic-no\n");//外接eeprom无效
    }
    else
    {
        puts("iic-yes\n"); //有外接eeprom
    }
}

void iic_write_one_byte(u8 iic_dat)
{
    g_iic_busy  = 1;
    iic_start();                //I2C启动
    iic_sendbyte(iic_dat);      //写数据
    iic_stop();                 //I2C停止时序
}
/*----------------------------------------------------------------------------*/
/**@brief   IIC写函数
   @param   chip_id ：目标IC的ID号
   @param   iic_addr: 目标IC的目标寄存器的地址
   @param   *iic_dat: 写望目标IC的数据的指针
   @param   n:需要写的数据的数目
   @return  无
   @note    void  iic_write(u8 chip_id,u8 iic_addr,u8 *iic_dat,u8 n)
*/
/*----------------------------------------------------------------------------*/
void iic_write(u8 chip_id,u8 iic_addr,u8 *iic_dat,u8 n)
{
    g_iic_busy  = 1;
    iic_start();                //I2C启动
    iic_sendbyte(chip_id);         //写命令

    if (0xff != iic_addr)
    {
        iic_sendbyte(iic_addr);   //写地址
    }
    for (; n>0; n--)
    {
        iic_sendbyte(*iic_dat++);      //写数据
    }
    iic_stop();                 //I2C停止时序
    g_iic_busy = 0;
}
/*----------------------------------------------------------------------------*/
/**@brief   IIC总线向一个目标ID读取几个数据
   @param   address : 目标ID
   @param   *p     :  存档读取到的数据的buffer指针
   @param   number :  需要读取的数据的个数
   @return  无
   @note    void i2c_read_nbyte(u8 address,u8 *p,u8 number)
*/
/*----------------------------------------------------------------------------*/
void iic_readn(u8 chip_id,u8 iic_addr,u8 *iic_dat,u8 n)
{
    g_iic_busy = 1;
    iic_start();                //I2C启动
    iic_sendbyte(chip_id);         //写命令
    if (0xff != iic_addr)
    {
        iic_sendbyte(iic_addr);   //写地址
    }
    for (; n>1; n--)
    {
        *iic_dat++ = iic_revbyte(0);      //读数据
    }
    *iic_dat++ = iic_revbyte(1);
    iic_stop();                 //I2C停止时序
    g_iic_busy = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   EEROM/RTC RAM读函数
   @param   addr ：要写的EEROM/RTC RAM的地址
   @return  读到的数据
   @note    u8 read_info(u8 addr)
*/
/*----------------------------------------------------------------------------*/
tu8 read_info(u8 addr)
{
    return read_eerom(addr);
}

/*----------------------------------------------------------------------------*/
/**@brief   EEROM/RTC RAM写函数
   @param   addr ：要读的EEROM/RTC RAM的地址
   @param   dat    ：需要读的数据
   @return  无
   @note    void write_info(u8 addr,u8 dat)
*/
/*----------------------------------------------------------------------------*/
void write_info(u8 addr,u8 dat)
{
    write_eerom(addr,dat);
}

/*----------------------------------------------------------------------------*/
/**@brief   eeprom START
   @param   无
   @return  无
   @note    void  eeprom_page_write_start(void)
*/
/*----------------------------------------------------------------------------*/
void  eeprom_page_write_start(void)
{
    g_iic_busy  = 1;
    iic_start();                //I2C启动
    iic_sendbyte(0xa0);         //写命令
}
/*----------------------------------------------------------------------------*/
/**@brief   IIC STOP
   @param   无
   @return  无
   @note    void eeprom_page_write_stop(void)
*/
/*----------------------------------------------------------------------------*/
void eeprom_page_write_stop(void)
{
    iic_stop();                     //I2C停止iic_data_out
    g_iic_busy = 0;
}
/*----------------------------------------------------------------------------*/
/**@brief   EEROM读函数
   @param   addr ：要写的EEROM的地址
   @param   无
   @return  无
   @note    u8 iic_read(u8 iic_addr)
*/
/*----------------------------------------------------------------------------*/
tu8 read_eerom(u8 iic_addr)
{
    u8  tbyte;
    g_iic_busy = 1;
    iic_start();                    //I2C启动
    iic_sendbyte(0xa0);             //写命令
    iic_sendbyte(iic_addr);       //写地址
    iic_start();                    //写转为读命令，需要再次启动I2C
    iic_sendbyte(0xa1);             //读命令
    tbyte = iic_revbyte(1);
    iic_stop();                     //I2C停止
    g_iic_busy = 0;
    return  tbyte;
}

/*----------------------------------------------------------------------------*/
/**@brief   带有毫秒延时的EEROM写函数
   @param   addr ：要写的EEROM的地址
   @param   dat    ：需要写的数据
   @return  无
   @note    void write_info(u8 addr,u8 dat)
*/
/*----------------------------------------------------------------------------*/
void write_eerom(u8 addr,u8 dat)
{
    iic_write(0xa0,addr,&dat,1);
    iic_delay();
    iic_delay();
    iic_delay();
    iic_delay();
    iic_delay();
    iic_delay();
}

/*----------------------------------------------------------------------------*/
/** @brief: 记忆信息到存储器（EEPROM）
    @param: void
    @return:void
    @author:Juntham
    @note:  void set_memory(u8 addr, u8 dat)
*/
/*----------------------------------------------------------------------------*/
 void set_memory(u8 addr, u8 dat)
{
    write_eerom(addr, dat);
}
/*----------------------------------------------------------------------------*/
/** @brief: 获取记忆信息（EEPROM）
    @param: void
    @return:void
    @author:Juntham
    @note:  u8 get_memory(u8 addr)
*/
/*----------------------------------------------------------------------------*/
u8 get_memory(u8 addr)
{
    return read_eerom(addr);
}
