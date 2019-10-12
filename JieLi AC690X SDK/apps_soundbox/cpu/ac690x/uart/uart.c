#include "sdk_cfg.h"
#include "uart.h"
#include "cpu/uart_api.h"
#include "uart_param.h"
#include "common/msg.h"
#include "rtos/os_api.h"
#include "dac/eq.h"
#include "fcc_test.h"
#include "clock.h"



__uart_handle uart0_handle;
__uart_handle uart1_handle;
__uart_handle uart2_handle;

__uart_handle *uart_put_handle;
__uart_handle *uart_updata_handle;
__uart_handle *fcc_uart_handle;
__uart_handle *eq_dbg_handle;

s32 uart_set_baud(u32 baud)
{
	uart_module_ctrl(uart_put_handle, UART_CMD_SET_BAUN_RATE,baud,NULL);
	return 0;
}

static int uart_reset(void)
{
    uart_set_baud(UART_BAUD_RAE);
    return 0;
}

static struct clock_switch clock_switch_uart;

CLOCK_SWITCH_USER_REGISTER(CLOCK_SWITCH_EXIT, &clock_switch_uart, uart_reset, "UART");

#define UART0   	0
#define UART1   	1
#define UART2   	2
#define UART_NO		UART0
#define UART_IO		UART_TXPA5_RXPA6	
s32 uart_init(u32 baud)
{
    s32 status = 0;
	__uart_handle *uart_handle;
	char *uart_name;
    uart_put_handle=NULL;

	if(UART_NO == UART0) 
	{
		uart_handle = &uart0_handle;
		uart_name = UART0_HARDWARE_NAME;
	}
	else if(UART_NO == UART1) 
	{
		uart_handle = &uart1_handle;
		uart_name = UART1_HARDWARE_NAME;
	}
	else if(UART_NO == UART2) 
	{
		uart_handle = &uart2_handle;
		uart_name = UART2_HARDWARE_NAME;
	}
	else 
	{
		return UART_DRV_USED_ERR;
	}
    uart_module_on();
#ifdef __DEBUG
    status=uart_module_open(uart_handle,uart_name);
    if(!status)
    {
       __uart_param cur_param;
	   memset(&cur_param,0,sizeof(__uart_param));
       cur_param.baud_rate=baud;
       cur_param.io=UART_IO;
       cur_param.workmode=UART_WORKMODE_NORMAL;
       cur_param.custom = 0;
       status=uart_module_init(uart_handle,&cur_param);
	   if(UART_OUTPUT_CHAL==cur_param.io)///设置要作为串口的GPIO
	   {
		  JL_PORTB->DIR&= ~BIT(9);
		  JL_PORTB->DIE &= ~BIT(9);
		  JL_PORTB->PD |= BIT(9);
		  JL_PORTB->PU |= BIT(9);
	   }
       if(!status)
       {
           status=uart_module_start(uart_handle);
           if(!status)
               uart_put_handle=uart_handle;
       }
    }
	register_handle_printf_putchar(putchar);
#endif

    return status;
}

void fcc_uart_init()
{
	u32 status = 0;
	puts("-----fcc_uart_init\n");
    fcc_uart_handle=NULL;
    status=uart_module_open(&uart1_handle,UART1_HARDWARE_NAME);
    if(!status)
    {
       __uart_param fcc_param;
	   memset(&fcc_param,0,sizeof(__uart_param));
       fcc_param.baud_rate=9600;
       fcc_param.io=  UART_USBP_USBM;
       fcc_param.workmode=UART_WORKMODE_NORMAL;
	   fcc_param.custom |= (BIT(14)|BIT(3));
       status=uart_module_init(&uart1_handle,&fcc_param);
	   uart_reg_isr_callback_fun(&uart1_handle,5,fcc_uart_isr_callback);
	   if(status)
	   {
		   puts("uart_module_init err\n");
	   }
       if(!status)
       {
           status=uart_module_start(&uart1_handle);
           if(!status)
               fcc_uart_handle=&uart1_handle;
       }

    }
}
void fcc_uart_write(char a)
{
	if(fcc_uart_handle)
	{
		fcc_uart_handle->putbyte(fcc_uart_handle,a);
	}
	else
	{
		puts("fcc_uart_write err\n");
	}
}

