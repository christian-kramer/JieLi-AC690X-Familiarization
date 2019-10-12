
#include "sdk_cfg.h"
#include "fcc_test.h"
#include "irq_api.h"
#include "cpu/uart_api.h"
#include "uart_param.h"
//#include "clock.h"
//#include "hwi.h"

//在这里要添加uart和电脑通信获取fcc配置

//usb口用作uart 使用uart1

//接收到数据回pc的状态
#define NACK  0xdd
#define ACK   0xcc

typedef struct _UartComm{
    u8 fcc_uart_sync[3]; //0xaa01bb
    u8 start;
    u16 len;
    u8 packet_type;    //DH135  2-DH135
	u8 channel;       //信道  0 - 78
	u8 data_type;      //PN9   单载
	u8 hopping_mode;       //跳频开关
	u8 tx_power;      //发射功率 0 - 15
	u8 sum;
	u8 end;
}__attribute__((packed)) UartComm;


__attribute__((used)) UartComm fcc_uart_buf;
u8 writepos =0;//记录buf写的位置

#define TX_MODE  0
#define RX_MODE  1

#define DH1_1        0
#define DH3_1        1
#define DH5_1        2
#define DH1_2        3
#define DH3_2        4
#define DH5_2        5

#define HOP_OFF  0
#define HOP_ON   1

#define DATE_PN9               0
#define DATE_SINGLE_CARRIER    1

/******初始化配置參數選擇***********/
#define TRX_MODE               TX_MODE
#define FRE_CHANNEL            0
#define PACKET_TYPE            DH1_1
#define DATA_TYPE              DATE_SINGLE_CARRIER
#define TRANSMIT_POWER         6
#define HOP_MODE               HOP_OFF
/*void __set_fcc_default_info(u8 trx_mode,u8 fre,u8 p_type,u8 d_type,u8 tx_power,hop_mode)
  trx_mode 1:rx  0:tx；
  fre:中心频率2402+fre
  p_type 0-5:dh1 dh3 dh5 2dh1 2dh3 2dh5
  d_type 1：single carrier  0:伪随机
  tx_power:发射功率
  hop_mode:跳频模式
*/
//fcc 默认初始化配置
void test_fcc_default_info()
{
	puts("test_fcc_default_info\n");
    __set_fcc_default_info(
                          TRX_MODE,
                          FRE_CHANNEL,
                          PACKET_TYPE,
                          DATA_TYPE,
                          TRANSMIT_POWER,
                          HOP_MODE);
}

void fcc_uart_isr_callback(u8 uto_buf,void *p, u8 isr_flag)
{
    u8 * ptr;
    if( UART_ISR_TYPE_DATA_COME == isr_flag)
    {
        ptr = (u8 *)&fcc_uart_buf;
        ptr[writepos] = uto_buf;
        /* printf("uto_buf=0x%x\n",ptr[writepos]); */
        //接收 sync
        if(writepos == 0)
        {
            if(ptr[writepos]==0xaa)
            {
                writepos=1;
            }
        }
        else if(writepos == 1)
        {
            if(ptr[writepos]==0x01)
            {
                writepos=2;
            }
            else
            {
                writepos=0;
            }
        }
        else if(writepos == 2)
        {
            if(ptr[writepos]==0xbb)
            {
                fcc_uart_write(ACK); //接收 sync 0xaa01bb 正确
                bt_putchar('S');
                writepos=3;
            }
            else
            {
                writepos=0;
            }
        }
        else  //接收有效数据
        {
            writepos++;
            if(writepos >= sizeof(UartComm))
            {
                writepos = 0;
                set_test_fcc_info(ptr);
            }
        }

        if(writepos >= sizeof(UartComm))
        {
            writepos = 0;
        }
    }
}
void set_test_fcc_info(u8 *buf)
{
    u32 sum;
    u8 trx_mode_temp;
    UartComm *p;

    p=(UartComm *)buf;
    sum = p->packet_type+p->channel+p->data_type+
            p->hopping_mode+p->tx_power;

    if(sum==p->sum)
    {
        fcc_uart_write(ACK);
        //bt_putchar('A');
        //put_u16hex(sizeof(UartComm));
        if(p->tx_power&BIT((7))) //接收模式  slave
        {
            trx_mode_temp = 1;
        }
        else    //发送模式   master
        {
            trx_mode_temp = 0;
        }

        __set_fcc_default_info(trx_mode_temp,
                               p->channel,
                               p->packet_type,
                               p->data_type,
                               p->tx_power,
                               p->hopping_mode);
         puts("----set_test_fcc_info over\n");
        printf("trx_mode :%d \n",trx_mode_temp);
       // printf("tx_fre :%d \n",p->tx_fre);
        printf("packet_type :%d \n",p->packet_type);
        printf("data_type :%d \n",p->data_type);
        printf("tx_power :%d \n",p->tx_power);
        printf("hopping_mode :%d \n",p->hopping_mode);


    }
    else
    {
        fcc_uart_write(NACK);
       // bt_putchar('N');
    }

  //  put_buf(buf,sizeof(UartComm));

}
