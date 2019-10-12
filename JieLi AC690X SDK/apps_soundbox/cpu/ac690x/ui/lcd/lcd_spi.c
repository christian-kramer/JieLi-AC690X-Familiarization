#include "sdk_cfg.h"
#include "ui/lcd/lcd_spi.h"
#include "ui/ui_api.h"
#include "ui/lcd/lcd_drv_api.h"

#if LCD_128X64_EN

#define WAIT_SPI1_OK()    while (!(JL_SPI1->CON & BIT(15)));JL_SPI1->CON |= BIT(14)

void SPI1_init(bool is3Wrie, u8 speed)
{
    JL_SPI1->BAUD = speed;

    JL_SPI1->CON = 0x4021;//0100 0000 0010 0001

    LCD_SPI_SET_GROUP();
    LCD_SPI_SET_CLK();
    LCD_SPI_SET_DATAOUT();

    if(is3Wrie)
    {
       LCD_SPI_SET_DATAIN();
       LCD_SPI_SET_MODE_INOUT();///Ë«Ïò
    }
    else
    {
        LCD_SPI_SET_MODE_OUT();///µ¥Ïò
    }
}


void SPI1_WriteByte(u8 dat)
{
    //put_u16hex(dat);
    JL_SPI1->CON &= ~BIT(12);
    JL_SPI1->BUF = dat;
    WAIT_SPI1_OK();
    //printf("ok");
}


void SPI1_DmaWrite(void *buf, u16 len)
{
    JL_SPI1->CON &= ~BIT(12);
    JL_SPI1->ADR = (u32) buf;
    JL_SPI1->CNT = len;

    WAIT_SPI1_OK();
}

#if 0
u8 SPI1_ReadByte(void)
{
    JL_SPI1->CON |= BIT(12);
    JL_SPI1_>BUF = 0xff;
    WAIT_SPI1_OK();

    return SPI1_BUF;
}

void SPI1_DmaRead(void *buf, tu16 len)
{
    JL_SPI1->CON |= BIT(12);      //[12]:DIR
    JL_SPI1_>ADR = (u32) buf;
    JL_SPI1_>CNT = len;
    WAIT_SPI1_OK();
}
#endif

#else

void spi_flash_read(u8 *buf, u32 addr, u32 len)
{
}

void spi_font_tab_read(u8 *buf, u32 addr, u32 len)
{
}

#endif/*LCD_128x64_EN*/
