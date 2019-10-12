#include "sdk_cfg.h"
#include "common/app_cfg.h"
#include "music_info.h"
#include "common/msg.h"
#include "dac/dac_api.h"
#include "dev_manage/drv_manage.h"
#include "ui/ui_api.h"
#include "fat/tff.h"
#include "fat/fs_io.h"
#include "music_ui.h"
#include "file_operate/file_bs_deal.h"
#include "uart.h"
#include "dec/decoder_phy.h"
extern u32 get_decode_time(void);
extern dec_inf_t * get_dec_inf_api(void *priv);

const decoder_ops_t *(*find_dec_ops[])(void) AT(.const_tab);
const decoder_ops_t *(*find_dec_ops[])(void) =
{
#if DEC_TYPE_SBC_ENABLE
	(void*)get_sbcd_ops,
#endif

#if DEC_TYPE_WAV_ENABLE
	(void*)get_wav_ops,
#endif

#if DEC_TYPE_WMA_ENABLE
	(void*)get_wma_ops,
#endif

#if DEC_TYPE_AAC_ENABLE
	(void*)get_aac_ops,
#endif

#if DEC_TYPE_FLAC_ENABLE
    (void*)get_flac_ops,
#endif

#if DEC_TYPE_APE_ENABLE
    (void*)get_ape_ops,
#endif

#if DEC_TYPE_MP3_ENABLE
	(void*)get_mp3_ops,
#endif

#if DEC_TYPE_F1A_ENABLE
	(void*)get_f1a_ops,
#endif
};

#define MP3_FFFR_ENABLE 	1
#define SBC_FFFR_ENABLE 	1
#define WAV_FFFR_ENABLE 	1
#define WMA_FFFR_ENABLE 	1
#define AAC_FFFR_ENABLE 	1
#define FLAC_FFFR_ENABLE 	1	//FLAC暂不支持快进快退
#define APE_FFFR_ENABLE 	1
#define F1A_FFFR_ENABLE 	0

const u8 dec_type_fffr_enable[] = {
#if DEC_TYPE_SBC_ENABLE
	SBC_FFFR_ENABLE,
#endif

#if DEC_TYPE_WAV_ENABLE
	WAV_FFFR_ENABLE,
#endif

#if DEC_TYPE_WMA_ENABLE
	WMA_FFFR_ENABLE,
#endif

#if DEC_TYPE_AAC_ENABLE
	AAC_FFFR_ENABLE,
#endif

#if DEC_TYPE_FLAC_ENABLE
    FLAC_FFFR_ENABLE,
#endif

#if DEC_TYPE_APE_ENABLE
    APE_FFFR_ENABLE,
#endif

#if DEC_TYPE_MP3_ENABLE
	MP3_FFFR_ENABLE,
#endif

#if DEC_TYPE_F1A_ENABLE
	F1A_FFFR_ENABLE,
#endif
};

const char dec_file_ext[][3] =
{
#if DEC_TYPE_MP3_ENABLE
    {"MP1"},
    {"MP2"},
    {"MP3"},
#endif // DEC_TYPE_MP3_ENABLE

#if DEC_TYPE_WMA_ENABLE
    {"WMA"},
#endif // DEC_TYPE_WMA_ENABLE

#if DEC_TYPE_WAV_ENABLE
    {"WAV"},
#endif // DEC_TYPE_WAV_ENABLE

#if DEC_TYPE_FLAC_ENABLE
    {"FLA"},
#endif // DEC_TYPE_FLAC_ENABLE

#if DEC_TYPE_APE_ENABLE
    {"APE"},
#endif // DEC_TYPE_APE_ENABLE

#if DEC_TYPE_F1A_ENABLE
    {"F1A"},
#endif // DEC_TYPE_APE_ENABLE

#if MUSIC_DECRYPT_EN
    {"SMP"},
#endif //SMP_FILE
    {'\0'},
};

//u32 music_playfile_start_time;
u32 music_playfile_total_time;
u32 music_cur_filenum,music_cur_filetotal;
u32 play_file_clust;


u32 get_find_dec_ops_type_cnt(void)//call by dec_lib
{
    return (sizeof(find_dec_ops)/sizeof(find_dec_ops[0]));
}

void set_flac_sys_freq(u8 flag)
{
#if 0
    if(flag)
    {
        puts("\nSYS = 120M\n");
        set_sys_freq(FLAC_SYS_Hz);
    }
    else
    {
        puts("\nSYS = 96M\n");
        set_sys_freq(SYS_Hz);
    }
#endif
}

