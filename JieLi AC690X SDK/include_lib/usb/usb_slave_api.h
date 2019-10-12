#ifndef	_USB_SLAVE_API_H_
#define _USB_SLAVE_API_H_

#include "typedef.h"

//----------------------------------
// HID key for audio
//----------------------------------
#define USB_AUDIO_STOP          0x8000
#define USB_AUDIO_FASTFORWARD   0x4000
#define USB_AUDIO_PREFILE       0x2000
#define USB_AUDIO_NEXTFILE      0x1000
#define USB_AUDIO_PP            0x0800
#define USB_AUDIO_MUTE          0x0400
#define USB_AUDIO_VOLDOWN       0x0200
#define USB_AUDIO_VOLUP         0x0100
#define USB_AUDIO_NONE          0x0000

#define USB_AUDIO_PAUSE         0x0080
#define USB_AUDIO_PLAY          0x0040
#define USB_AUDIO_BALANCE_LEFT  0x0020
#define USB_AUDIO_BALANCE_RIGHT 0x0010
#define USB_AUDIO_VOLUME        0x0008
#define USB_AUDIO_STOP_EJECT    0x0004
#define USB_AUDIO_TRACKING_DEC  0x0002
#define USB_AUDIO_TRACKING_INC  0x0001
#define USB_AUDIO_NONE          0x0000

//----------------------------------
// IO CONTROL CMD
//----------------------------------
enum
{
	USB_SLAVE_CLASS_CONFIG_RESET = 1,
	USB_SLAVE_CARD_READER0_REG,
	USB_SLAVE_CARD_READER1_REG,
	USB_SLAVE_GET_SPEAKER_VOL,
	USB_SLAVE_ONLINE_DETECT,
	USB_SLAVE_GET_ONLINE_STATUS,
	USB_SLAVE_RUN_CARD_READER,
	USB_SLAVE_HID,
	USB_SLAVE_MD_PROTECT,
};

typedef struct _card_reader_parm
{
    u32 (* usb_get_dev_capacity)(void);
    u8  *sDevName;
    u8  bWriteProtectFlag;
	u8  align[3];
}card_reader_parm;

typedef struct _card_reader_status
{    
    u8 working;
    u8 busy;
    u8 align[2];    
}card_reader_status; 

typedef struct _usb_slave_init_parm
{
	void (*os_msg_post_init)(void);	
	u8 vol_left;
	u8 vol_right;
}usb_slave_init_parm;

typedef struct _usb_slave_dev_io
{
	s32 (*dev_init)		(void *parm);	
	s32 (*dev_open)		(void *parm);	
	s32 (*dev_close)	(void *parm);	
	s32 (*dev_ioctrl)	(void *parm, u32 cmd);	
}usb_slave_dev_io;

typedef struct _usb_pc_protect
{
	u32  gc_ProtectFATNum;
	u32  ProtectedFATSectorIndex[0];
}usb_pc_protect;

extern const usb_slave_dev_io susb_io; 

// extern function
void usb_class_setting(u8 config);
u32 get_pc_volume(void);
void usb_hid_control(u32 key);
void reg_usb_pc_protect(void * protect_param);
void usb_device_default_register(void);

#endif	/*	_USB_SLAVE_API_H_	*/
