#include "music.h"
#include "music_ab_rpt.h"
#include "vm/vm_api.h"
#include "cpu/crc_api.h"
#include "sdk_cfg.h"
#include "fat/fs_io.h"

A_B_REPEAT_OBJ ab_rept;

#if MUSIC_AB_RPT_EN

static u32 save_break_point(MUSIC_OP_API* m_op_api, void* buf)
{
	FS_BRK_POINT *bp_p = buf;
	DEC_API *dec_api = &m_op_api->dop_api->dec_api;

	if(0 == strcmp("FLA",dec_api->phy_ops->dec_ops->name))
	{
		memcpy(&bp_p->inf.flac_brk_buff,(u8 *)(dec_api->phy_ops->dec_ops->get_bp_inf(dec_api->phy_ops->dc_buf)),dec_api->bp_size);
	}
	else
	{
		memcpy(&bp_p->inf.norm_brk_buff,(u8 *)(dec_api->phy_ops->dec_ops->get_bp_inf(dec_api->phy_ops->dc_buf)),dec_api->bp_size);
	}
	/* bp_p->inf.crc=CRC16((u8*)&bp_p->inf.norm_brk_buff,VM_FILE_BK_LEN-2); */

	if(fs_get_brk_info(bp_p, m_op_api->fop_api->cur_lgdev_info->lg_hdl->file_hdl))
	{
		puts("fs_get_brk_info true\n");
		printf_buf((u8*)bp_p,20);
		return true;
	}
	else
	{
		puts("fs_get_brk_info false\n");
		return false;
	}
}


void * get_break_point_a(void *buf)
{
	if(ab_rept.a_stage)
	{
		return ab_rept.a_stage;
	}
	else
	{
		return NULL;
	}
}

void clean_ab_rpt(void)
{
	if(ab_rept.a_stage)
	{
		free_fun((void **)&ab_rept.a_stage);
	}

	ab_rept.b_stage.play_time = 0;
	ab_rept.b_stage.file_num = 0;
	ab_rept.statu = AB_RPT_NON;
}

void switch_a_b_repeat(MUSIC_OP_API* m_op_api)
{
	switch(ab_rept.statu)
	{
		case AB_RPT_NON:
			puts("AB_RPT_NON\n");
			if(ab_rept.a_stage == NULL)
			{
				ab_rept.a_stage = malloc(sizeof(FS_BRK_POINT));
				ASSERT(ab_rept.a_stage);
				memset(ab_rept.a_stage,0x00,sizeof(FS_BRK_POINT));
			}
			if(save_break_point(m_op_api, ab_rept.a_stage))
			{
				ab_rept.statu = AB_RPT_ASTA;
				ab_rept.b_stage.file_num = (u32)m_op_api->dop_api->file_num;//mark file_number when save a bpoint
			}
			else	//save_break_point_a err, rest to statu_non
			{
				free_fun((void **)&ab_rept.a_stage);
				ab_rept.statu = AB_RPT_NON;
				ab_rept.b_stage.play_time = 0;
			}
			break;

		case AB_RPT_ASTA:
			puts("AB_RPT_ASTA \n");
			if(ab_rept.b_stage.file_num == (u32)(m_op_api->dop_api->file_num))
			{
				ab_rept.b_stage.play_time = get_decode_time();
				ab_rept.statu = AB_RPT_BSTA;
			}
			else
			{
				puts("AB_RPT_ASTA_err\n");
				clean_ab_rpt();
			}
			break;

		case AB_RPT_BSTA:
			puts("AB_RPT_BSTA\n");
			free_fun((void **)&ab_rept.a_stage);
			ab_rept.b_stage.play_time = 0;
			ab_rept.b_stage.file_num = 0;
			ab_rept.statu = AB_RPT_NON;
			break;

		default:
			break;
	}
}


#else

void * get_break_point_a(void *buf)
{
	return NULL;
}

void clean_ab_rpt(void)
{
}
#endif //MUSIC_AB_RPT
