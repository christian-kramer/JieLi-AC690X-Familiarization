#ifndef _MUSIC_API_H_
#define _MUSIC_API_H_

#include "file_operate/file_op.h"
#include "decoder_phy.h"
#include "file_operate/fs_io_h.h"

/*
typedef enum __FORMAT_STATUS
{
}_FORMAT_STATUS;
*/

typedef struct __MUSIC_API
{
    void *dec_phy_name;
    void *file_type;
    u32 file_num;
    DEC_API dec_api;
    DEC_API_IO *io;
    FS_BRK_POINT *brk;
}_MUSIC_API;

typedef struct _MUSIC_OP_API_
{
    FILE_OPERATE *fop_api;
    _MUSIC_API   *dop_api;
}MUSIC_OP_API;

typedef struct _MP3_ID3
{
    u8 *id3_buf;
    u32 id3_len;
}MP3_ID3;

typedef void * (*GET_ID3_FUN)(u32 len);


u32 music_play(MUSIC_OP_API *m_op_api,ENUM_DEV_SEL_MODE dev_sel,u32 dev_let,ENUM_FILE_SELECT_MODE file_sel,void *pra);
void music_stop_decoder(MUSIC_OP_API *parm);
void music_decoder_info(void * before_fun, void * after_fun);

void reg_get_id3v2_buf(GET_ID3_FUN fun);
MP3_ID3 * get_id3v2_info(void);

#endif
