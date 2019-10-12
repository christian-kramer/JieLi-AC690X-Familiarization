#include "file_operate/file_op.h"
#include "file_operate/file_op_err.h"
#include "fat/fs_io.h"
#include "fat/tff.h"
#include "vm/vm_api.h"
#include "crc_api.h"
/***************************2016-12-13*********************************/
#ifdef MUSIC_USB_SD_VOICE
//////////////////////语音区分 start//////////////////////////////////
#include "file_operate\logic_dev_list.h"
#include "dev_manage\drv_manage.h"
//////////////////////语音区分 end//////////////////////////////////
#endif
/*********************************************************************/
//#define FILE_OP_DEBUG

#ifdef FILE_OP_DEBUG
#define file_op_deg 		printf
#define file_op_deg_buf 	printf_buf
#else
#define file_op_deg(...)
#define file_op_deg_buf(...)
#endif

extern volatile u8 new_lg_dev_let;

//u32 lg_dev_list_init();
//u32 lg_dev_add(u8 drv, void * dev_item,u32 snum, u8 dev_type);
//u32 lg_dev_find_by_phydevnum(u8 drvnum);
//lg_dev_list * lg_dev_find(u32 drv);
//u32 lg_dev_del(void *dev_item);
//u32 get_total_dev(u8 dev_type);
//s32 get_dev_first(u8 dev_type);
//s32 get_dev_last(u8 dev_type);
//s32 get_dev_next(u8 drv, u8 dev_type);
//s32 get_dev_prev(u8 drv, u8 dev_type);
extern u32 lg_dev_get_totalfile(void *parm,u8 *file_type,void *bpinfo);
extern u32 lg_dev_open_file(void *parm,u32 *filenum,void *bp_buf);
u32 lg_dev_open_file_bypath(void *parm,void *path,void *filenum);
//u32 lg_dev_read_file(void *parm,u8 *buf,u16 len);
//u32 lg_dev_seek_file(void *parm,u32 seek_mode,u32 offset);
//void lg_dev_close(void *parm);
extern u32 lg_dev_online_status(void *parm);
//u32 lg_dev_get_total_phydev();
//u32 lg_dev_status_chg(u32 status,void *parm,u32 lgdevnum);
//u32 lg_dev_mount(void *parm,u8 first_let,u32 total_lgdev);
extern u32 lg_dev_get_phydev_type(void *parm);
extern s32 get_spec_lgdev(u8 drv, u32 snum,u8 dev_type);
extern u32 lg_dev_find_update_file(void *parm);
extern u32 lg_dev_get_folder_info(void *parm,u32 *t_num,u32 *c_num);
u32 fs_getfnum_byfolder(u32 foldermode,u32 max_filenum,void *p_f_hdl,u32 *filenum);
//extern volatile u8 new_lg_dev_let;
//lg_dev_list * lg_dev_set_inuse(u32 drv);
extern void lg_dev_close_all();
extern u32 lg_dev_open_dir(void *parm,void * dir_info, void *file_type);
//u32 lg_dev_exit_dir(void *parm);
//u32 lg_dev_get_dir(void *parm,u32 start_num, u32 total_num, void *buf_info);
extern u32 lg_dev_open_file_bysclust(void *parm,u32 *filenum,u32 spc_sclust);
extern u32 lg_dev_get_file_indir(void *parm, void *ext_name);
/******************************2016-12-13***********************************/
#if 1//def MUSIC_USB_SD_VOICE
//////////////////////语音区分 start//////////////////////////////////
////////////////////////////////////////////////
extern u32 file_operate_get_total_lgdev();
extern s32 get_dev_first(u8 dev_type);
lg_dev_list * lg_dev_find(u32 drv);
u32 *try_play_dev(void)
{
    lg_dev_list *p_list=NULL;
    u32 tmp_let;
    s32 tmp;
    tmp_let=new_lg_dev_let;
    if(tmp_let==0)
        return 0;
    p_list=lg_dev_find(tmp_let);
    if(p_list==NULL)
    {
        if(file_operate_get_total_lgdev())
        {
            tmp=get_dev_first(DEV_TYPE_MASS);
            if(tmp<0)
                return NULL;
            else
            {
                new_lg_dev_let=(u8)tmp;
                p_list=lg_dev_find(new_lg_dev_let);

                return p_list->cur_dev.lg_hdl->phydev_item;
            }
        }
        else
            return NULL;
    }
    return p_list->cur_dev.lg_hdl->phydev_item;
}
//////////////////////语音区分 end//////////////////////////////////
#endif
/************************************************************************************/
static u32 random_number(u32 start, u32 end)
{
	if(end<=start)
	{
		return start;
	}

	//获得一个范围在start到end之间的随机数
	return JL_TIMER0->CNT % (end-start+1) + start;
}

