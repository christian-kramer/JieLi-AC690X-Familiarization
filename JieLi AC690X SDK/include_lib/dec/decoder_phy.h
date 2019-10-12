#ifndef _DECODER_PHY_H_
#define _DECODER_PHY_H_

//#include "if_dec_file.h"
#include "if_decoder_ctrl.h"
#include "rtos/os_api.h"
#include "stdarg.h"
#include "sup_dec_op.h"


#define UART_DEC_PHY_DEG
#ifdef UART_DEC_PHY_DEG
    #define dec_phy_puts           puts
    #define dec_phy_u32d           put_u32d
    #define dec_phy_u32hex0        put_u32hex0
    #define dec_phy_u32hex         put_u32hex
    #define dec_phy_printf         printf

    #define dec_phy_without_lock0  printf_without_lock
    #define dec_phy_without_lock1  puts_without_lock
    #define dec_phy_u16hex         put_u16hex
    #define dec_phy_u8hex0         put_u8hex0
    #define dec_phy_u8hex          put_u8hex
    #define dec_phy_buf            put_buf
    #define dec_phy_buf0           printf_buf
    #define dec_phy_char           putchar
#else
    #define dec_phy_puts(...)
    #define dec_phy_u32d(...)
    #define dec_phy_u32hex0(...)
    #define dec_phy_u32hex(...)
    #define dec_phy_printf(...)

    #define dec_phy_without_lock0(...)
    #define dec_phy_without_lock1(...)
    #define dec_phy_u16hex(...)
    #define dec_phy_u8hex0(...)
    #define dec_phy_u8hex(...)
    #define dec_phy_buf(...)
    #define dec_phy_buf0(...)
    #define dec_phy_char(...)
#endif



enum
{   MAD_ERROR_FILE_END         = 0x40,
    MAD_ERROR_FILESYSTEM_ERR   = 0x41,              // NO USED
    MAD_ERROR_DISK_ERR         = 0x42,              // NO USED
    MAD_ERROR_SYNC_LIMIT       = 0x43,              // 文件错误
    MAD_ERROR_FF_FR_FILE_END   = 0x44,              //快进结束
    MAD_ERROR_FF_FR_END        = 0x45,              // NO USED
    MAD_ERROR_FF_FR_FILE_START = 0x46,              //快退到头
    MAD_ERROR_LIMIT            = 0x47,              // NO USED
    MAD_ERROR_NODATA           = 0x48,              // NO USED
};

typedef enum __DECODE_STATUS
{
    DECODER_STOP = 0,
    DECODER_PAUSE,
    DECODER_PLAY = 0x55aa,
}_DECODE_STATUS;

typedef enum __FORMAT_STATUS
{
    FORMAT_OK = 0,
    FORMAT_OK_BUT_NO_SUPPORT,                  //格式正确但不支持的时候，不再继续检查
    FORMAT_ERR,
    FORMAT_NOT_DETECTED,
}_FORMAT_STATUS;

typedef struct __FF_FR
{
    u32 ff_disable_time;  //快进导致切换歌曲时，锁定歌曲不能快进的秒数
    u32 fr_disable_time;  //快退导致切换歌曲时，锁定歌曲不能快退的秒数
    bool fr_stop_dec;     //快退切歌曲使能
    u8 ff_fr_enable;
    volatile u8 ff_fr;
}_FF_FR;

struct if_dec_file {
	int (*read)(void *priv, void *data, int len);
	int (*seek)(void *priv, int from, int offset);
};


typedef struct _DEC_PHY
{
	const char *father_name;
	decoder_ops_t *dec_ops;
	void *dc_buf;
	struct if_decoder_io dec_io;
	struct if_dec_file *file_ops;
	void *dec_dev_priv;
	u32 (*dev_online)(void *priv);
	void (*dec_reset)(struct _DEC_PHY *phy_ops);
	volatile _DECODE_STATUS status;
	volatile u8 busy;
	_FF_FR ff_fr;
}DEC_PHY;


typedef struct _DEC_API_IO
{
    void *father_name;
    void *f_p;
    u16(*f_read)(void *fp , void *buff, u16 len);
    u32 (*f_seek)(void *fp , u8 type, u32 offsiz);
    u32 (*set_music_info)(dec_inf_t *inf,tbool wait);
    _OP_IO output;
    u32 op_cnt; //输出的样点累计数
    volatile tbool dev_active;
}DEC_API_IO;

///*AAC解码*/
typedef enum
{
    SET_DEFAULT_NOTING,
    SET_AAC_SR_NCH_BR ,
}_MUSIC_DEC_CONFIG;

typedef struct _AAC_DEFAULT_SETTING
{
    u32 samplerate;
    u32 nchannels;
    u32 bitrate;
}AAC_DEFAULT_SETTING;


