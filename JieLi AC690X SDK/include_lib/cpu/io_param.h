#ifndef __IO_PARAM_H__
#define __IO_PARAM_H__


enum
{
    IO_NO_ERR             =0,
    IO_DRV_NAME_ERR       =-1000,
    IO_DRV_CMD_ERR        ,
};

enum
{
    IO_CMD_OP_DIR         =0,
    IO_CMD_OP_DATA          ,
    IO_CMD_OP_PU            ,
    IO_CMD_OP_PD            ,
    IO_CMD_OP_HD            ,
    IO_CMD_OP_DIE           ,
};



#endif