static u32 file_operate_op(u32 cmd,FILE_OPERATE *fop,void *input,void *output)
{
    FILE_OPERATE *cur_fop;
    u32 status=FILE_OP_NO_ERR;
    u32 *tmp;
    cur_fop=fop;
    switch(cmd)
    {
        case FOP_INIT:
             break;
        case FOP_FIND_SPEC_LGDEV:
                tmp=input;
                status=get_spec_lgdev(cur_fop->cur_lgdev_info->dev_let,*tmp,cur_fop->cur_lgdev_info->dev_type);
                if(status<=0)
                    status=FILE_OP_ERR_LGDEV_NO_FIND;
                break;
        case FOP_GET_TOTALFILE_NUM:
                status = lg_dev_get_totalfile(cur_fop->cur_lgdev_info, (u8*)input, output);
                cur_fop->cur_lgdev_info->total_file_num = status;
                break;
        case FOP_OPEN_FILE_BYPATH:
                status=lg_dev_open_file_bypath(cur_fop->cur_lgdev_info,cur_fop->fop_init->filepath,input);
                if(status)
                {
                   cur_fop->cur_lgdev_info->last_op_file_num=*((u32*)input);
                   status = FILE_OP_NO_ERR;
                }
                else
                {
                    status = FILE_OP_ERR_NUM;
                }
                break;

        case FOP_OPEN_FILE_BYSCLUCT:    //指定簇号播放，用于
                status=lg_dev_open_file_bysclust(cur_fop->cur_lgdev_info, input, cur_fop->fop_init->filesclust);
                if(!status)
                {
                   cur_fop->cur_lgdev_info->last_op_file_num=*((u32*)input);
                   status = FILE_OP_NO_ERR;
                }
                else
                {
                    status = FILE_OP_ERR_NUM;
                }
                break;

        case FOP_OPEN_FILE:
//                tmp=input;
//                file_op_deg("open file num = %08x   \n",*tmp);
                status = lg_dev_open_file(cur_fop->cur_lgdev_info, input, output);
                if(!status)
                {   ///<打开成功，记录下操作的文件号
                    cur_fop->cur_lgdev_info->last_op_file_num = *((u32*)input);
                }
                break;
#if 1
        case FOP_READ_BPINFO:
                status=lg_dev_get_phydev_type(cur_fop->cur_lgdev_info);
                if(status<100)
                {
                    status+=VM_DEV0_BREAKPOINT;
                    file_op_deg("bp read =%08x \n",status);
                    status=vm_read_api(status,output);
                    if(status == VM_FILE_BK_LEN)
                    {
						u8 i;
						for(i=0;i<VM_FILE_BK_LEN;i++)
						   	file_op_deg("%02x ",*((u8 *)output+i));
                        file_op_deg("bp read ok \n");
                        FS_BRK_INFO *tmpbp;
                        u16 crcvalue;
                        tmpbp=(FS_BRK_INFO *)output;
                        /* crcvalue=CRC16((u8*)tmpbp,VM_FILE_BK_LEN-2); */
                        /* if(crcvalue==tmpbp->crc) */
                        {file_op_deg("bp crc ok \n");
//                            printf_buf(tmpbp,VM_FILE_BK_LEN);
                            status= FILE_OP_NO_ERR;
                        }
                        /* else
                        {file_op_deg("bp crc err \n");
                            memset((u8*)output,0,VM_FILE_BK_LEN);
                            status=FILE_OP_ERR_OPEN_FILE;
                        } */
                        break;
                    }
                    else
                    {file_op_deg("bp read err0 \n");

                        if(status > VM_FILE_BK_LEN)
                        {file_op_deg("bp read err1 \n");
                            memset((u8*)output,0,VM_FILE_BK_LEN);
                        }
                        status=FILE_OP_ERR_OPEN_FILE;
                    }
                }
                else
                {
                    status=FILE_OP_ERR_LGDEV_NO_FIND;
                }

                break;
        case FOP_READ_FLACBPINFO:
                file_op_deg("FOP_READ_FLACBPINFO \n");
                status=lg_dev_get_phydev_type(cur_fop->cur_lgdev_info);
                if(status<100)
                {
                    status+=VM_DEV0_FLACBREAKPOINT;
                    file_op_deg("bp read =%08x \n",status);
                    status=vm_read_api(status,((FS_BRK_INFO *)output)->flac_brk_buff);
                    if(status == VM_FILE_FLACBK_LEN)
                    {
						u16 i;
						for(i=0;i<VM_FILE_FLACBK_LEN;i++)
							file_op_deg("%02x ",*((u8 *)((((FS_BRK_INFO *)output)->flac_brk_buff))+i));
                        file_op_deg("bp read ok \n");
                        /* crcvalue=CRC16((u8*)tmpbp,VM_FILE_BK_LEN-2); */
                        /* if(crcvalue==tmpbp->crc) */
                        {file_op_deg("bp crc ok \n");
//                            printf_buf(tmpbp,VM_FILE_BK_LEN);
                            status= FILE_OP_NO_ERR;
                        }
                        /* else
                        {file_op_deg("bp crc err \n");
                            memset((u8*)output,0,VM_FILE_BK_LEN);
                            status=FILE_OP_ERR_OPEN_FILE;
                        } */
                        break;
                    }
                    else
                    {file_op_deg("bp read err0 \n");

                        if(status > VM_FILE_BK_LEN)
                        {file_op_deg("bp read err1 \n");
                            memset((u8*)(((FS_BRK_INFO *)output)->flac_brk_buff),0,VM_FILE_BK_LEN);
                        }
                        status=FILE_OP_ERR_OPEN_FILE;
                    }
                }
                else
                {
                    status=FILE_OP_ERR_LGDEV_NO_FIND;
                }

                break;
        case FOP_SAVE_BPINFO:
                status=lg_dev_get_phydev_type(cur_fop->cur_lgdev_info);
                if(status<100)
                {   file_op_deg("bp save bg \n");
                    if(fs_get_brk_info(input,cur_fop->cur_lgdev_info->lg_hdl->file_hdl))
                    {   file_op_deg("bp save ge finfo ok \n");
                        FS_BRK_INFO *tmpbp;
                        tmpbp=(FS_BRK_INFO *)input;
                        status+=VM_DEV0_BREAKPOINT;
						file_op_deg("bp save =%08x \n",status);
						/* u16 crcvalue=CRC16((u8*)tmpbp,VM_FILE_BK_LEN-2); */
						/* tmpbp->crc=crcvalue; */
						u8 i;
						for(i=0;i<VM_FILE_BK_LEN;i++)
							file_op_deg("%02x ",*((u8 *)input+i));
						if(VM_FILE_BK_LEN == vm_write_api(status,input))
						{   file_op_deg("bp save ok \n");
													   /* printf_buf(input,VM_FILE_BK_LEN); */
							status= FILE_OP_NO_ERR;
							break;
						}
						else
						{   file_op_deg("bp save err \n");
							status=FILE_OP_ERR_OPEN_FILE;
						}

					}
					else
					{
						file_op_deg("bp save ge finfo err \n");
						status=FILE_OP_ERR_LGDEV_NO_FIND;
					}
				}
				else
				{   file_op_deg("bp save err0 \n");
					status=FILE_OP_ERR_LGDEV_NO_FIND;
				}
				break;
		case FOP_SAVE_FLACBPINFO:
				file_op_deg("FOP_SAVE_FLACBPINFO\n");
				status=lg_dev_get_phydev_type(cur_fop->cur_lgdev_info);
				if(status<100)
				{   file_op_deg("bp save bg \n");
					if(fs_get_brk_info(input,cur_fop->cur_lgdev_info->lg_hdl->file_hdl))
					{   file_op_deg("bp save ge finfo ok \n");
						FS_BRK_INFO *tmpbp;
						tmpbp=(FS_BRK_INFO *)input;
						/* status+=VM_DEV0_FLACBREAKPOINT; */
						file_op_deg("bp save =%08x \n",status);
						/* u16 crcvalue=CRC16((u8*)tmpbp,VM_FILE_BK_LEN-2); */
						/* tmpbp->crc=crcvalue; */
						u16 i;
						for(i=0;i<VM_FILE_FLACBK_LEN;i++)
							file_op_deg("%02x ",*((u8 *)((((FS_BRK_INFO *)input)->flac_brk_buff))+i));
						/* memset(((FS_BRK_INFO *)input)->norm_brk_buff,1,8); */
						if((VM_FILE_BK_LEN ==vm_write_api(status+VM_DEV0_BREAKPOINT,input) ) && (VM_FILE_FLACBK_LEN ==vm_write_api(status+VM_DEV0_FLACBREAKPOINT,((FS_BRK_INFO *)input)->flac_brk_buff) ))//
						{   file_op_deg("bp save ok \n");
							//                            printf_buf(input,VM_FILE_BK_LEN);
							status= FILE_OP_NO_ERR;
                            break;
                        }
                        else
                        {   file_op_deg("bp save err \n");
                            status=FILE_OP_ERR_OPEN_FILE;
                        }

                    }
                    else
                    {
                        file_op_deg("bp save ge finfo err \n");
                        status=FILE_OP_ERR_LGDEV_NO_FIND;
                    }
                }
                else
                {   file_op_deg("bp save err0 \n");
                    status=FILE_OP_ERR_LGDEV_NO_FIND;
                }
                break;
 #endif
        case FOP_CLEAR_BPINFO:
                status=lg_dev_get_phydev_type(cur_fop->cur_lgdev_info);
				if(status<100)
				{  
					file_op_deg("bp save bg \n");
					u8 i;
					/* for(i=0;i<VM_FILE_BK_LEN;i++) */
					/* file_op_deg("%02x ",*((u8 *)input+i)); */
					memset((u8 *)input,0,sizeof(FS_BRK_INFO));
					if((VM_FILE_BK_LEN ==vm_write_api(status+VM_DEV0_BREAKPOINT,input) ) && (VM_FILE_FLACBK_LEN ==vm_write_api(status+VM_DEV0_FLACBREAKPOINT,((FS_BRK_INFO *)input)->flac_brk_buff) ))//
					{
						file_op_deg("bp save ok \n");
						/* printf_buf(input,VM_FILE_BK_LEN); */
						for(i=0;i<VM_FILE_BK_LEN;i++)
							file_op_deg("%02x ",*((u8 *)((((FS_BRK_INFO *)input)))+i));
						status= FILE_OP_NO_ERR;
						break;
					}
					else
					{   file_op_deg("bp save err \n");
						status=FILE_OP_ERR_OPEN_FILE;
					}
				}
				else
				{   file_op_deg("bp save err0 \n");
					status=FILE_OP_ERR_LGDEV_NO_FIND;
				}
				break;

/*        case FOP_SAVE_LASTOP_DEV:
                status=lg_dev_get_phydev_type(cur_fop->cur_lgdev_info);
                if(status<100)
                {
                    u8 devnum=status;
                    if(!vm_write_api(VM_MUSIC_DEVICE,&devnum))
                    {   file_op_deg("save last dev =%08x \n",devnum);
                        status= FILE_OP_NO_ERR;
                        break;
                    }
                }
                status=FILE_OP_ERR_OPEN_FILE;
                break;*/
        case FOP_DEV_STATUS:
                status=lg_dev_online_status(cur_fop->cur_lgdev_info);
                break;

        case FOP_CLOSE_LOGDEV:
                file_op_deg("close all lg dev \n");
                lg_dev_close_all();
                break;

        case FOP_ALLFILE_ERR_LGDEV:
                cur_fop->cur_lgdev_info->allfileerr=1;
                break;

        case FOP_GET_PHYDEV_INFO:
                status=lg_dev_get_phydev_type(cur_fop->cur_lgdev_info);
                break;

        case FOP_FIND_UPDATE_FILE:
                status=lg_dev_find_update_file(cur_fop->cur_lgdev_info);
                break;
        case FOP_GET_FOLDER_INFO:
                status=lg_dev_get_folder_info(cur_fop->cur_lgdev_info,input,output);
                break;

#if GET_LFN
        case FOP_GET_FILE_NAME:
                status = fs_file_name(cur_fop->cur_lgdev_info->lg_hdl->fs_hdl,cur_fop->cur_lgdev_info->lg_hdl->file_hdl, output);
                break;
#endif

        default:
                status = FILE_OP_ERR_CMD;     ///<不能识别的命令
                break;
    }
    return status;
}