void * music_get_file_id3(u32 len)
{
    u8* id3_buf = NULL;

    if(len <= 3*1024)
        id3_buf = malloc(len);

//    printf("------len = %x byte\n",len);
//    printf("id3_buf malloc = %x byte----\n",id3_buf);

    return id3_buf;
}

#define CLUS_READ_BIG_U32(a)   ((*((u8*)(a)) <<24) + (*((u8*)(a)+1)<<16) + (*((u8*)(a)+2)<<8) + *((u8*)(a)+3))
void music_file_info(MUSIC_OP_API *m_op_api)
{
//    FS_DISP_NAME music_name_buf;
    //u32 t_fnum,t_fnum;
//    file_operate_ctl(FOP_GET_FOLDER_INFO,m_op_api->fop_api,&music_cur_filetotal,&music_cur_filenum);

    music_ui.ui_curr_statu = (u32*)&m_op_api->dop_api->dec_api.phy_ops->status;
    
	music_cur_filenum = (u32)m_op_api->dop_api->file_num;
    music_cur_filetotal =(u32)m_op_api->fop_api->cur_lgdev_info->total_file_num;
	
    /* printf("file_num = %d,total_num = %d \n",music_cur_filenum,music_cur_filetotal); */

	// (u32)file_operate_ctl(FOP_GET_TOTALFILE_NUM,m_op_api->fop_api,m_op_api->dop_api->file_type,0);
//
//    printf("file_num = %d,total_fnum = %d \n",music_ui.ui_curr_file,music_ui.ui_total_file);

//get name

    if(music_name_buf.tpath == NULL)
    {
        puts("disp buff null\n");
        return;
    }

    //init
    music_name_buf.file_name->lfn_cnt = 0;
    music_name_buf.dir_name->lfn_cnt = 0;
    music_name_buf.tpath[0] = 0;

    file_operate_ctl(FOP_GET_FILE_NAME,m_op_api->fop_api,0,&music_name_buf);

    /* printf("\nget path: %s\n",music_name_buf.tpath); */

//    put_buf((void*)music_name_buf.file_name,32);
//    put_buf((void*)music_name_buf.dir_name,32);
/*
    if(music_name_buf.file_name->lfn_cnt != 0)
    {
        puts("\nget file long name:\n");
        put_buf((u8*)music_name_buf.file_name->lfn , music_name_buf.file_name->lfn_cnt );
    }

    if(music_name_buf.dir_name->lfn_cnt != 0)
    {
        puts("\nget dir long name:\n");
        put_buf((u8*)music_name_buf.dir_name->lfn , music_name_buf.dir_name->lfn_cnt );
    }
*/
    file_comm_change_display_name(music_name_buf.tpath,music_name_buf.file_name, music_name_buf.dir_name);
    music_name_buf.update_flag = 1;
/*
    puts("\ndisp file name:");
    if(music_name_buf.file_name->lfn_cnt != 0)
    {
        put_buf((u8*)music_name_buf.file_name->lfn , music_name_buf.file_name->lfn_cnt );
    }
    else
    {
        puts(music_name_buf.file_name->lfn);
    }

    puts("\ndisp dir name:");
    if(music_name_buf.dir_name->lfn_cnt != 0)
    {
        put_buf((u8*)music_name_buf.dir_name->lfn , music_name_buf.dir_name->lfn_cnt );
    }
    else
    {
        puts(music_name_buf.dir_name->lfn);
    }
*/
    play_file_clust = get_sclust(m_op_api->fop_api->cur_lgdev_info->lg_hdl->file_hdl);
    /* printf("\n--music_file clust: 0x%x\n",play_file_clust); */

    play_file_clust = CLUS_READ_BIG_U32(&play_file_clust);

    {
        dec_inf_t * dec_info = get_dec_inf_api(m_op_api->dop_api->dec_api.phy_ops);
        /* printf("br: %d\n",dec_info->br); */
        /* printf("nch: %d\n",dec_info->nch); */
        /* printf("sr: %d\n",dec_info->sr); */
        /* printf("total_time: %d\n",dec_info->total_time); */

        music_playfile_total_time =  dec_info->total_time;
    }


    /*
    #if LRC_LYRICS_EN
        if(lrc_init(m_op_api))
        {
            printf("lrc_init err\n");
        }
    #endif
    */


}

u8 get_floder_name(char* ptr,u32 filecount)
{
    if(strcmp(ptr,"JL_REC     ") == 0)
    {
        printf("---ptr %s--\n",ptr);
        printf("---filecount %d\n",filecount);
        return 1;
    }

    return 0;
}

void get_floder_totalfile(u32 filecount)
{
    printf("---totalfile %d\n",filecount);
}
