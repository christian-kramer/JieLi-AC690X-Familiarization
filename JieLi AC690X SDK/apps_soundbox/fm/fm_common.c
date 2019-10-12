#include "sdk_cfg.h"
#include "fm/fm_radio.h"
#include "fm/fm_api.h"
#include "vm/vm_api.h"
#include "vm/vm.h"
#include "fm_prompt.h"
#include "common/common.h"

#if FM_RADIO_EN

/*----------------------------------------------------------------------------*/
/**@brief   获取一个byte中有几个位被置一
   @param   byte ：所传进去的byte
   @return  被置一位数
   @note    tu8 get_total_mem_channel(void)
*/
/*----------------------------------------------------------------------------*/
tu8 my_get_one_count(u8 byte)
{
    u8  i,b=0;
    for (i = 0; i < 8; i++)
    {
        if (byte & BIT(7))
        {
            b++;
        }
        byte <<= 1;                   //数据左移一位,进入下一轮送数
    }
    return b;
}
/*----------------------------------------------------------------------------*/
/**@brief 获取全部记录的频道
   @param 	无
   @return  频道总数
   @note  tu8 get_total_mem_channel(void)
*/
/*----------------------------------------------------------------------------*/
tu8 get_total_mem_channel(void)
{
    u8 i,j;
    u8 total;
    total = 0;
    for (i = 0; i < MEM_FM_LEN; i++)
    {
        j = fm_info->dat[FM_CHANNL + i];
        total += my_get_one_count(j);
    }
    if (total > MAX_CHANNL)
        total = MAX_CHANNL;

    return total;
}
/*----------------------------------------------------------------------------*/
/**@brief 根据频点偏移量获取频道
   @param 	channel：频道
   @return  频道
   @note  tu8 get_channel_via_fre(u8 fre)

*/
/*----------------------------------------------------------------------------*/
tu8 get_channel_via_fre(u8 fre)
{
    u8 i,j, k;
    u8 total;
    total = 0;
    for (i = 0; i < MEM_FM_LEN; i++)
    {
        j = fm_info->dat[FM_CHANNL + i];
        for (k = 0; k < 8; k++)
        {
            if (j & (BIT(k)))
            {
                total++;
                if (fre == (i * 8 + k))
                {
                    return total;		 //return fre index
                }
            }
        }
    }
    return  0x00;//fm_mode_var->wFreChannel;						    //find none
}
/*----------------------------------------------------------------------------*/
/**@brief 通过频道获取频点
   @param 	channel：频道
   @return  有效的频点偏移量
   @note  u8 get_fre_via_channle(u8 channel)
*/
/*----------------------------------------------------------------------------*/
tu8 get_fre_via_channle(u8 channel)
{
    u8 i,j, k;
    u8 total;

    total = 0;
    for (i = 0; i < MEM_FM_LEN; i++)
    {
        j = fm_info->dat[FM_CHANNL + i];
        for (k = 0; k < 8; k++)
        {
            if (j & (BIT(k)))
            {
                total++;
                if (total == channel)
                {
                    return i * 8 + k;		 //fre = MIN_FRE + return val
                }
            }
        }

    }
    return 0xff;							//find none
}

/*----------------------------------------------------------------------------*/
/**@brief 从vm清除所有频点信息
   @param 	无
   @return  无
   @note  void clear_all_fm_point(void)
*/
/*----------------------------------------------------------------------------*/
void clear_all_fm_point(void)
{
    u8 i;

    for (i = FM_CHANNL; i <= (FM_CHANNL + MEM_FM_LEN); i++)
    {
        fm_info->dat[i]=0x00;
    }
    fm_save_info();
}

/*----------------------------------------------------------------------------*/
/**@brief 根据频点偏移量保存到相应的频点位变量到vm
   @param 	fre：频点偏移量
   @return  无
   @note  void save_fm_point(u8 fre)
*/
/*----------------------------------------------------------------------------*/
void save_fm_point(u8 fre)
{
    u8 i,j, k;
    i = fre / 8;
    k = fre % 8;
    fm_read_info();
    j=fm_info->dat[FM_CHANNL + i]| BIT(k);
    fm_info->dat[FM_CHANNL+i]=j;
    fm_save_info();
}

/*----------------------------------------------------------------------------*/
/**@brief 删除频道
   @param 	无
   @return  无
   @note  void delete_fm_point(u8 fre)
*/
/*----------------------------------------------------------------------------*/
void delete_fm_point(u8 fre)
{
    u8 i,j,k;
    i = fre / 8;
    k = fre % 8;
    j=fm_info->dat[FM_CHANNL + i] & (~BIT(k));
    fm_info->dat[FM_CHANNL + i]=j;
    fm_save_info();
}
/*----------------------------------------------------------------------------*/
/**@brief 保存频道
   @param 	无
   @return  无
   @note  u8 ch_save(void)
*/
/*----------------------------------------------------------------------------*/
void ch_save(void)
{
    save_fm_point(fm_mode_var->wFreq - MIN_FRE);
    fm_mode_var->wFreChannel = get_channel_via_fre(fm_mode_var->wFreq - MIN_FRE);
    fm_mode_var->wTotalChannel = get_total_mem_channel();
}

/*----------------------------------------------------------------------------*/
/**@brief  保存信息到fm_buf
   @param  无
   @return 无
   @note  void fm_save_info()
*/
/*----------------------------------------------------------------------------*/
void fm_save_info(void)
{
    vm_write_api(VM_FM_INFO, fm_info);
}

/*----------------------------------------------------------------------------*/
/**@brief  从vm读取信息
   @param  无
   @return 无
   @note  void fm_read_info()
*/
/*----------------------------------------------------------------------------*/
void fm_read_info(void)
{
    if (-VM_READ_NO_INDEX == vm_read_api(VM_FM_INFO, fm_info))
    {
        puts("fm_info is null\n");
        memset(fm_info,0x00,sizeof(FM_INFO));
    }
}

#endif
