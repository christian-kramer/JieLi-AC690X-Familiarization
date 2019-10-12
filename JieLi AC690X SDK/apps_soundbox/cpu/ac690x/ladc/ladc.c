/*
*********************************************************************************************************
*                                             BR17
*
*                                             CODE
*
*                          (c) Copyright 2015-2016, ZHUHAI JIELI
*                                           All Rights Reserved
*
* File : *
* By   : jamin.li
* DATE : 11/11/2015 build this file
*********************************************************************************************************
*/
#include "sdk_cfg.h"
#include "cbuf/circular_buf.h"
#include "dac/dac.h"
#include "dac/ladc.h"
#include "rtos/task_manage.h"
#include "aec/aec_api.h"
#include "encode/encode.h"
#include "dac/dac_api.h"
#include "cpu/dac_param.h"
#include "linein.h"
#define	LADC_DATA_2_DAC     0  ///define for debug

static cbuffer_t adc_cb;
//extern volatile u32 dac_energy_value;

/*
 *********************************************************************************************************
 *
 * Description: ladc buf初始化
 * Arguments  : none
 *
 * Returns    : none
 *
 * Note: 上电只做一次，资源不会释放
 *********************************************************************************************************
 */
void ladc_buf_init(void)
{
    u8 *outbuf;
    outbuf = malloc(1024);
    if (outbuf == 0)
    {
        puts("out buf malloc err\r\n");
    }

    cbuf_init(&adc_cb, outbuf, 512);
}

/*
 *********************************************************************************************************
 *
 * Description: ladc采样 资源申请，配置采样回调函数
 * Arguments  : max_channel--采样通道个数, channel--采样通道类型, call_back--采样输出回调
 *
 * Returns    : ladc 结构指针
 *
 * Note: 在 ladc_reg_init之前调用，申请的资源由调用者主动是否
 *********************************************************************************************************
 */
LADC_CTL * ladc_ctl_init(u8 max_channel, u8 channel, void * call_back)
{
    LADC_CTL *ladc;
    u8 i;

    ladc = (LADC_CTL *) calloc(1, sizeof(LADC_CTL));
    ASSERT(ladc);

    ladc->source_chl = channel;

    ///输入C buffer申请：rec_ctl->ladc.c_buff
    for (i = 0; i < max_channel; i++)
    {
        ladc->buff[i] = calloc(1, DAC_DUAL_BUF_LEN * 50);
        ASSERT(ladc->buff[i]);

        ladc->c_buff[i] = calloc(1, sizeof(cbuffer_t)); //<申请保存LADC buffer的C buffer
        ASSERT(ladc->c_buff[i]);

        cbuf_init(ladc->c_buff[i], ladc->buff[i], DAC_DUAL_BUF_LEN * 50);
    }

    ladc->save_ladc_buf = (void(*)(void *, void *, u8, u32)) call_back; //写LADC数据到C buffer的函数

    return ladc;
}


/*
 *********************************************************************************************************
 *
 * Description: ladc采样资源释放
 * Arguments  : none
 *
 * Returns    : none
 *
 * Note: 在 disable_ladc 之后调用,上层不调用该函数，也可按结构体资源自行释放
 *********************************************************************************************************
 */
void ladc_ctl_free(LADC_CTL *ladc)
{
    u8 i;

    if(ladc != NULL)
    {
        for (i = 0; i < 4; i++)
        {
            if(NULL != ladc->c_buff[i])
            {
                free(ladc->c_buff[i]);
                ladc->c_buff[i] = NULL;
            }

            if(NULL != ladc->buff[i])
            {
                free(ladc->buff[i]);
                ladc->buff[i] = NULL;
            }
        }

        free(ladc);
    }
}
/*
 *********************************************************************************************************
 *
 * Description: read ladc cbuff采样数据
 * Arguments  : s16 * buf, u32 len
 *
 * Returns    : 实际读到的数据长度
 *
 * Note:
 *********************************************************************************************************
 */
u32 cbuf_read_adc(s16 * buf, u32 len)
{
    return cbuf_read(&adc_cb, buf, len);
}

