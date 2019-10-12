#ifndef __MUSIC_DECRYPT_H__
#define __MUSIC_DECRYPT_H__

#include "typedef.h"

typedef struct _CIPHER
{
	//      u8  cipher_code[4];
	u32 cipher_code;        ///>解密key
	u32 cipher_database;    ///>文件地址，扇区，用于辨别是否需要解密
	u8  cipher_enable;      ///>解密读使能
}CIPHER;


void get_decode_file_info(void *file);
void cipher_init(u32 key);
void cipher_close(void);
extern void reg_dec_input_cb(void * fun);


#endif //__MUSIC_DECRYPT_H__
