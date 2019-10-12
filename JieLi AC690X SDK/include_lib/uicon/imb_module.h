#ifndef __IMB_MODULE_H__
#define __IMB_MODULE_H__

#include "typedef.h"
//#include "dsc_ram.h"

struct imb_isr_work{
	void * param;
	void (*handler)(void * param);
};

// #define REGISTER_IMB_WORK(hdl, p)  const struct imb_isr_work imb_isr_##hdl \
                                        // __attribute__((section(".imb_isr_work"))) = {\
                                        // .param = (void *)p,\
                                        // .handler=hdl\
                                    // }
#define REGISTER_IMB_WORK(hdl, p)  const struct imb_isr_work imb_isr_##hdl \
                                        sec(.imb_isr_work) = {\
										.param = (void *)p,\
										.handler=hdl\
									}

#define SDRAM_ADDR(addr)  ((u32)(0x2000000+addr))
#define SET_REG(val,pos)  ((u32)val << pos)

typedef enum
{
	LAYER_IMAGE0,			//图片层0(没有透明度)
	LAYER_IMAGE1,			//图片层1
	LAYER_OSD0_WIN0,		//OSD层0 窗口0
	LAYER_OSD0_WIN1,		//OSD层0 窗口1
	LAYER_OSD0_WIN2,		//OSD层0 窗口2
	LAYER_OSD1_WIN0,		//OSD层1 窗口0
	LAYER_OSD1_WIN1,		//OSD层1 窗口1
	LAYER_OSD1_WIN2,		//OSD层1 窗口2
	LAYER_IMAGE1_MASK,  //MASK层(图片层1)
}LAYER_TYPE;//图层标号
//说明：
//1.osd层一共有两层,每层osd最多有三个窗口，这三个窗口在垂直方向上不能交叉,不同层的osd在垂直方向可以相互交叉
//2.除了图片层0没有透明度，其它各层都有透明度
//3.各图层还需要遵循对齐的原则
//  图片层x,y,width,height都需要4对齐
//  OSD层x,y,width,height都需要2对齐


//字体颜色
typedef struct
{
	u8 forecolor;
	u8 backcolor;
}OSDCOLOR;

typedef struct
{
	u16 x;
	u16 y;
}TEXTPOS;
extern TEXTPOS text_pos;

typedef struct
{
	u16 x;
	u16 y;
	u16 width;
	u16 height;
}GUI_RECT;

typedef struct
{
	u16 x;
	u16 y;
}GUI_POINT;


#define OSD0            0x00
#define OSD1            0x01
#define OSD_ALL         0x02
#define OSD1_B2         0x03   //2bit模式，只对OSD1有效，窗口宽度16 bytes对齐
#define WINDOW0         0x00
#define WINDOW1         0x10
#define WINDOW2         0x20

//<显示设备>
enum
{
    DISP_RGB,
    DISP_MCU,
    DISP_HDMI,
};

//<接口参数>
#define MODE_RGB_SYNC   0x00//无显存 连续帧  接HSYNC VSYNC信号
#define MODE_RGB_DE     0x01//无显存 连续帧  接DE信号
#define MODE_MCU        0x02//带显存 单帧
#define MODE_HDMI       0x03//HDMI显示
#define MODE_AVOUT      0x04//AV OUT显示
#define MODE_AVOUT1     0x05

#define PORT_1BIT       0x00//AV OUT
#define PORT_8BITS      0x01
#define PORT_16BITS     0x03
#define PORT_18BITS     0x07
#define PORT_24BITS     0x0f

//<输出格式>
#define FORMAT_RGB888   0x01
#define FORMAT_YUV888   0x02
#define FORMAT_RGB565   0x03
#define FORMAT_YUV422   0x04
#define FORMAT_ITU656   0x05

enum
{
    CFG_OSD_ADDR,  //配置OSD颜色表起始地址(包括OSD0和OSD1)
    CFG_OSD0_DAT,  //配置OSD0颜色表数据
    CFG_OSD1_DAT,  //配置OSD1颜色表数据
    CFG_ALL_DAT,   //同时配置OSD0和OSD1颜色表数据
};

typedef struct
{
    u16 h_total_width;  //水平时钟总数
    u16 h_sync_width;   //水平同步时钟
    u16 h_act_start;    //水平起始时钟
    u16 h_act_width;    //水平宽度
    u16 v_total_height; //垂直时钟总数
    u16 v_sync_height;  //垂直同步时钟
    u16 v_act_start;    //垂直起始时钟
    u16 v_act_start1;   //垂直起始时钟(隔行模式)
    u16 v_act_width;    //垂直宽度
}L0_PARA_CFG;


//OSD图层参数
typedef struct _OSDWIN
{
	u16 x;
	u16 y;
	u16 width;
	u16 height;
	u8 *buf;
    u8 layer;
	u8 alpha;
	u8 enable;
}OSDWIN;