/*
 *********************************************************************************************************
 *
 * Description: 单通道变双通道，数据copy
 * Arguments  : dest_buf,buf,point_cnt--采样点数
 *
 * Returns    : none
 *
 * Note:
 *********************************************************************************************************
 */

void ladc_one_to_two(u16 *dest_buf,u16 *buf,u16 point_cnt)
{
    int cnt,cnt2;
    cnt2 = 0;
    for(cnt = 0; cnt < point_cnt; cnt++)
    {
        dest_buf[cnt2] = buf[cnt];
        dest_buf[cnt2+1] = buf[cnt];
        cnt2+=2;
    }
}

/*
 *********************************************************************************************************
 *
 * Description: ladc dual channel,combine ladc channel buf 
 * Arguments  : ladc_l = left adc channel buf 
 *				ladc_r = right adc channel buf
 * Returns    : none
 *
 * Note:
 *********************************************************************************************************
 */
void ladc_stereo(s16 *ladc_l,s16 *ladc_r,u16 point_cnt,s16 *dest_buf)
{
	u8 i;
	for(i = 0; i < point_cnt; i++)
	{
		dest_buf[i*2]   = ladc_l[i];
		dest_buf[i*2+1] = ladc_r[i];
	}
}

/*
 *********************************************************************************************************
 *
 * Description: calculate data energy  
 * Arguments  : buffer = source_data  
 *				len = data length(byte)
 *				packet_cnt = total calculate packets
 * Returns    : none
 *
 * Note:
 *********************************************************************************************************
 */
static s32 abs(s32 a)
{
    if(a > 0){
        return a;
    }else{
        return (-a);
    }
}
u32 data_energy_value(void *buffer,u16 len,u16 packet_cnt)
{
    static u32 total_res = 0;
	static u32 digital_energy = 0;
    static u16 cnt = 0;
    u32 res = 0;
    u32 i;

    s16 *buf = buffer;
    len >>= 1;//convert to point


    for(i = 0; i < len; i++)
    {
        res += abs(buf[i]);
    }
    total_res += res;
    cnt++;
    if (cnt >= packet_cnt)
    {
        cnt = 0;
        digital_energy = total_res;
        total_res = 0;
    }
    return digital_energy;
}

#if 0
static const u8 sine_buf_32K[] =
{
    /* 0x00, 0x00, 0x00, 0x00, 0xae, 0x11, 0xae, 0x11, 0xad, 0x22, 0xad, 0x22, 0x58, 0x32, 0x58, 0x32, 0x13, */
    /* 0x40, 0x13, 0x40, 0x58, 0x4b, 0x58, 0x4b, 0xb8, 0x53, 0xb8, 0x53, 0xe0, 0x58, 0xe0, 0x58, 0x9e, 0x5a, */
    /* 0x9e, 0x5a, 0xe0, 0x58, 0xe0, 0x58, 0xb8, 0x53, 0xb8, 0x53, 0x58, 0x4b, 0x58, 0x4b, 0x13, 0x40, 0x13, */
    /* 0x40, 0x58, 0x32, 0x58, 0x32, 0xad, 0x22, 0xad, 0x22, 0xae, 0x11, 0xae, 0x11, 0x00, 0x00, 0x00, 0x00, */
    /* 0x52, 0xee, 0x52, 0xee, 0x53, 0xdd, 0x53, 0xdd, 0xa8, 0xcd, 0xa8, 0xcd, 0xed, 0xbf, 0xed, 0xbf, 0xa8, */
    /* 0xb4, 0xa8, 0xb4, 0x48, 0xac, 0x48, 0xac, 0x20, 0xa7, 0x20, 0xa7, 0x62, 0xa5, 0x62, 0xa5, 0x20, 0xa7, */
    /* 0x20, 0xa7, 0x48, 0xac, 0x48, 0xac, 0xa8, 0xb4, 0xa8, 0xb4, 0xed, 0xbf, 0xed, 0xbf, 0xa8, 0xcd, 0xa8, */
    /* 0xcd, 0x53, 0xdd, 0x53, 0xdd, 0x52, 0xee, 0x52, 0xee */

	0x00, 0x00, 0xae, 0x11, 0xad, 0x22, 0x58, 0x32,
	0x13, 0x40, 0x58, 0x4b, 0xb8, 0x53, 0xe0, 0x58,
	0x9e, 0x5a, 0xe0, 0x58, 0xb8, 0x53, 0x58, 0x4b,
	0x13, 0x40, 0x58, 0x32, 0xad, 0x22, 0xae, 0x11,
	0x00, 0x00, 0x52, 0xee, 0x53, 0xdd, 0xa8, 0xcd,
	0xed, 0xbf, 0xa8, 0xb4, 0x48, 0xac, 0x20, 0xa7,
	0x62, 0xa5, 0x20, 0xa7, 0x48, 0xac, 0xa8, 0xb4,
	0xed, 0xbf, 0xa8, 0xcd, 0x53, 0xdd, 0x52, 0xee
};
#endif
extern void aux_ladc_cbuf_write(u8 *data,u16 length);
void ladc_to_dac(void * buf,u32 len)
{
	u8 l2d_buf[DAC_DUAL_BUF_LEN*2];
	s16 * sp = buf;//(void*)sine_buf_32K;//buf;
	s16 * dp = (s16 *)l2d_buf;
	u8 cnt;

	for(cnt = 0; cnt<(len/2); cnt++)
	{
		dp[cnt*2]=sp[cnt];
		dp[cnt*2+1]=sp[cnt];
	}
	/* cbuf_write_dac(sine_buf_32K,sizeof(sine_buf_32K)); */

#if USE_SOFTWARE_EQ
	aux_ladc_cbuf_write((u8*)dp,len*2);
#else	
	cbuf_write_dac(dp,len*2);
#endif
}

