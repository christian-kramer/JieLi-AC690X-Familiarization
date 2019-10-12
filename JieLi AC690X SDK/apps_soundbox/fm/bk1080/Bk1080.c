/*--------------------------------------------------------------------------*/
/**@file     bk1080.c
   @brief    BK1080收音底层驱动
   @details
   @author
   @date   2011-3-30
   @note
*/
/*----------------------------------------------------------------------------*/
#include "sdk_cfg.h"

#if	BK1080

#include "BK1080.h"
#include "iic.h"
#include "fm\fm_api.h"
#include "fm_inside.h"
#include "fm\fm_radio.h"
#include "dac.h"

/*------------BK1080 Initialization Table-----------------*/
/*
const u16 HW_Reg[]=
{
    0x0008,
    0x1080,
#if XTAL_CLOCK	//reg2
    0x1201,	//for internal crystal clock
#else
    0x0201,	//for extern clock
#endif
    0x0000,
    0x40C0,
    0x0A1F,	  //reg5	RSSI[15:8] BAND[7:6] SPACE[5:4],Europe standar
    0x002E,
    0x02FF,
    0x5B11,
    0x0000,
    0x411E,
    0x0000,
    0xCE00,
    0x0000,
    0x0000,
    0x1000,		//reg15
    0x0010,
    0x0000,
    0x13FF,
    0x9852,
    0x0000,
    0x0000,
    0x0008,
    0x0000,
    0x51E1,
    0x28DC,
    0x2645,
    0x00E4,
    0x1CD8,
    0x3A50,
    0xEAF0,
    0x3000,	//reg31
    0x0200,
    0x0000,
};
*/
const u16 HW_Reg[] =
{
    0x0800,
    0x8010,
#if XTAL_CLOCK	//reg2
    0x0112,	//for internal crystal clock
#else
    0x0142,	//for extern clock, system mute
#endif
    0x0000,
    0xC040,
    0x1F0A,	  //reg5	RSSI[15:8] BAND[7:6] SPACE[5:4],Europe standar
    0x2E00,
    0xFF02,
    0x115B,
    0x0000,
    0x1E41,
    0x0000,
    0x00CE,
    0x0000,
    0x0000,
    0x0010,		//reg15
    0x1000,
    0x0000,
    0xFF13,
    0x5298,
    0x0000,
    0x0000,
    0x0800,
    0x0000,
    0xE151,
    0xDC38,
    0x4526,
    0xE400,
    0xD81C,
    0x503A,
    0xF0EA,
    0x0030,	//reg31
    0x0002,
    0x0000,
};

/*----------------------------------------------------------------------------*/
/**@brief    BK1080读寄存器函数
   @param    num 需要的数目
   @return   无
   @note     void BEKEN_I2c_Read(u8 reg,u8 *pBuf,u8 num)
*/
/*----------------------------------------------------------------------------*/
static void BEKEN_I2c_Read(u8 reg, u8 *pBuf, u8 num)
{
    reg = reg<<1;
    reg |= 0x01;
    app_IIC_readn(CHIP_DEV_ID,reg,pBuf,num);
}

/*----------------------------------------------------------------------------*/
/**@brief    BK1080读寄存器函数
   @param    num 需要的数目
   @return   无
   @note     void BEKEN_I2c_Write(u8 reg,u8 *pBuf,u8 num)
*/
/*----------------------------------------------------------------------------*/
static void BEKEN_I2c_Write(u8 reg, u8 *pBuf, u8 num)
{
    reg = reg<<1;
    app_IIC_write(CHIP_DEV_ID,reg,pBuf,num);
}

/*----------------------------------------------------------------------------*/
/**@brief    BK1080 初始化
   @param    无
   @return   无
   @note     void init_BK1080(void)
*/
/*----------------------------------------------------------------------------*/
void bk1080_init(void)
{
    u8 temp[2];
    //delay_n10ms(100);	//delay 1s,please revise delay time according to your MCU

    BEKEN_I2c_Write(0,(u8 *)HW_Reg,68);	//start from reg2,total 60 byte

    delay_n10ms(25);			//delay 250ms
    temp[0] = HW_Reg[25];
    temp[1] = HW_Reg[25] >> 8;
    temp[1] &= 0x7f;
    BEKEN_I2c_Write(25,temp,2);

    temp[1] |= 0x80;
    BEKEN_I2c_Write(25,temp,2);

    dac_channel_on(FM_IIC_CHANNEL, FADE_ON);
    
    delay_n10ms(5);			//delay 50ms
}


/*----------------------------------------------------------------------------*/
/**@brief    设置一个频点 BK1080
   @param    curFreq：设置频点
   @return   无
   @note     void bk1080_setfreq(u16 curFreq)
*/
/*----------------------------------------------------------------------------*/
void bk1080_setfreq(u16 curFreq)
{
    u8 curChan;
    u16 temp;

    curChan = curFreq - MIN_FRE;

    temp = curChan << 8;
    BEKEN_I2c_Write(0x03,(u8 *)&temp,2);   //write reg3,with 2 bytes

    temp |= 0x80;
    BEKEN_I2c_Write(0x03,(u8 *)&temp,2);
}

