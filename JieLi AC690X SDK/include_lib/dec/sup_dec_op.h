#ifndef _SUP_DEC_OP_H_
#define _SUP_DEC_OP_H_

#include "typedef.h"
#include "if_decoder_ctrl.h"

typedef struct __OP_IO
{
    void *priv;
    void*(*output)(void *priv , void *buff, u32 len);
}_OP_IO;


u32 set_music_sr(dec_inf_t *inf,tbool);
extern void *sup_dec_phy_op(void *priv  ,void *data, u32 len);
extern tbool sup_dec_op_over(void *priv);
extern void sup_dec_op_clear(void *priv);

#endif
