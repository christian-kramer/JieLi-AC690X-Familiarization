/*
 *********************************************************************************************************
 *                                                AC51
 *                                      fs browser select
 *                                             CODE
 *
 *                          (c) Copyright 2015-2016, ZHUHAI JIELI
 *                                           All Rights Reserved
 *
 * File : *
 * By   : jamin.li
 * DATE : 2015-10-15 build this file
 *********************************************************************************************************
 */

#include "file_operate/file_bs_deal.h"
#include "uart.h"

#define FILE_BS_OPT_DBG
#ifdef  FILE_BS_OPT_DBG

    #define file_bs_puts     puts
    #define file_bs_printf   printf

#else
    #define file_bs_puts(...)
    #define file_bs_printf(...)

#endif

/*
 *********************************************************************************************************
 *
 * Description: 在指定数据中,搜索key(byte)的位置
 *
 * Arguments  : buf - 指定数据起始地址, buf_len-长度, key-搜索的key,
 *              find_type -搜索方向:0-从前往后搜索,1-从后往前搜索
 *
 * Returns    : key的位置
 *
 * Note:
 *********************************************************************************************************
 */
//查找byte的位置
int static find_byte_pos(u8 *buf, u16 buf_len, u8 key, u8 find_type)
{
    u8 *data = buf;

    if (0 == find_type)
    {
        //从前往后找
        while (buf_len > 0)
        {
            if (*data == key)
            {
                return (int) (data - buf);
            }
            data++;
            buf_len--;
        }
    }
    else
    {
        //从后往前找
        data += (buf_len - 1);
        while (buf_len > 0)
        {
            if (*data == key)
            {
                return (int) (data - buf);
            }
            data--;
            buf_len--;
        }
    }
    return -1;
}


/*
 *********************************************************************************************************
 *
 * Description: 转化路径格式
 *
 * Arguments  :
 *
 * Returns    : 0-为成功，其他值 失败
 *
 * Note:
 *********************************************************************************************************
 */
int file_comm_change_file_path(char *dest,char *src)
{
    u8 start_flag = 0,space_flag,val;

    while(1)
    {
        val = *src;
        if(val == '\0')
        {
            *dest = '\0';
            break;
        }
        switch(val)
        {
            case '/':
                if(start_flag != 0)
                {
                    space_flag = 0;
                }
                start_flag++;
                *dest = val;
            break;

            case ' ':
                space_flag = 1;
            break;

            default:
                if(space_flag != 0)
                {
                    *dest = '.';
                    dest++;
                    space_flag = 0;
                }
                *dest = val;
            break;
        }

        dest++;
        src++;
    }

    return 0;
}

/*
 *********************************************************************************************************
 *
 * Description: 修正从fs取等的长名
 *
 * Arguments  : str,len
 *
 * Returns    : 字节长度(不包含结束符)
 *
 * Note:    过滤0xffff值，自动填入结束符
 *********************************************************************************************************
 */
int file_comm_long_name_fix(u8 *str,u16 len)
{
    u8 *src = str;
    while(len > 1)
    {
        if ((*str == 0xff) && (*(str + 1) == 0xff))
        {
            break;
        }

        if ((*str == 0x0) && (*(str + 1) == 0x0))
        {
            break;
        }

        str += 2;
        len-= 2;
    }
    *str = 0x00;
    *(str + 1) = 0x00;
    return (int) (str - src);
}

/*
 *********************************************************************************************************
 *
 * Description: 转换8+3名字显示
 *
 * Arguments  : dest,src
 *
 * Returns    : 转换后名字的长度(不包含结束符)
 *
 * Note:      转换后自动填入结束符
 *********************************************************************************************************
 */
int file_comm_display_83name(u8 *dest, u8 *src)
{
    u8 offset;

    for (offset = 8; offset > 0; offset--)
    {
        if (src[offset - 1] != 0x20)
        {
            break;
        }
    }

    memcpy(dest, src, offset);

    if(src[8] != 0x20)
    {
        dest[offset++] = '.';
        memcpy(dest + offset, src + 8, 3);
        offset += 3;
    }

    dest[offset++] =  '\0';

    return offset; //name 's len
}

/*
 *********************************************************************************************************
 *
 * Description: 获取显示的文件夹或文件名字
 *
 * Arguments  :tpath,disp_file_name,disp_dir_name
 *
 * Returns    :
 *
 * Note:      转换后 lfn_cnt 为0为短名，lfn_cnt不为0 则是长名
 *********************************************************************************************************
 */
