#include "common/ASCII_lib.h"


/*----------------------------------------------------------------------------*/
/** @brief:
    @param:
    @return:
    @author:Juntham
    @note:
    @date: 2012-06-08,10:16
*/
/*----------------------------------------------------------------------------*/

//void ASCII_ToUpper(void *buf, u32 len)
//{
//    while (len--)
//    {
//        if ((((u8 *)buf)[len] >= 'a') && (((u8 *)buf)[len] <= 'z'))
//        {
//            ((u8 *)buf)[len] -= 'a' - 'A';
//        }
//    }
//}

/*----------------------------------------------------------------------------*/
/** @brief:字符串匹配，文件名可以使用?来匹配
    @param:
    @return:
    @author:Juntham
    @note:
    @date: 2012-06-08,14:52
*/
/*----------------------------------------------------------------------------*/

u32 ASCII_StrCmp(const u8 *src, const u8 *dst, u32 len)
{
    u32 m = len; 			//用于处理文件名中'*'
    while (len--)
    {
        if ((*src == '\0') && (*dst== '\0') )				//同时结束
        {
            return 0;
        }
        if ((*src == '\0') || (*dst== '\0'))	//有一方提前结束
        {
            return m-len;
        }

        if (*src != *dst)							//比较不相等
        {
            if ((*dst != '?') && (*src != '?'))
            {
                return m-len;
            }
        }

        src++;
        dst++;
    }
    return 0;

}


void ASCII_IntToStr(void *pStr , u32 intNum , u32 strLen)
{
    while(strLen--)
    {
        ((u8 *)pStr)[strLen] = '0' + intNum%10;
        intNum/=10;
    }
}


u32 ASCII_StrToInt(const void *pStr , u32 *pRint, u32 strLen)
{
    u32 m;

    for(m=1,*pRint=0; strLen--; m*=10)
    {
        if(((u8 *)pStr)[strLen]<'0' || ((u8 *)pStr)[strLen]>'9')
        {
            return strLen;
        }
        *pRint += (((u8 *)pStr)[strLen]-'0') * m;
    }

    return 0;
}


//u32 ASCII_StrLen(void *str , u32 len)
//{
//    u32 i;
//
//    for(i=0 ; i< len ; i++)
//    {
//        if(((u8   *)str)[i] ==0)
//        {
//            break;
//        }
//    }
//
//    return i;
//}


//u32 ASCII_WStrLen(void *str , u32 len)
//{
//    u32 i;
//
//    for(i=0 ; i< len ; i+=2)
//    {
//        if(((u8 *)str)[i] ==0 && ((u8 *)str)[i+1] ==0)
//        {
//            break;
//        }
//    }
//
//    return i;
//}
