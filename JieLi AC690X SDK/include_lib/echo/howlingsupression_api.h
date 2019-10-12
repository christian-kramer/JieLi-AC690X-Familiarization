#ifndef howlingsuppress_api_h__
#define howlingsuppress_api_h__

#ifndef u8
#define u8  unsigned char
#endif

#ifndef u16
#define u16 unsigned short
#endif

#ifndef s16
#define s16 short
#endif


#ifndef u32
#define u32 unsigned int
#endif


#ifndef s32
#define s32 int
#endif

#ifndef s16
#define s16 signed short
#endif


#define delay_MAX 100


typedef struct _Howlingsup_IO_
{
	void *priv;
	void (*output)(void *priv,short *outdata,short len);               //len是多少个byte
}Howlingsup_IO;



typedef struct _HowlingSupression_FUNC_API_
{
	u32 (*need_buf)(void);
	u32 (*init)(unsigned char *ptr,Howlingsup_IO *rv_io,unsigned char suppressionV);  //中途改变参数，可以调init
	u32 (*run)(unsigned char *ptr,short *indata,short len);                                //len是多少个byte

}HowlingSupression_FUNC_API;

typedef struct _HowlingSupression_API_STRUCT_
{
	HowlingSupression_FUNC_API *howlingsuppress_api;
	Howlingsup_IO *howlingsuppress_io;
	u8 *howlingsuppress_ptr;
}HowlingSupression_API_STRUCT;

extern HowlingSupression_FUNC_API *get_howlingsupression_func_api();


#endif // reverb_api_h__