static u32 file_sel_op(u32 cmd, FILE_OPERATE *fop, void *input, void *output)
{
    FILE_OPERATE *cur_fop;
    u32 status;
    s32 step = 0;
    u32 dev_type;
    u32 curr_file = *((u32*)input);
    u32 first_file, last_file;
    u32 file_total;

    cur_fop = fop;
    if( (cur_fop->fop_init->cur_dev_mode == DEV_SEL_CUR) && (cur_fop->cur_lgdev_info == NULL) )
    {
		file_op_deg("file_sel_op  err  \n");
        return FILE_OP_ERR_LGDEV_NO_FIND;
    }

    ///--------dev sel-----------------------------
    dev_type = cur_fop->cur_lgdev_info->dev_type;
    file_op_deg("old file num=%08x  \n", curr_file);
    if(dev_type == DEV_TYPE_SPI)
    {    ///< SPI设备，不切换
        cur_fop->fop_init->cur_dev_mode = DEV_SEL_CUR;
    }
    file_total = cur_fop->cur_lgdev_info->total_file_num;
    first_file = 1;
    last_file = file_total;

    ///----------选择播放模式-------------------------------
    file_op_deg("file_sel_op cur_play_mode = 0x%x  \n", cur_fop->fop_init->cur_play_mode);
    switch(cur_fop->fop_init->cur_play_mode)
	{
		case REPEAT_FOLDER:
			//获取当前文件夹first、last

            if(PLAY_BREAK_POINT == cur_fop->fop_init->cur_sel_mode)
            {
                break;
            }

            if(!fs_folder_file(cur_fop->cur_lgdev_info->lg_hdl->file_hdl, &first_file, &last_file))
            {
                first_file = 1;
                last_file=file_total;
            }

			//first_file = ((*(FIL *)((_FIL_HDL *)(cur_fop->cur_lgdev_info->lg_hdl->file_hdl))->hdl).fs_msg.file_total_outdir) + 1;
            //last_file = first_file -1 +  ((*(FIL *)((_FIL_HDL *)(cur_fop->cur_lgdev_info->lg_hdl->file_hdl))->hdl).fs_msg.file_total_indir);

			break;

		case REPEAT_ONE:
			if(PLAY_AUTO_NEXT == cur_fop->fop_init->cur_sel_mode)
			{
				first_file = curr_file;
				last_file = curr_file;
			}
			break;

		case REPEAT_RANDOM:
			//随机数获取
			curr_file = random_number(first_file, last_file);
			break;
			
	    case REPEAT_ONE_LGDEV:
		case REPEAT_ALL:
		default :
			break;
	}
	
	file_op_deg("cur_sel_mode = 0x%x  \n", cur_fop->fop_init->cur_sel_mode);
    ///----------选择文件-------------------------------
    switch(cur_fop->fop_init->cur_sel_mode)
    {
        case PLAY_NEXT_FILE:
        case PLAY_AUTO_NEXT:
                step=1;
                break;
        case PLAY_PREV_FILE:
                step=-1;
                break;
        case PLAY_FIRST_FILE:
                if (cur_fop->fop_init->cur_play_mode==REPEAT_FOLDER)
                {
                    curr_file = first_file;
                    goto _open_file;
                }
                curr_file = 1;
                goto _open_file;
        case PLAY_LAST_FILE:
                if (cur_fop->fop_init->cur_play_mode==REPEAT_FOLDER)
                {
                    curr_file = last_file;
                    goto _open_file;
                }
                curr_file = file_total;
                goto _open_file;
        case PLAY_SPEC_FILE:
                if (curr_file == 0 || curr_file > file_total)
                {
                    return FILE_OP_ERR_NUM;
                }
				goto _open_sel_file;
		case PLAY_AB_REPEAT:
        case PLAY_BREAK_POINT:
            	goto _open_file;
        case PLAY_NEXT_FOLDER:
                if(fs_getfnum_byfolder(1,file_total,cur_fop->cur_lgdev_info->lg_hdl->file_hdl,&curr_file))
                {
                    goto _open_file;
                }
                else
                    return FILE_OP_ERR_NUM;

        case PLAY_PRE_FOLDER:
                if(fs_getfnum_byfolder(0,file_total,cur_fop->cur_lgdev_info->lg_hdl->file_hdl,&curr_file))
                {
                    goto _open_file;
                }
                else
                    return FILE_OP_ERR_NUM;
                break;
        default :
			 break;
    }
    curr_file += step;
    if ((REPEAT_RANDOM==cur_fop->fop_init->cur_play_mode)
            && curr_file==cur_fop->cur_lgdev_info->last_op_file_num)
	{///随机数不要等于本身
		curr_file++;
	}
_open_sel_file:
	///文件范围检测
	if(curr_file > last_file)
	{
	    if(cur_fop->fop_init->cur_play_mode != REPEAT_ALL)
	    {
            curr_file =  first_file;
	    }
	    else
	    {
            return FILE_OP_ERR_END_FILE;
	    }
	}
	else if(curr_file < first_file)
	{
	    if(cur_fop->fop_init->cur_play_mode != REPEAT_ALL)
	    {
            curr_file =  last_file;
	    }
	    else
	    {
            return FILE_OP_ERR_PRE_FILE;
	    }
	}

_open_file:
	file_op_deg("curr_file num = 0x%x\n", curr_file);
	status = file_operate_op(FOP_OPEN_FILE, cur_fop, (void*)&curr_file, output);
	if(!status)
	{
	    if(cur_fop->fop_init->cur_sel_mode==PLAY_PRE_FOLDER)
	    {
	        u32 f_file,e_file;
	        if(fs_folder_file(cur_fop->cur_lgdev_info->lg_hdl->file_hdl,&f_file,&e_file))
	        {
//	            printf("f_file2 =%08x   e_file=%08x \n ",f_file,e_file);
//	            printf("curr_file2 =%08x\n ",curr_file);
	            curr_file=f_file;
	            cur_fop->fop_init->cur_sel_mode=PLAY_SPEC_FILE;
                goto _open_file;
	        }
	    }
	    file_op_deg("open file ok  \n");
	}
	else
	{
	    file_op_deg("open file err  \n");
	}

    *((u32*)input) = curr_file;

    return status;
}

