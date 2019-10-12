#ifndef _LCD_SPI_H_
#define _LCD_SPI_H_

#include "typedef.h"


extern void SPI1_init(bool is3Wrie, u8 speed);
extern void SPI1_WriteByte(u8 dat);
extern u8 SPI1_ReadByte(void);
extern void SPI1_DmaWrite(void *buf, u16 len);
extern void SPI1_DmaRead(void *buf, u16 len);

#endif/*_LCD_SPI_H_*/
