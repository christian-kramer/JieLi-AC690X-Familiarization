#ifndef __LCD_DRV_INTERFACE_H__
#define __LCD_DRV_INTERFACE_H__

#include "sdk_cfg.h"
//#include "dc_showres.h"

//常量声明
//const LCD_INFO lcd_info;

//函数声明
void lcd_clear_area(u8 start_page,u8 end_page);
void lcd_clear_area_with_draw(u8 start_page,u8 end_page);
void lcd_clear_area_rect(u8 start_page,u8 end_page,u8 x0,u8 x1);
u32 lcd_TurnPixelReverse_Page(u8 startpage, u8 pagelen);
u32 lcd_TurnPixelReverse_Rect(u8 left, u8 top, u8 right, u8 bottom);
void lcd_sendcmd(u8 cmd);
void lcd_draw(u8 start_page, u8 page_len,u8 * pbuf);
u32  lcd_write_dma(u8 * pbuf);
void lcd_spi_init(void);

#endif/*__LCD_DRV_INTERFACE_H__*/