void ladc_stereo_to_dac(void *buf_l,void *buf_r,u32 len)
{
	u8 l2d_buf[DAC_DUAL_BUF_LEN*2];
	s16 * sp_l = buf_l;
	s16 * sp_r = buf_r;
	s16 * dp = (s16 *)l2d_buf;
	u8 cnt;

	for(cnt = 0; cnt<(len/2); cnt++)
	{
		dp[cnt*2]=sp_l[cnt];
		dp[cnt*2+1]=sp_r[cnt];
	}
#if USE_SOFTWARE_EQ
	aux_ladc_cbuf_write((u8*)dp,len*2);
#else	
	cbuf_write_dac(dp,len*2);
#endif
}

//*********************************************************
//                   interrupt
//*********************************************************
static void sr_16k_2_8K(void * buf, u32 len)
{
	u16 i;
    s16 *res = buf;
	len >>= 2;//	x/2point 	x/2/2->16K28K

	for(i = 0; i < len; i++)
	{
		res[i] = res[i*2];
	}
}

static void ladc_input_fun(LADC_CTL *ladc_p, void *ladc_buf,u32 buf_flag,u32 buf_len)
{
    u8  i;
    s16 *res;
	res = (s16*)ladc_buf;

    if (NULL != ladc_p)
    {
        if (!ladc_p->save_ladc_buf) {
			puts("WARNING:ladc_p->save_ladc_buf is NULL\n");
			return;
		}

        if (ladc_p->source_chl == ENC_MIC_CHANNEL)
        {
             //mic单声道
            ladc_p->save_ladc_buf((void *)ladc_p, res, 0, DAC_DUAL_BUF_LEN);
        }
        else if(ladc_p->source_chl == ENC_LINE_LEFT_CHANNEL)
        {
            for(i = 0; i < 2; i++) //左右声道
            {
                ladc_p->save_ladc_buf((void *) ladc_p, res, i, DAC_DUAL_BUF_LEN);
            }
        }
        else if(ladc_p->source_chl == ENC_LINE_RIGHT_CHANNEL)
        {
            for(i = 0; i < 2; i++) //左右声道
            {
                ladc_p->save_ladc_buf((void *) ladc_p, res, i, DAC_DUAL_BUF_LEN);
            }
        }
        else if(ladc_p->source_chl == ENC_LINE_LR_CHANNEL)
        {
            //左右声道
            ladc_p->save_ladc_buf((void *) ladc_p, res, 0, DAC_DUAL_BUF_LEN);
            ladc_p->save_ladc_buf((void *) ladc_p, (res+DAC_DUAL_BUF_LEN), 1, DAC_DUAL_BUF_LEN);
        }
        else {
            /* putchar('E'); */
        }
    }
}

