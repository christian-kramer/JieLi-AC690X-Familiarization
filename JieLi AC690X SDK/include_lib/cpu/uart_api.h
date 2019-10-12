#ifndef __UART_API_H__
#define __UART_API_H__

#include "typedef.h"
#include "uart_param.h"

typedef struct uart_handle
{
    char name[5];
    void (*putbyte)(void *param,char a);
    char (*getbyte)(void *param);
}__uart_handle;


void uart_module_on();
void uart_module_off();
s32 uart_module_open(__uart_handle *handle,char *name);
s32 uart_module_close(__uart_handle *handle);
s32 uart_module_init(__uart_handle *handle,__uart_param *param);
s32 uart_module_start(__uart_handle *handle);
s32 uart_module_stop(__uart_handle *handle);
s32 uart_reg_isr_callback_fun(__uart_handle *handle,u32 priority,void (*callback_fun)(u8 value,void *p,u8 isr_flag));
s32 uart_unreg_isr_callback_fun(__uart_handle *handle,u32 priority,void (*callback_fun)(u8 value,void *p,u8 isr_flag));
s32 uart_module_ctrl(__uart_handle *handle,u32 cmd,u32 para,void *p);

#endif