typedef struct _DEC_API
{
    DEC_PHY *phy_ops;
    void *format_name;
    void *op_priv;
    void *(*dec_op_clr)(void *priv);
    tbool *(*dec_op_over)(void *priv);
    void *dec_phy_name;
    AAC_DEFAULT_SETTING AAC_info;
    tbool save_brk;
//    tbool enable_brk;
    u32 bp_size; //断点长度
    u8 *bp_buff; //断点长度
	u8 *flac_bp_buff;
    dec_inf_t *inf;
    u32 type_enable;
    u8 ef_enable;
    void *ef_info;
    u8 eq;
    void *ef_out_name;
}DEC_API;

typedef struct _FRFF_ENABLE
{
	bool fr_disable;
	bool ff_disable;
}FRFF_ENABLE;

enum
{
#if DEC_TYPE_SBC_ENABLE
    DEC_TYPE_SBC = 0,
#endif

#if DEC_TYPE_WAV_ENABLE
    DEC_TYPE_WAV,
#endif

#if DEC_TYPE_WMA_ENABLE
    DEC_TYPE_WMA,
#endif

#if DEC_TYPE_AAC_ENABLE
    DEC_TYPE_AAC,
#endif

#if DEC_TYPE_FLAC_ENABLE
    DEC_TYPE_FLAC,
#endif

#if DEC_TYPE_APE_ENABLE
    DEC_TYPE_APE,
#endif

#if DEC_TYPE_MP3_ENABLE
    DEC_TYPE_MP3,
#endif

#if DEC_TYPE_F1A_ENABLE
    DEC_TYPE_F1A,
#endif
		DEC_TYPE_NULL = 0xff,
};


#define DEC_PHY_MP3  BIT(DEC_TYPE_MP3)
#define DEC_PHY_SBC  BIT(DEC_TYPE_SBC)

#if DEC_TYPE_WAV_ENABLE
    #define DEC_PHY_WAV  BIT(DEC_TYPE_WAV)
#else
    #define DEC_PHY_WAV  0
#endif

#if DEC_TYPE_WMA_ENABLE
    #define DEC_PHY_WMA  BIT(DEC_TYPE_WMA)
#else
    #define DEC_PHY_WMA  0
#endif

#if DEC_TYPE_AAC_ENABLE
    #define DEC_PHY_AAC  BIT(DEC_TYPE_AAC)
#else
    #define DEC_PHY_AAC  0
#endif

#if DEC_TYPE_FLAC_ENABLE
    #define DEC_PHY_FLAC BIT(DEC_TYPE_FLAC)
#else
    #define DEC_PHY_FLAC 0
#endif

#if DEC_TYPE_APE_ENABLE
    #define DEC_PHY_APE  BIT(DEC_TYPE_APE)
#else
    #define DEC_PHY_APE 0
#endif

#if DEC_TYPE_F1A_ENABLE
    #define DEC_PHY_F1A  BIT(DEC_TYPE_F1A)
#else
    #define DEC_PHY_F1A 0
#endif


#if DEC_TYPE_MP3_ENABLE
    extern decoder_ops_t *get_mp3_ops(void);
#endif

#if DEC_TYPE_SBC_ENABLE
    extern decoder_ops_t *get_sbcd_ops(void);
#endif

#if DEC_TYPE_WAV_ENABLE
    extern decoder_ops_t *get_wav_ops(void);
#endif

#if DEC_TYPE_WMA_ENABLE
    extern decoder_ops_t *get_wma_ops(void);
#endif

#if DEC_TYPE_AAC_ENABLE
    extern decoder_ops_t *get_aac_ops(void);
#endif

#if DEC_TYPE_FLAC_ENABLE
    extern decoder_ops_t *get_flac_ops(void);
#endif

#if DEC_TYPE_APE_ENABLE
    extern decoder_ops_t *get_ape_ops(void);
#endif

#if DEC_TYPE_F1A_ENABLE
    extern decoder_ops_t *get_f1a_ops(void);
#endif

//DEC_PHY_WAV |  DEC_PHY_WMA | DEC_PHY_SBC | DEC_PHY_AAC | DEC_PHY_FLAC | DEC_PHY_APE | DEC_PHY_MP3

#define MIDI_CMD_SAMPLERATE   0

#define MIDI_SAMPLE_R22050   22050
#define MIDI_SAMPLE_R16000   16000
#define MIDI_SAMPLE_R12000   12000
#define MIDI_SAMPLE_R8000    8000

#define STOP_DEC_NULL           BIT(0)
#define HAS_STOP_DEC_OPT        BIT(1)
#define STOP_DEC_GOT_BRKINFO    BIT(2)
#define STOP_DEC_GOT_FLACBRKINFO    BIT(3)
void *malloc_fun(void *ptr,u32 len,char *info);
void free_fun(void **ptr);

u8 dec_ff_fr(void *priv,u8 ff_fr,u8 second);
void decoder_phy_task(void *p_arg);
u32 stop_decoder(DEC_API *dec_api);
u32 start_decoder(DEC_API *dec_api,DEC_API_IO *api_io,void *bp_buf);
u32 kick_start_decoder(DEC_API *dec_api);
_DECODE_STATUS pp_decode(DEC_API *dec_api);
_DECODE_STATUS get_decode_statu(DEC_API *dec_api);
u8 get_decode_nch(void);

extern u32 get_decode_time(void);
#endif