void ladc_isr_callback(void *ladc_buf,u32 buf_flag,u32 buf_len)
{
    s16 *res;
	res = (s16*)ladc_buf;
	//u32 energy_value = 0;

    ///*********录音用***************///
	ladc_input_fun(p_ladc, ladc_buf, buf_flag, buf_len);
	ladc_input_fun(p_ladc_rec, ladc_buf, buf_flag, buf_len);

#if AUX_AD_ENABLE
	if((LINEIN_CHANNEL >= DAC_AMUX0)&&(LINEIN_CHANNEL <= DAC_AMUX2)) {
		//putchar('2');//ladc stereo
		ladc_stereo_to_dac(res,(res+DAC_DUAL_BUF_LEN),DAC_DUAL_BUF_LEN);
	} else {
		//putchar('1');//ladc mono
		ladc_to_dac(ladc_buf, DAC_DUAL_BUF_LEN); 
	}
#endif

#if LADC_DATA_OUPUT_DAC_SW
	s16 tmp_ladc_buf[DAC_SAMPLE_POINT * 2] ;
	/*ENC_LINE_LEFT_CHANNEL, ENC_LINE_RIGHT_CHANNEL or ENC_MIC_CHANNEL*/
	ladc_one_to_two((u16 *)tmp_ladc_buf, res, DAC_SAMPLE_POINT);
	/*ENC_LINE_LR_CHANNEL*/
	//ladc_stereo(res,res+DAC_DUAL_BUF_LEN,DAC_SAMPLE_POINT,tmp_ladc_buf);
    cbuf_write_dac(tmp_ladc_buf,DAC_BUF_LEN);
#endif

    ///*********蓝牙用***************///
	if (curr_task) {
	    //energy_value = data_energy_value((s16 *)res, DAC_SAMPLE_POINT*2,1);

	    if (!strcmp(curr_task->name, "BtStackTask"))
	    {
             if(aec_interface.fill_adc_ref_buf)
             {
#if BT_KTV_EN
				 sr_16k_2_8K(res, 32 * sizeof(short));
                 aec_interface.fill_adc_ref_buf(res, 32 * sizeof(short) / 2); //回声消除
#else
                 aec_interface.fill_adc_ref_buf(res, 32 * sizeof(short)); //回声消除
#endif
             }
         }
#if USB_PC_EN
	    if (!strcmp(curr_task->name, UDISK_TASK_NAME))
		{
			usb_mic_ladc_input(res,DAC_SAMPLE_POINT);
		}
#endif
     }
}

//*********************************************************
//                        LADC API
//*********************************************************
void ladc_reg_init(u8 ch_sel, u16 sr)
{
	ladc_reg_isr_cb_api(ladc_isr_callback);
	ladc_enable(ch_sel,sr,VCOMO_EN);
}

void ladc_close(u8 ch_sel)
{
	ladc_disable(ch_sel);
}


//*********************************************************
//                        DAA API
//*********************************************************
void microphone_open(u8 mic_gain,u8 mic_gx2)
{
	//puts("microphone_open\n");

	dac_mute(1,1);

	//init mic IO
	JL_PORTB->DIR |=  BIT(13);
	JL_PORTB->DIE &= ~BIT(13);	
	JL_PORTB->PD  &= ~BIT(13);	
	JL_PORTB->PU  &= ~BIT(13);	

	ladc_adcude(1);
	ladc_mic_gain(mic_gain,mic_gx2);
	ladc_mic_en(1);
	mic_2_LR(1,1);
	dac_mute(0,1);
	ladc_adcude(0);
}

void microphone_close(void)
{
	//puts("microphone_close\n");
	ladc_mic_en(0);
	mic_2_LR(0,0);
}