#if UART_UPDATA_EN

static u8 uart_rx_buf[4];
const u8 uart_updata_flag[4] = {0xEA, 0x55 ,0xAA ,0xAE};
void uart_updata_isr_callback(u8 uto_buf,void *p , u8 isr_flag)
{
	u32 ret;
        ret = memcmp(uart_updata_flag, uart_rx_buf,4);
        if(UART_ISR_TYPE_DATA_COME == isr_flag)
        {
            if(0 == ret)
            {
                puts("----MSG_UPDATA MSG_UPDATA----\n");
                os_taskq_post_msg("MainTask",3,MSG_UPDATA,UART_UPDATA, NULL);
            }
            else
            {
                uart_module_ctrl(uart_updata_handle,UART_CMD_DMA_WR_SADR,sizeof(uart_rx_buf),uart_rx_buf);
                uart_module_ctrl(uart_updata_handle,UART_CMD_DMA_WR_CNT,sizeof(uart_rx_buf),uart_rx_buf);
            }
        }
//        printf_buf(uart_rx_buf,4);
//        printf("ret = %x\n",ret);


}
s32 uart_update_init()
{
	puts("--------uart_update_init\n");
    s32 status;
    uart_updata_handle=NULL;
    status=uart_module_open(&uart1_handle,UART1_HARDWARE_NAME);
    if(!status)
    {
       __uart_param uart_updata_param;
	   memset(&uart_updata_param,0,sizeof(__uart_param));
       uart_updata_param.baud_rate=460800;
       uart_updata_param.io=UART_INPUT_CHAL;
       uart_updata_param.workmode=UART_WORKMODE_DMA;
       uart_updata_param.custom |= BIT(14)|BIT(5)|BIT(3)|BIT(2);//接收超时中断允许(5),接收中断允许(3),发送中断允许(2)
       uart_updata_param.custom |= BIT(7);//清UT1_HRCNT
       status=uart_module_init(&uart1_handle,&uart_updata_param);
	   uart_reg_isr_callback_fun(&uart1_handle,5,uart_updata_isr_callback);
	   /* printf("uart_rx_buf=0x%x,size=%d\n",uart_rx_buf,sizeof(uart_rx_buf) ); */
       uart_module_ctrl(&uart1_handle,UART_CMD_DMA_WR_SADR,sizeof(uart_rx_buf),uart_rx_buf);
       uart_module_ctrl(&uart1_handle,UART_CMD_DMA_WR_EADR,sizeof(uart_rx_buf),uart_rx_buf);
       uart_module_ctrl(&uart1_handle,UART_CMD_DMA_WR_CNT,sizeof(uart_rx_buf),uart_rx_buf);
       uart_module_ctrl(&uart1_handle,UART_CMD_OT,0xffff,NULL);

/// 串口inputchannel函数，可以把UART RX 映射到任意IO口
/// 初始化必须把该IO口设置输入，关闭上下拉，uboot必须对应修改
///IOMC2 BIT(29)-BIT(24):
///0x00-0x0f   PA[0:15]
///0x10-0x1f   PB[0:15]
///0x2b-0x2f   PC[11:15]
///0x30-0x37   PD[0:7]
///0x3e        USBDP
///0x3f        USBDM
	   if(UART_INPUT_CHAL==uart_updata_param.io)///设置要作为串口的GPIO
	   {
		   /*--------------------PA0----------------------*/
           uart_module_ctrl(&uart1_handle,UART_CMD_INPUT_CHAL,/*0x3e*/0,NULL);//0x3e
           PORTA_PU |= BIT(0);
           PORTA_DIR |= BIT(0);

	   }
       if(!status)
       {
           status=uart_module_start(&uart1_handle);
           if(!status)
              uart_updata_handle=&uart1_handle;
       }

    }
    return status;
}

