#ifndef _VM_H_
#define _VM_H_


#include "common/list.h"
#include "typedef.h"
#include "rtos/os_api.h"
#include "peripherals/winbond_flash.h"


#define AT_VM AT(.vm)


/*-------------------vm_new_version------------------------*/
#define VM_INDEX_DES_MAX    128

typedef struct _VM_DESC
{
    u16 v_addr;
    u16 v_len;
}VM_DESC;

typedef struct _AREA_RANGE
{
	u32 start_addr;
	u32 len;			//byte
	u16 erase_bit_map;	//surport 16 * 4 sector
}AREA_INFO;

typedef struct _VM_STRUCT
{
	AREA_INFO area[2];
	VM_DESC idx_desc[VM_INDEX_DES_MAX];
	struct list_head write_list;
	OS_MUTEX mutex;
	u32 last_write_addr;
	u8 using_area;		//0:main_area	1:temp_area
	u8 warning_line;	//percentage
	u8 init_flag;		//vm_init_statu
	// u8 defrag_flag;		//vm_defrag
}VM_STRUCT;
/*-------------------vm_new_version------------------------*/


enum
{
    VM_ERR_NONE = 0,
    VM_INDEX_ERR = -0x100,
    VM_INDEX_EXIST,
	VM_DATA_LEN_ERR,
    VM_READ_NO_INDEX,
    VM_READ_DATA_ERR,
    VM_WRITE_OVERFLOW,
	VM_NOT_INIT,
	VM_INIT_ALREADY,
	VM_DEFRAG_ERR,
    VM_ERR_INIT,
    VM_ERR_PROTECT
};

typedef s32	vm_err;
typedef u8	vm_hdl;

/*格式化VM_区域*/
extern vm_err vm_eraser(void);

/*
每次上电初始化VM系统,
need_defrag_when_write_full: 当vm区域写满时是否需要整理碎片
*/
// extern void vm_run_mode_and_keyt_init(u8 mode);
extern vm_err vm_init(u32 vm_addr, u32 vm_len, u32 spi_base, u8 mode, bool need_defrag_when_write_full);

/*
在 vm_write,vm_read 前必须先打开 指定index 获得句柄,
允许多次打开: vm_read/vm_write(vm_open(index,data_len),data_buf)
*/
extern s32 vm_open(u8 index, u16 data_len);

/*不再需用调用vm_write,vm_read 时可以关闭句柄释放内存*/
extern vm_err vm_close(vm_hdl hdl);

/*读写函数，返回正数表示实际读写长度，返回负数表示错误*/
extern s32 vm_write(vm_hdl hdl, const void *data_buf);
extern s32 vm_read(vm_hdl hdl, void * data_buf);

/*手动整理碎片*/
extern vm_err vm_defrag(void);

/*侦查 vm 区域 使用情况, 返回使用的百分比*/
extern vm_err vm_status(VM_STRUCT * vm_str_p);

extern const struct DEV_IO * dev_reg_vm(void *parm);

/*----------------------------------------------------------------------------*/
/**@brief  VM碎片检测函数
   @param  level：0：以最快速度整理VM区域，但不一定全区域整理，1：整理全VM区域
   @return NON
   @note   外部必须定时通过此接口查询VM是否需要整理。
*/
/*----------------------------------------------------------------------------*/
void vm_check_all(u8 level);

/*----------------------------------------------------------------------------*/
/**@brief  内部flash
   @param  cmd：擦方式
   @param  addr：擦地址，flash相对地址
   @return 0：擦错误，1：擦成功
   @note   u32 sfc_read(u8 *buf, u32 addr, u32 len)
*/
/*----------------------------------------------------------------------------*/
bool sfc_erase(FLASH_ERASER cmd, u32 addr);

/*----------------------------------------------------------------------------*/
/**@brief  内部flash
   @param  buf：写内容
   @param  addr：写地址，flash相对地址
   @param  len：写长度，单位byte
   @return 0：写错误，1：写成功len
   @note   u32 sfc_read(u8 *buf, u32 addr, u32 len)
*/
/*----------------------------------------------------------------------------*/
u32 sfc_write(const u8 *buf, u32 adr, u32 len);

/*----------------------------------------------------------------------------*/
/**@brief  内部flash
   @param  buf：读内容
   @param  addr：读地址，flash相对地址
   @param  len：读长度，单位byte
   @return 0：读错误，1：读成功
   @note   u32 sfc_read(u8 *buf, u32 addr, u32 len)
*/
/*----------------------------------------------------------------------------*/
u32 sfc_read(u8 *buf, u32 adr, u32 len);

#endif  //_VM_H_