typedef struct _OSDLAYER
{
	u32 *palette;
	u16 item;
	OSDWIN win[3];
}OSDLAYER;

//IMAGE图层参数
typedef struct
{
    u8 layer;
    u8 enable;
    u8 alpha;       //图层1有效
    u8 mask_en;     //图层1有效
    u8 buf_num;
    u16 x;
    u16 y;
    u16 width;
    u16 height;
//    u8 *ay;
//    u8 *au;
//    u8 *av;
    u8 *buf;
    u8 *mask_buf;   //图层1有效
    struct dsc_ram *dsc_ram;
}IMGLAYER;

//MASK图层参数
typedef struct
{
    u16 x;
    u16 y;
    u16 width;
    u16 height;
    u8  *buf;
    u8  enable;
}MASKLAYER;

//转换图层参数
typedef struct
{
    u32 y_offs;
    u32 y_gain;
    u32 u_gain;
    u32 v_gain;
    u32 r_gain;
    u32 g_gain;
    u32 b_gain;
}CGAIN;


typedef struct
{
    u8 drive_mode;         //驱动类型
    u8 data_width;         //数据接口位宽
    u8 out_format;         //输出数据格式
    u8 isr_prior;          //中断优先级
    u32 bkcolor;           //背景颜色
    L0_PARA_CFG *para;     //时钟配置
    CGAIN *gain;           //颜色增益
    void (*hook) (u16 x,u16 y,u16 w,u16 h);   //配置控制器，只针对MCU接口的屏
    u8 interface_mode;
}DISP_DEV_CFG;

extern IMGLAYER image[2];
extern OSDLAYER osdlayer[2];
extern MASKLAYER mask;

extern u32 tempaddr;
extern u16 tempwidth,tempheight;

#define SET_A(addr,width,height)  tempaddr = addr; tempwidth = width; tempheight = height;
#define GET_AY()  (tempaddr)
#define GET_AU()  (tempaddr+tempwidth*tempheight)
#define GET_AV()  (tempaddr+tempwidth*tempheight*5/4)

//OSD图层颜色定义
#define COLOR_LUCENCY   0      //透明
#define COLOR_YELLOW    5      //黄色
#define COLOR_RED       35     //红色
#define COLOR_GREEN     225    //绿色
#define COLOR_BLUE		235    //蓝色
#define COLOR_WHITE		245    //白色
#define COLOR_GRAY      250    //灰色
//#define COLOR_BLACK		255    //黑色
#define COLOR_BLACK		224    //黑色

/*----------------------------------------------------------------------------*/
/*                            IMB Module Function List                        */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**@brief   初始化IMB模块
   @param   dev:设备参数
   @return  无
   @note    void imb_init(DISP_DEV_CFG* dev);
*/
/*----------------------------------------------------------------------------*/
extern void imb_init(DISP_DEV_CFG* dev);
/*----------------------------------------------------------------------------*/
/**@brief   获取图片层句柄
   @param   layer:图层序号
   @return  图片层句柄
   @note    IMGLAYER *imb_get_image_hdl(u8 layer);
*/
/*----------------------------------------------------------------------------*/
extern IMGLAYER *imb_get_image_hdl(u8 layer);
/*----------------------------------------------------------------------------*/
/**@brief   图片层配置
   @param   img:图片层句柄
   @return  无
   @note    void imb_image_cfg(IMGLAYER *img);
*/
/*----------------------------------------------------------------------------*/
extern void imb_image_cfg(IMGLAYER *img);
/*----------------------------------------------------------------------------*/
/**@brief   获取OSD层句柄
   @param   layer:图层序号
   @return  OSD层句柄
   @note    OSDWIN *imb_get_osd_hdl(u8 layer);
*/
/*----------------------------------------------------------------------------*/
extern OSDWIN *imb_get_osd_hdl(u8 layer);
/*----------------------------------------------------------------------------*/
/**@brief   OSD层配置
   @param   oshdl:OSD层句柄
   @return  无
   @note    void imb_osd_win_cfg(OSDWIN *osdhdl);
*/
/*----------------------------------------------------------------------------*/
extern void imb_osd_win_cfg(OSDWIN *osdhdl);
/*----------------------------------------------------------------------------*/
/**@brief   启动IMB模块
   @param   无
   @return  无
   @note    void imb_start(void);
*/
/*----------------------------------------------------------------------------*/
extern void imb_start(void);
/*----------------------------------------------------------------------------*/
/**@brief   打开IMB模块
   @param   无
   @return  无
   @note    void imb_open(void);
*/
/*----------------------------------------------------------------------------*/
extern void imb_open(void);
/*----------------------------------------------------------------------------*/
/**@brief   关闭IMB模块
   @param   无
   @return  无
   @note    void imb_close(void);
*/
/*----------------------------------------------------------------------------*/
extern void imb_close(void);


#endif
