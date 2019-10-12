#ifndef CIRCULAR_BUF_INTERFACE_H
#define CIRCULAR_BUF_INTERFACE_H


#include "typedef.h"



typedef struct _cbuffer
{
    u8 *begin;
    u8 *end;
    u8  *read_ptr;
    u8  *write_ptr;
    u8  *tmp_ptr ;
    u32 tmp_len;
    u32 data_len;
    u32 total_len;
}cbuffer_t;

extern void cbuf_init(cbuffer_t *cbuffer, void *buf, u32 size);

extern u32 cbuf_read(cbuffer_t *cbuffer, void *buf, u32 len);

extern u32 cbuf_write(cbuffer_t *cbuffer, void *buf, u32 len);

extern u32 cbuf_is_write_able(cbuffer_t *cbuffer, u32 len);

extern void * cbuf_write_alloc(cbuffer_t *cbuffer, u32 *len);

extern void  cbuf_write_updata(cbuffer_t *cbuffer, u32 len);

void * cbuf_read_alloc(cbuffer_t *cbuffer, u32 *len);

void cbuf_read_updata(cbuffer_t *cbuffer, u32 len);

void cbuf_clear(cbuffer_t *cbuffer);

u32 cbuf_rewrite(cbuffer_t *cbuffer, void *begin ,void *buf, u32 len);

void  cbuf_discard_prewrite(cbuffer_t *cbuffer);

void cbuf_updata_prewrite(cbuffer_t *cbuffer);

u32 cbuf_prewrite(cbuffer_t *cbuffer, void *buf, u32 len);

#define cbuf_get_writeptr(a) (a)->write_ptr

#define cbuf_get_data_size(a) (a)->data_len

#define cbuf_get_readptr(a ) (a)->read_ptr

#define cbuf_get_space(a) (a)->total_len

#endif
