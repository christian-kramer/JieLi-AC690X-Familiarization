#include "sdk_cfg.h"
#include "ui/lcd/lcd_drv_api.h"
#include "ui/lcd/lcd_spi.h"
#include "uicon/dc_showres.h"
#include "ui/lcd/lcd_drv_interface.h"
#include "uicon/ui_con.h"
#include "uicon/menu.h"
#include "ui/ui_api.h"

#if LCD_128X64_EN

#define LCD_MALLOC_EN	0

#define LCD_SIZE	(8*128)

#if LCD_MALLOC_EN
extern void * zebra_malloc(u32 size);
u8 *disp_buf;
#else
u8 disp_buf[LCD_SIZE] AT(.ram1_bss);
#endif

LCD_INFO *lcd_parm;
extern u8 g_language_mode;
extern const LCD_INFO lcd_info;


/*----------------------------------------------------------------------------*/
/**@brief   lcd update buf
   @param   void
   @return  void
   @note    void draw_lcd_buf(void)
*/
/*----------------------------------------------------------------------------*/
void draw_lcd_buf(void)
{
     lcd_draw(0 ,LCDPAGE,disp_buf);
}

/*----------------------------------------------------------------------------*/
/**@brief   lcd spi初始化
   @param   void
   @return  void
   @note    void lcd_spi_init(void)
*/
/*----------------------------------------------------------------------------*/
void lcd_clear(void)
{
    memset(disp_buf, 0x00, LCDPAGE*LCDCOLUMN);
//    flush_dcache(disp_buf, LCDPAGE*LCDCOLUMN);//liwenhong
//    lcd_draw(0 ,LCDPAGE,disp_buf);
}


/*----------------------------------------------------------------------------*/
/**@brief   LCD显示初始化,包括：硬件初始化，字体初始化，控件初始化
   @param   void
   @return  void
   @note    void lcd_init(void)
*/
/*----------------------------------------------------------------------------*/
void lcd_init(void)
{
    puts("---lcd_init---!\n");

#if LCD_MALLOC_EN
	disp_buf = zebra_malloc(LCD_SIZE);
#endif

    memset(disp_buf, 0x0,LCDPAGE*LCDCOLUMN);
    lcd_parm = (LCD_INFO*)&lcd_info;

    ///硬件初始化
    lcd_hardware_init();

    ///非UI显示接口初始化
    if(!dc_init())
    {
        ui_puts("\ndc init fail   ---!\n");
        return;
    }
    else
    {
        ui_puts("dc init succ !\n");
    }

    dc_set_screen_scale(1,1);//对屏幕显示进行同步放大

    ///字库接口初始化
    g_language_mode = Chinese_Simplified;//English;//Chinese_Simplified;
    if(!font_init(g_language_mode))
    {
        ui_puts("\nfont init fail  ---! \n");
		return;
    }
    else
    {
        ui_puts("font init succ!\n");
    }

    ///UI显示接口初始化
    if(!ui_init(SCR_WIDTH,SCR_HEIGHT))
    {
        ui_puts("\nui init fail  ---!\n");
        return;
    }
    else
    {
        ui_puts("ui init succ!\n");
    }

    lcd_clear();
}

#endif/*LCD_128x64_EN*/

