#ifndef __KEY_DRV_IR_H__
#define __KEY_DRV_IR_H__

#include "includes.h"
#include "key_drv/key.h"

/*------------------------------------------------------------------------
 *  IRFLT IO SEL：
 *  0x00--0x0f   PA[0:15]
 *  0x10--0x1f   PB[0:15]
 *  0x20--0x2f   PC[0:15] 
 *  0x30--0x37   PD[0:7]
 *  0x38--0x3d   Reserved
 *  0x3e         USBDP
 *  0x3f         USBDM
 * ----------------------------------------------------------------------*/
#define IR_PORTA(x) (0x00 + x)
#define IR_PORTB(x) (0x10 + x)
#define IR_PORTC(x) (0x20 + x)
#define IR_PORTD(x) (0x30 + x)
#define IR_USBDP    (0x3E)
#define IR_USBDM 	(0x3F)


#if KEY_IR_EN

/*ir key define*/
#define IR_00                   0
#define IR_01                   1
#define IR_02                   2
#define IR_03                   3
#define IR_04                   4
#define IR_05                   5
#define IR_06                   6
#define IR_07                   7
#define IR_08                   8
#define IR_09                   9
#define IR_10                   10
#define IR_11                   11
#define IR_12                   12
#define IR_13                   13
#define IR_14                   14
#define IR_15                   15
#define IR_16                   16
#define IR_17                   17
#define IR_18                   18
#define IR_19                   19
#define IR_20                   20
#define IR_21                   21
#define IR_22                   22

//////////////////////////////////
#define NKEY_00               0xff
#define NKEY_01               0xff
#define NKEY_02               0xff
#define NKEY_03               0xff
#define NKEY_04               0xff
#define NKEY_05               0xff
#define NKEY_06               0xff
#define NKEY_07               0xff
#define NKEY_08               0xff
#define NKEY_09               0xff
#define NKEY_0A               0xff
#define NKEY_0B               0xff
#define NKEY_0C               0xff
#define NKEY_0D               0xff
#define NKEY_0E               0xff
#define NKEY_0F               0xff
#define NKEY_10               0xff
#define NKEY_11               0xff
#define NKEY_12               0xff
#define NKEY_13               0xff
#define NKEY_14               0xff
#define NKEY_15               0xff
#define NKEY_16               0xff
#define NKEY_17               0xff
#define NKEY_18               0xff
#define NKEY_19               0xff
#define NKEY_1A               0xff
#define NKEY_1B               0xff
#define NKEY_1C               0xff
#define NKEY_1D               0xff
#define NKEY_1E               0xff
#define NKEY_1F               0xff
#define NKEY_20               0xff
#define NKEY_21               0xff
#define NKEY_22               0xff
#define NKEY_23               0xff
#define NKEY_24               0xff
#define NKEY_25               0xff
#define NKEY_26               0xff
#define NKEY_27               0xff
#define NKEY_28               0xff
#define NKEY_29               0xff
#define NKEY_2A               0xff
#define NKEY_2B               0xff
#define NKEY_2C               0xff
#define NKEY_2D               0xff
#define NKEY_2E               0xff
#define NKEY_2F               0xff
#define NKEY_30               0xff
#define NKEY_31               0xff
#define NKEY_32               0xff
#define NKEY_33               0xff
#define NKEY_34               0xff
#define NKEY_35               0xff
#define NKEY_36               0xff
#define NKEY_37               0xff
#define NKEY_38               0xff
#define NKEY_39               0xff
#define NKEY_3A               0xff
#define NKEY_3B               0xff
#define NKEY_3C               0xff
#define NKEY_3D               0xff
#define NKEY_3E               0xff
#define NKEY_3F               0xff
#define NKEY_40               0xff
#define NKEY_41               0xff
#define NKEY_42               0xff
#define NKEY_43               0xff
#define NKEY_44               0xff
#define NKEY_45               0xff
#define NKEY_46               0xff
#define NKEY_47               0xff
#define NKEY_48               0xff
#define NKEY_49               0xff
#define NKEY_4A               0xff
#define NKEY_4B               0xff
#define NKEY_4C               0xff
#define NKEY_4D               0xff
#define NKEY_4E               0xff
#define NKEY_4F               0xff
#define NKEY_50               0xff
#define NKEY_51               0xff
#define NKEY_52               0xff
#define NKEY_53               0xff
#define NKEY_54               0xff
#define NKEY_55               0xff
#define NKEY_56               0xff
#define NKEY_57               0xff
#define NKEY_58               0xff
#define NKEY_59               0xff
#define NKEY_5A               0xff
#define NKEY_5B               0xff
#define NKEY_5C               0xff
#define NKEY_5D               0xff
#define NKEY_5E               0xff
#define NKEY_5F               0xff
#define NKEY_60               0xff
#define NKEY_61               0xff
#define NKEY_62               0xff
#define NKEY_63               0xff
#define NKEY_64               0xff
#define NKEY_65               0xff
#define NKEY_66               0xff
#define NKEY_67               0xff
#define NKEY_68               0xff
#define NKEY_69               0xff
#define NKEY_6A               0xff
#define NKEY_6B               0xff
#define NKEY_6C               0xff
#define NKEY_6D               0xff
#define NKEY_6E               0xff
#define NKEY_6F               0xff
#define NKEY_70               0xff
#define NKEY_71               0xff
#define NKEY_72               0xff
#define NKEY_73               0xff
#define NKEY_74               0xff
#define NKEY_75               0xff
#define NKEY_76               0xff
#define NKEY_77               0xff
#define NKEY_78               0xff
#define NKEY_79               0xff
#define NKEY_7A               0xff
#define NKEY_7B               0xff
#define NKEY_7C               0xff
#define NKEY_7D               0xff
#define NKEY_7E               0xff
#define NKEY_7F               0xff
#define NKEY_80               0xff
#define NKEY_81               0xff
#define NKEY_82               0xff
#define NKEY_83               0xff
#define NKEY_84               0xff
#define NKEY_85               0xff
#define NKEY_86               0xff
#define NKEY_87               0xff
#define NKEY_88               0xff
#define NKEY_89               0xff
#define NKEY_8A               0xff
#define NKEY_8B               0xff
#define NKEY_8C               0xff
#define NKEY_8D               0xff
#define NKEY_8E               0xff
#define NKEY_8F               0xff
#define NKEY_90               0xff
#define NKEY_91               0xff
#define NKEY_92               0xff
#define NKEY_93               0xff
#define NKEY_94               0xff
#define NKEY_95               0xff

typedef struct _IR_CODE
{
    u16 wData;          //<键值
    u8  bState;         //<接收状态
    u16 wUserCode;      //<用户码
    u8  boverflow;      //<红外信号超时
} IR_CODE;

tu8 get_irkey_value(void);
extern void ir_count(void);
extern s32 ir_key_init(void);
extern  u8  ir_key_get_value(void);
extern  u8  ir_key_pressed_to_msg(u8 key_style, u8 key_num);
extern void ir_timeout(void *param);
extern void set_ir_clk(void);
#endif

extern u16 timer2_pad;

#endif

