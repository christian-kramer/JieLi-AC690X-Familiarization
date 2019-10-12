#ifndef __MUSIC_AB_RPT_H_
#define __MUSIC_AB_RPT_H_

#include "typedef.h"
#include "dec/music_api.h"
#include "file_operate/fs_io_h.h"

typedef struct _FS_BRK_POINT_B
{
    u32 file_num;
    u32 play_time;
}FS_BRK_POINT_B;

typedef enum
{
    AB_RPT_NON = 0,
    AB_RPT_ASTA,
    AB_RPT_BSTA
}AB_RPT_STA;



typedef struct _A_B_REPEAT_OBJ
{
    AB_RPT_STA statu;
    FS_BRK_POINT *a_stage;
    FS_BRK_POINT_B b_stage;

}A_B_REPEAT_OBJ;

extern A_B_REPEAT_OBJ ab_rept;
void switch_a_b_repeat(MUSIC_OP_API* m_op_api);
void clean_ab_rpt(void);


#endif // __MUSIC_AB_RPT_H_
