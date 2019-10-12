#ifndef	_IIC_H_
#define _IIC_H_

#include "typedef.h"
#include "common/common.h"
#include "sdk_cfg.h"


void iic_write(u8 chip_id,u8 iic_addr,u8 *iic_dat,u8 n);
void iic_readn(u8 chip_id,u8 iic_addr,u8 *iic_dat,u8 n);
void iic_init(void);
void iic_init_io(void);
void write_info(u8 addr,u8 dat);
tu8 read_info(u8 addr);

void write_eerom(u8 addr,u8 dat);
tu8 read_eerom(u8 iic_addr);
void eeprom_page_write_stop(void);
void eeprom_page_write_start(void);
void eeprom_verify(void);

void iic_start(void);
void iic_stop(void);
void iic_sendbyte_io(u8 byte);
void iic_sendbyte(u8 byte);
tu8 iic_revbyte( u8 para );

bool g_iic_busy;

#define iic_delay()      delay(3000)

#define IIC_DATA_PORT       12
#define IIC_CLK_PORT        13

#define iic_data_out()   do{JL_PORTA->DIR &= ~BIT(IIC_DATA_PORT);JL_PORTA->PU |= BIT(IIC_DATA_PORT);}while(0)
#define iic_data_in()    do{JL_PORTA->DIR |=  BIT(IIC_DATA_PORT);JL_PORTA->PU |= BIT(IIC_DATA_PORT);}while(0)
#define iic_data_r()     (JL_PORTA->IN&BIT(IIC_DATA_PORT))
#define iic_data_h()     do{JL_PORTA->OUT |= BIT(IIC_DATA_PORT);}while(0)
#define iic_data_l()     do{JL_PORTA->OUT &=~BIT(IIC_DATA_PORT);}while(0)

#define iic_clk_out()    do{JL_PORTA->DIR &=~BIT(IIC_CLK_PORT);JL_PORTA->PU |= BIT(IIC_CLK_PORT);}while(0)
#define iic_clk_h()      do{JL_PORTA->OUT |= BIT(IIC_CLK_PORT);}while(0)
#define iic_clk_l()      do{JL_PORTA->OUT &=~BIT(IIC_CLK_PORT);}while(0)

#define app_IIC_write(a, b, c, d) \
  iic_write(a, b, c, d)
#define app_IIC_readn(a, b, c, d)  \
  iic_readn(a, b, c, d)
#define app_E2PROM_write(a, b)  \
  write_eerom(a, b)
#define app_E2PROM_read(a)   \
  read_eerom(a)

#endif