u32 file_operate_ctl(u32 cmd, void *parm, void *input, void *output)
{
    u32 status = FILE_OP_NO_ERR;
    FILE_OPERATE *cur_fop;

    cur_fop = (FILE_OPERATE *)parm;
    if((cur_fop->cur_lgdev_info == NULL)&&(cmd != FOP_CLOSE_LOGDEV))
    {
        return FILE_OP_ERR_OP_HDL;
    }

    if(cmd < FOP_MAX)
    {
        status = file_operate_op(cmd, cur_fop, input, output);
    }
    else if(cmd < MAX_PLAY_MODE)
    {
        cur_fop->fop_init->cur_play_mode = cmd;
    }
    else if(cmd < MAX_FILE_SEL_MODE)
    {
        cur_fop->fop_init->cur_sel_mode = cmd;
        if(PLAY_FILE_BYPATH == cmd)
        {
            status = file_operate_op(FOP_OPEN_FILE_BYPATH, cur_fop, input, output);
        }
        else
            status = file_sel_op(0, cur_fop, input, output);
    }
    else if(cmd<MAX_DEV_MODE)
    {
        cur_fop->fop_init->cur_dev_mode = cmd;
        status = file_sel_op(0, cur_fop, input, output);
    }
    else
        status = FILE_OP_ERR_CMD;     ///<不能识别的命令

    return status;
}