void file_comm_change_display_name(char *tpath,LONG_FILE_NAME *disp_file_name,LONG_FILE_NAME *disp_dir_name)
{
    u16 len;
    int pos;

     //取文件名字显示

    if(disp_file_name != NULL)
    {
        if(disp_file_name->lfn_cnt != 0)
        {
            //long name
            file_bs_puts("file long name  \n");
            //printf_buf(disp_file_name->lfn, 16);
            disp_file_name->lfn_cnt= file_comm_long_name_fix((void*)disp_file_name->lfn,disp_file_name->lfn_cnt );//增加结束符
        }
        else
        {
            //short name
            file_bs_puts("file short name\n");
            len = strlen((void *)tpath); //增加结束符
            pos = find_byte_pos((void *)tpath,len,0x2f,1);//
            if(pos == -1)
            {
                strcpy(disp_file_name->lfn,"----");
            }
            else
            {
                memcpy((void*)&disp_file_name->lfn[32],tpath + pos + 1,11);
                file_comm_display_83name((void*)disp_file_name->lfn,(void*)&disp_file_name->lfn[32]);
            }

            disp_file_name->lfn_cnt = 0;
        }

        //printf_buf(disp_file_name->lfn, 32);

    }


    //取文件夹名字显示
    if(disp_dir_name != NULL)
    {
        if(disp_dir_name->lfn_cnt != 0)
        {
            //long name
            file_bs_puts("folder long name \n");
            //printf_buf(disp_dir_name->lfn, 16);
            disp_dir_name->lfn_cnt= file_comm_long_name_fix((void*)&disp_dir_name->lfn,disp_dir_name->lfn_cnt);//增加结束符
        }
        else
        {
            //short name
            file_bs_puts("folder short name \n");
            len = strlen((void *)tpath); //增加结束符
            pos = find_byte_pos((void *)tpath,len,0x2f,1);//

            if(pos != -1)
            {
                pos = find_byte_pos((void *)tpath,pos,0x2f,1);//
            }

            if(pos == -1)
            {
                strcpy(disp_dir_name->lfn,"ROOT");
            }
            else
            {
                memcpy(&disp_dir_name->lfn[32],tpath + pos + 1,11);
                file_comm_display_83name((void*)disp_dir_name->lfn,(void*)&disp_dir_name->lfn[32]);
            }
            disp_dir_name->lfn_cnt = 0;
        }

        //printf_buf(disp_dir_name->lfn, 32);
    }
}
/*
 *********************************************************************************************************
 *
 * Description: 打开文件浏览器
 *
 * Arguments  : m_op_api
 *
 * Returns    : dir的总数
 *
 * Note:
 *********************************************************************************************************
 */
//返回根目录dir总数
extern void ui_ext_setting(const char *str);
extern u32 lg_dev_open_dir(void *parm,void * dir_info, void *file_type);
u32 file_bs_open_handle(MUSIC_OP_API *m_op_api,u8 *ext_name)
{
    u32 total_dir;

    if(m_op_api == NULL)
   {
        puts("*open hdl is null\n");
        return 0;
   }

    file_bs_puts("open bs hdl\n");
    file_bs_printf("bs_mapi:0x%x\n",(int)m_op_api);

    if(ext_name != NULL)
    {
        ui_ext_setting((void*)ext_name);
    }
    else
    {
        ui_ext_setting("WAVWMAMP3FLA");
    }


    //ui_ext_setting("MP3");

    total_dir = lg_dev_open_dir(m_op_api->fop_api->cur_lgdev_info,NULL,0);
    if(m_op_api->fop_api->cur_lgdev_info->lg_hdl->ui_fhdl == 0)
    {
        file_bs_puts("open bs fail\n");
        return 0;
    }
    file_bs_printf("open bs hdl_out,root_dir = 0x%x\n",total_dir);
    return total_dir;
}

/*
 *********************************************************************************************************
 *
 * Description: 关闭文件浏览器
 *
 * Arguments  : m_op_api
 *
 * Returns    :
 *
 * Note:
 *********************************************************************************************************
 */
void file_bs_close_handle(MUSIC_OP_API *m_op_api)
{
    file_bs_puts("close bs hdl\n");
    fs_file_close(m_op_api->fop_api->cur_lgdev_info->lg_hdl->fs_hdl,&m_op_api->fop_api->cur_lgdev_info->lg_hdl->ui_fhdl);
    file_bs_puts("close bs hdl_out\n");
}

/*
 *********************************************************************************************************
 *
 * Description: 进入文件夹目录
 *
 * Arguments  : m_op_api,dir_info
 *
 * Returns    : dir的总数
 *
 * Note:
 *********************************************************************************************************
 */
u32 file_bs_entern_dir(MUSIC_OP_API *m_op_api,FS_DIR_INFO *dir_info)
{
    u32 total_dir;
    file_bs_printf("bs_enter dir\n");
    total_dir = lg_dev_open_dir(m_op_api->fop_api->cur_lgdev_info,dir_info,0);//
    file_bs_printf("bs_enter_out,t = 0x%x\n",total_dir);
    return total_dir;
}

/*
 *********************************************************************************************************
 *
 * Description: 返回上传目录
 *
 * Arguments  : m_op_api
 *
 * Returns    : dir的总数
 *
 * Note:
 *********************************************************************************************************
 */
 extern u32 lg_dev_exit_dir(void *parm);
u32 file_bs_exit_dir(MUSIC_OP_API *m_op_api)
{
    u32 total_dir;
    file_bs_printf("bs_exit dir\n");
    total_dir =  lg_dev_exit_dir(m_op_api->fop_api->cur_lgdev_info);//
    file_bs_printf("bs_exit_out,total = 0x%x\n",total_dir);
    return total_dir;
}

/*
 *********************************************************************************************************
 *
 * Description: 获取指定位置的dir信息
 *
 * Arguments  : m_op_api,buf--被填入的buf,start_sn -获取的其实位置（从1开始）,get_cnt--获取dir的个数
 *
 * Returns    : 真正获取到的dir总数
 *
 * Note:
 *********************************************************************************************************
 */
 extern u32 lg_dev_get_dir(void *parm,u32 start_num, u32 total_num, void *buf_info);
u32 file_bs_get_dir_info(MUSIC_OP_API *m_op_api,FS_DIR_INFO *buf,u16 start_sn,u16 get_cnt)
{
    u32 real_dir;
    file_bs_printf("bs_get dir_info\n");
    real_dir =  lg_dev_get_dir(m_op_api->fop_api->cur_lgdev_info,start_sn,get_cnt,buf);
    file_bs_printf("bs_get_out,t = 0x%x\n",real_dir);
    return real_dir;
}