#endif

#if EQ_EN
#if EQ_UART_BDG
/*if you want to change eq uart port ,you can change the #define vlaue below*/
#define eq_uart_handle uart1_handle
#define EQ_UART_NAME   UART1_HARDWARE_NAME
#define EQ_UART_PORT   UART_TXPA1_RXPA2

/*************************************************************************/
static u8 eq_uart_rx_buf[512];

static void eq_dbg_uart_isr_callback(u8 uto_buf, void *p ,u8 isr_flag)    //p rx  buf
{

    static u8 set_eq_flag;

    u8 res;
    if(UART_ISR_TYPE_DATA_COME ==isr_flag)
    {
        if(eq_uart_rx_buf[0]=='E'&&eq_uart_rx_buf[1]=='Q')
        {
            putchar('#');
            set_eq_flag |= BIT(eq_uart_rx_buf[2]);
            if(set_eq_flag&BIT(4))
            {
                 //the last packet length is not the same as before
                res = update_ef_info(eq_uart_rx_buf+4,2140-4*sizeof(eq_uart_rx_buf)-4,eq_uart_rx_buf[2]);
            }
			else
			{
                res = update_ef_info(eq_uart_rx_buf+4,sizeof(eq_uart_rx_buf)-4,eq_uart_rx_buf[2]);
            }

            if(set_eq_flag&BIT(3))
            {
                uart_module_ctrl(&eq_uart_handle,UART_CMD_DMA_WR_SADR,sizeof(eq_uart_rx_buf),eq_uart_rx_buf);
                uart_module_ctrl(&eq_uart_handle,UART_CMD_DMA_WR_EADR,sizeof(eq_uart_rx_buf),eq_uart_rx_buf);
                uart_module_ctrl(&eq_uart_handle,UART_CMD_DMA_WR_CNT,2140-4*sizeof(eq_uart_rx_buf) ,eq_uart_rx_buf);   //LEFT DATA SIZE
            }
            //两包数据都收到&&数据校验OK
            if((res))
            {
                puts("eq_info_rev_success!!!\n");
                set_eq_flag = 0;
                uart_module_ctrl(&eq_uart_handle,UART_CMD_DMA_WR_SADR,sizeof(eq_uart_rx_buf),eq_uart_rx_buf);
                uart_module_ctrl(&eq_uart_handle,UART_CMD_DMA_WR_EADR,sizeof(eq_uart_rx_buf),eq_uart_rx_buf);
                uart_module_ctrl(&eq_uart_handle,UART_CMD_DMA_WR_CNT,sizeof(eq_uart_rx_buf),eq_uart_rx_buf);
            }
        }
    }

    if(UART_ISR_TYPE_TIMEOUT ==isr_flag)
    {
        set_eq_flag = 0;
    }
    uart_module_ctrl(&eq_uart_handle,UART_CMD_OT,54321*1000,NULL);//这里要重设一次避免出错
}