/*----------------------------------------------------------------------------*/
/**@brief    设置一个频点BK1080
   @param    fre 频点  875~1080
   @return   1：当前频点有台，0：当前频点无台
   @note     bool bk1080_set_fre(u16 freq)
*/
/*----------------------------------------------------------------------------*/
bool bk1080_set_fre(u16 freq)
{
    _no_init static u16 _data last_tuned_freq;
    _no_init static u16 _data g_last_freq_deviation_value;
    u8 temp[4];
    u16 cur_freq_deviation;

    bk1080_setfreq(freq);
    delay_n10ms(4);		//延时时间>=30ms

///////////////////////////////////////////////////////////////////
//new added 2009-05-30

    BEKEN_I2c_Read(0x07,temp,2);	//start from reg 0x7,with 2bytes

    cur_freq_deviation = (temp[0] << 8) | temp[1];
    cur_freq_deviation = cur_freq_deviation>>4;

    BEKEN_I2c_Read(0x0a,&temp[2],2);

/////////////////////////////////////////////////////////////////////
    if (temp[2]&0x10)    			//check AFCRL bit12
    {
        last_tuned_freq = freq;			//save last tuned freqency
        g_last_freq_deviation_value = cur_freq_deviation;
        return 0;
    }

	if(temp[3]<8) //RSSI<10   //搜台较少可减少该值，假台较多可增大该值    //8
	{
		last_tuned_freq=freq;			//save last tuned freqency
		g_last_freq_deviation_value=cur_freq_deviation;
		return 0;
	}

    if ( (temp[1]&0x0f) <2) //SNR<2 //搜台较少可减少该值，假台较多可增大该值
    {
        last_tuned_freq = freq;			//save last tuned freqency
        g_last_freq_deviation_value = cur_freq_deviation;
        return 0;
    }
/////////////////////////////////////////////////////////////////////////
//add frequency devation check
	if( (cur_freq_deviation>=192)&&(cur_freq_deviation<=(0xfff-192)))	  //0x64 //搜台较少可减少该值，假台较多可增大该值
	{
		last_tuned_freq=freq;			//save last tuned freqency
		g_last_freq_deviation_value=cur_freq_deviation;

        return 0;
    }

///////////////////////////////////////////////////////////////////////////
//new added 2009-05-30
    if ( (freq > 875)&&((freq-last_tuned_freq) == 1) )//start_freq)&&( (freq-last_tuned_freq)==1) )
    {
        if (g_last_freq_deviation_value&0x800)
        {
            last_tuned_freq = freq;		//save last tuned freqency
            g_last_freq_deviation_value = cur_freq_deviation;
            return 0;
        }

        if (g_last_freq_deviation_value < 150)	 //搜台较少可减少该值，假台较多可增大该值
        {
            last_tuned_freq = freq;		//save last tuned freqency
            g_last_freq_deviation_value = cur_freq_deviation;
            return 0;
        }
    }

    if ( (freq >= 875)&&( (last_tuned_freq - freq)==1) )//start_freq)&&( (last_tuned_freq - freq)==1) )
    {
        if ( (g_last_freq_deviation_value&0x800)==0)
        {
            last_tuned_freq = freq;		//save last tuned freqency
            g_last_freq_deviation_value = cur_freq_deviation;
            return 0;
        }

        if (g_last_freq_deviation_value>(0xfff-150) ) //搜台较少可减少该值，假台较多可增大该值
        {
            last_tuned_freq = freq;		//save last tuned freqency
            g_last_freq_deviation_value = cur_freq_deviation;
            return 0;
        }
    }

    last_tuned_freq = freq;				//save last tuned freqency
    g_last_freq_deviation_value = cur_freq_deviation;

    return 1;
}


/*----------------------------------------------------------------------------*/
/**@brief    关闭 BK1080的电源
   @param    无
   @return   无
   @note     void BK1080_PowerDown(void)
*/
/*----------------------------------------------------------------------------*/
void bk1080_powerdown(void)
{
    u16 temp = 0x4102;
    //temp[0] = 0x02;			  //write 0x0241 into reg2
    //temp[1] = 0x41;
    BEKEN_I2c_Write(0x02,(u8 *)&temp,2);

    dac_channel_off(FM_IIC_CHANNEL, FADE_ON);
    delay_n10ms(5);				  //delay 50ms
}

/*----------------------------------------------------------------------------*/
/**@brief    FM 模块静音控制
   @param    dir：1-mute 0-unmute
   @return   无
   @note     void BK1080_mute(dir)
*/
/*----------------------------------------------------------------------------*/
void bk1080_mute(u8 flag)
{
    u8 TmpData8[2];

    BEKEN_I2c_Read(2,TmpData8,2);
    if (flag)
        TmpData8[0] |= 0x40;//mute
    else
        TmpData8[0] &= 0xbf;

    BEKEN_I2c_Write(2,TmpData8, 2);
}

/*----------------------------------------------------------------------------*/
/**@brief   FM模块检测，获取BK1080 模块ID
   @param   无
   @return  检测到BK1080模块返回1，否则返回0
   @note    bool BK1080_Read_ID(void)
*/
/*----------------------------------------------------------------------------*/
bool bk1080_read_id(void)
{
    u16 bk_id;

    BEKEN_I2c_Read(0x01,(u8 *)&bk_id,2); 	//read reg3,with 2 bytes

    if (0x8010 == bk_id)
    {
        return TRUE;
    }
    return FALSE;
}
#endif
