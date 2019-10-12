#ifndef __UART_PARAM_H__
#define __UART_PARAM_H__


#define UART0_HARDWARE_NAME  "uart0"
#define UART1_HARDWARE_NAME  "uart1"
#define UART2_HARDWARE_NAME  "uart2"     /*UART2 not support DMA mode*/


enum
{
    UART_NO_ERR               =0,
    UART_DRV_USED_ERR         =-1000 ,
    UART_DRV_NAME_ERR                ,
    UART_DRV_PARA_ERR                ,
    UART_DRV_ISRFUN_RDEF_ERR         ,
    UART_DRV_IO_ERR                  ,
    UART_DRV_WORKMODE_ERR            ,
};

enum
 {
    UART_TXPA5_RXPA6        =0, 
    UART_TXPB6_RXPB7          ,
    UART_TXPC2_RXPC3          ,
    UART_TXPA14_RXPA15        ,

    UART_TXPB0_RXPB1          ,
    UART_TXPC0_RXPC1          ,
    UART_TXPA1_RXPA2          ,
    UART_USBP_USBM            ,

    UART_TXPA3_RXPA4          ,
    UART_TXPA9_RXPA10        ,
    UART_TXPB9_RXPB10          ,
    UART_TXPC4_RXPC5        ,

	UART_OUTPUT_CHAL	      ,
	UART_INPUT_CHAL	          ,
};

enum
{
    UART_CMD_GET_CUR_WORK_MODE    =0,
    UART_CMD_SET_ISR_STOP           ,
    UART_CMD_SET_ISR_START          ,
    UART_CMD_SET_CLK_SRC            ,   
    UART_CMD_CHECK_DEVNAME          ,
    UART_CMD_DMA_WR_SADR          ,
    UART_CMD_DMA_WR_EADR          ,
    UART_CMD_DMA_WR_CNT          ,
    UART_CMD_OT          ,
    UART_CMD_INPUT_CHAL,
	UART_CMD_SET_BAUN_RATE,
	UART_CMD_DMA_SENT_DATA,
};

enum
{
    UART_WORKMODE_NORMAL  =0,
    UART_WORKMODE_DMA       ,
};
enum
{
	UART_ISR_TYPE_DATA_COME = 0,
	UART_ISR_TYPE_WRITE_OVER,
	UART_ISR_TYPE_TIMEOUT,
};
typedef struct uart_param
{
    u32 baud_rate;
    u8  io;
    u8 workmode;
    u16 custom;
}__uart_param;

#endif