s32 EQ_uart_init()
{

	s32 status;
	eq_dbg_handle = NULL;
    puts("----eq uart init\n");
    status=uart_module_open(&eq_uart_handle,EQ_UART_NAME);
    if(!status)
    {
       __uart_param eq_param;
	   memset(&eq_param,0,sizeof(__uart_param));
       eq_param.baud_rate=115200;
       eq_param.io=EQ_UART_PORT;
       eq_param.workmode=UART_WORKMODE_NORMAL;
       eq_param.custom = 0;
  //     UT1_CON = BIT(13) | BIT(12) |BIT(10)| BIT(6)| BIT(5) |BIT(3)|BIT(0);       //rx_ie , CLR RX PND  6:DMA接收中断
        //接收超时中断允许(5),接收中断允许(3),发送中断允许(2),BIT(7)清UT1_HRCNT
       eq_param.custom |= BIT(13) | BIT(12) |BIT(10)| BIT(7)|BIT(6)| BIT(5) |BIT(3)|BIT(0);
       status=uart_module_init(&eq_uart_handle,&eq_param);
       uart_reg_isr_callback_fun(&eq_uart_handle,2,eq_dbg_uart_isr_callback);
	   /* printf("eq_uart_rx_buf=0x%x,size=%d\n",eq_uart_rx_buf,sizeof(eq_uart_rx_buf) ); */
       uart_module_ctrl(&eq_uart_handle,UART_CMD_DMA_WR_SADR,sizeof(eq_uart_rx_buf),eq_uart_rx_buf);
       uart_module_ctrl(&eq_uart_handle,UART_CMD_DMA_WR_EADR,sizeof(eq_uart_rx_buf),eq_uart_rx_buf);
       uart_module_ctrl(&eq_uart_handle,UART_CMD_DMA_WR_CNT,sizeof(eq_uart_rx_buf),eq_uart_rx_buf);
       uart_module_ctrl(&eq_uart_handle,UART_CMD_OT,80000*1000,NULL);

      if(!status)
       {
           status=uart_module_start(&eq_uart_handle);
           if(!status)
               eq_dbg_handle=&eq_uart_handle;
       }
    }
    return status;
}


#endif
#endif


void putbyte(char a)
{
	if(uart_put_handle)
	{
		if(a=='\n')
		{
       		uart_put_handle->putbyte(uart_put_handle,'\r');
		}
       uart_put_handle->putbyte(uart_put_handle,a);
	}
}

char getbyte()
{
	if(uart_put_handle)
	{
       return uart_put_handle->getbyte(uart_put_handle);
	}
	return 0;
}



void putchar(char a)
{
    if(a == '\n')
    {
        putbyte(0x0d);
        putbyte(0x0a);
    }
    else
    {
        putbyte(a);
    }
}

void put_u4hex(u8 dat)
{
    dat = 0xf & dat;

    if(dat > 9)
    {
        putbyte(dat - 10 + 'A');
    }
    else
    {
        putbyte(dat + '0');
    }
}
void put_u32hex(u32 dat)
{
    putbyte('0');
    putbyte('x');
    put_u4hex(dat >> 28);
    put_u4hex(dat >> 24);

    put_u4hex(dat >> 20);
    put_u4hex(dat >> 16);

    put_u4hex(dat >> 12);
    put_u4hex(dat >> 8);

    put_u4hex(dat >> 4);
    put_u4hex(dat);
    putchar('\n');
}

void put_u32hex0(u32 dat)
{

    put_u4hex(dat >> 28);
    put_u4hex(dat >> 24);

    put_u4hex(dat >> 20);
    put_u4hex(dat >> 16);

    put_u4hex(dat >> 12);
    put_u4hex(dat >> 8);

    put_u4hex(dat >> 4);
    put_u4hex(dat);
}

void put_u64hex(u64 dat)
{
    putbyte('0');
    putbyte('x');
    put_u32hex0(dat>>32);
    put_u32hex0(dat);
    putchar('\n');
}
void put_u16hex(u16 dat)
{
    putbyte('0');
    putbyte('x');


    put_u4hex(dat >> 12);
    put_u4hex(dat >> 8);

    put_u4hex(dat >> 4);
    put_u4hex(dat);
    putchar(' ');
}

void put_u8hex(u8 dat)
{
    put_u4hex(dat >> 4);
    put_u4hex(dat);
    putchar(' ');
}

void printf_buf(u8 *buf, u32 len)
{

    u32 i ;

    for(i = 0 ; i < len ; i++)
    {
        if((i % 16) == 0)
        {
            putbyte('\n') ;
        }

        put_u8hex(buf[i]) ;
    }

    putbyte('\n') ;

}

int puts(const char *out)
{
    while(*out != '\0')
    {
        putchar(*out);
        out++;
    }

    return 0;
}

void put_buf(u8 *buf, u32 len )
{
    u32 i ;
    for(i = 0 ; i < len ; i++)
    {
        if((i%16)==0)
        {
            putchar('\n') ;
        }
        put_u8hex(buf[i]) ;
    }
    putchar('\n') ;
}



