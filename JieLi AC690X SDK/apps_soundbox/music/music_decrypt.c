#include "music_decrypt.h"
#include "music.h"
#include "fat/fs_io.h"
#include "fat/tff.h"
#include "sdk_cfg.h"

#if MUSIC_DECRYPT_EN

#if 1
#define decrypt_puts(...)
#define decrypt_put_buf(...)
#define decrypt_printf(...)
#else
#define decrypt_puts puts
#define decrypt_put_buf put_buf
#define decrypt_printf printf
#endif

CIPHER cipher_file;

/*----------------------------------------------------------------------------*/
/**@brief  解密读使能开关
  @param  u8 ctl
  @return 无
  @note   void cipher_ctl(u8 ctl)
 */
/*----------------------------------------------------------------------------*/
static void cipher_ctl(u8 ctl)
{
	cipher_file.cipher_enable = ctl;
}

/*----------------------------------------------------------------------------*/
/**@brief  解密读文件数据的回调函数，用于底层的物理读
  @param  void* buf, u32 lba
  @return 无
  @note   void cryptanalysis_buff(void* buf, u32 faddr, u32 len)
 */
/*----------------------------------------------------------------------------*/
#define ALIN_SIZE	4
static void cryptanalysis_buff(void* buf, u32 faddr, u32 len)
{
	u32 i;
	u8 j;
	u8	head_rem;//
	u8  tail_rem;//
	u32 len_ali;

//	u32 *buf_4b_ali;
	u8  *buf_1b_ali;
	u8  *cipher_code;

	cipher_code = (u8 *)&cipher_file.cipher_code;

	if(!cipher_file.cipher_enable)
	{
		return;
	}

	/* printf("----faddr = %d \n",faddr); */
	/* put_buf(buf,len); */

	decrypt_printf("buf_addr = %d \n",buf);

	head_rem = ALIN_SIZE-(faddr%ALIN_SIZE);
	if(head_rem == ALIN_SIZE)
	{
		head_rem = 0;
	}
	tail_rem = (faddr+len)%ALIN_SIZE;
	decrypt_printf("head_rem = %d tail_rem = %d \n",head_rem,tail_rem);

	decrypt_puts("deal_head_buf\n");
	buf_1b_ali = buf;
	j = 3;
	for(i = head_rem; i>0; i--)
	{
		buf_1b_ali[i-1] ^= cipher_code[j--];
		decrypt_printf("i = %d \n",i-1);
		decrypt_printf("buf_1b_ali[i] = %x \n",buf_1b_ali[i-1]);
	}
	decrypt_puts("\n\n-----------TEST_HEAD-----------------");
	decrypt_put_buf(buf_1b_ali,head_rem);

	decrypt_puts("deal_main_buf\n");
	buf_1b_ali = buf;
	buf_1b_ali = (u8 *)(buf_1b_ali+head_rem);
	len_ali = len - head_rem-tail_rem;
	decrypt_printf("len_ali = %d \n",len_ali);
	decrypt_printf("buf_1b_ali = %d \n",buf_1b_ali);
	decrypt_printf("buf_4b_ali = %d \n",buf_4b_ali);
	/* if(cipher_file.cipher_database <= lba) */
	{
		for(i = 0; i<(len_ali/4);i++)
		{
			/* buf_4b_ali[i] = cipher_file.cipher_code; */
			buf_1b_ali[0+i*4] ^= cipher_code[0];
			buf_1b_ali[1+i*4] ^= cipher_code[1];
			buf_1b_ali[2+i*4] ^= cipher_code[2];
			buf_1b_ali[3+i*4] ^= cipher_code[3];
		}
	}
	decrypt_puts("\n\n-----------TEST_MAIN-----------------");
	decrypt_put_buf(buf_1b_ali,len_ali);

	decrypt_puts("deal_tail_buf\n");
	buf_1b_ali = buf;
	buf_1b_ali+=len-tail_rem;
	j = 0;
	for(i = 0 ; i<tail_rem; i++)
	{
		buf_1b_ali[i] ^=cipher_code[j++];
	}
	decrypt_puts("\n\n-----------TEST_TAIL-----------------");
	decrypt_put_buf(buf_1b_ali,tail_rem);
	
	decrypt_puts("\n\n-----------TEST-----------------");
	/* put_buf(buf,len); */
}

/*----------------------------------------------------------------------------*/
/**@brief  模式提示音播放
  @param  void *file：当前解码文件的句柄
  @return 无
  @note   void get_decode_file_info(void *file)
 */
/*----------------------------------------------------------------------------*/
void get_decode_file_info(void *file)
{
	_FIL_HDL *f_h=(_FIL_HDL *)(file);

	if(f_h->io->type == FAT_FS_T)
	{
		FIL *p_f=(FIL *)(f_h->hdl);
		u8 *file_name;
		file_name =(u8 *)p_f->dir_info.fname;
		printf("---name = %s---\n",p_f->dir_info.fname);

		if((file_name[8]== 'S') &&
				(file_name[9]== 'M') &&
				(file_name[10]== 'P'))
		{
			puts("\n----It's a SMP FILE---\n");
			cipher_ctl(1);
			cipher_file.cipher_database = clust2sect(p_f->fil.fs, p_f->fil.start_clust);
			return;
		}
	}
	cipher_ctl(0);
}

/*----------------------------------------------------------------------------*/
/**@brief  解密读初始化
  @param  无
  @return 无
  @note   void cipher_init(u32 key)
 */
/*----------------------------------------------------------------------------*/
void cipher_init(u32 key)
{
	cipher_file.cipher_code = key;
	reg_dec_input_cb(cryptanalysis_buff);
	/* u8 tmp_buf[128]; */
	/* memset(tmp_buf,0x00,128); */
	/* cryptanalysis_buff(tmp_buf,1,65); */
}

/*----------------------------------------------------------------------------*/
/**@brief  解密读初关闭
  @param  无
  @return 无
  @note   void cipher_close(void)
 */
/*----------------------------------------------------------------------------*/
void cipher_close(void)
{
	cipher_ctl(0);
	reg_dec_input_cb(NULL);
}
#endif

