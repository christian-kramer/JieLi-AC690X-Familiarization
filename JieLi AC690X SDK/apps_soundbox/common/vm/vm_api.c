#include "vm/vm_api.h"
#include "dac/dac_api.h"
#include "common/flash_cfg.h"
#include "common/app_cfg.h"
#include "sdk_cfg.h"
#include "peripherals/winbond_flash.h"

#define MAX_VM_CACHE 3
vm_hdl *vm_ptr = NULL;
_VM_CACHE *vm_cache = NULL;

extern void free_fun(void **ptr);
extern void *malloc_fun(void *ptr,u32 len,char *info);
/*----------------------------------------------------------------------------*/
/** @brief: 开辟的各个存储单元的长度
    @param: MAX_VM：开辟的存储单元数目
    @return:void
    @author:lj
    @note:  const u8 vm_index_len[MAX_VM]
*/
/*----------------------------------------------------------------------------*/
const u16 vm_index_len[VM_MAX_SIZE] =
{
    VM_SYS_VOL_LEN,/* VM_SYS_VOL = 0,  */
    VM_SYS_EQ_LEN,/*VM_SYS_EQ         */
    VM_DEV0_BK_LEN,/* VM_DEV_BREAKPOINT0, */
    VM_DEV1_BK_LEN,/* VM_DEV_BREAKPOINT1, */
    VM_DEV2_BK_LEN,/* VM_DEV_BREAKPOINT2, */
    VM_DEV3_BK_LEN,/* VM_DEV_BREAKPOINT3, */
    VM_DEV0_FLACBK_LEN,/* VM_DEV_BREAKPOINT0, */
    VM_DEV1_FLACBK_LEN,/* VM_DEV_BREAKPOINT1, */
    VM_DEV2_FLACBK_LEN,/* VM_DEV_BREAKPOINT2, */
    VM_DEV3_FLACBK_LEN,/* VM_DEV_BREAKPOINT3, */
    VM_MUSIC_DEVICE_LEN,
    VM_PC_VOL_LEN, /*PC VOL*/
    VM_FM_INFO_LEN,
	VM_PHONE_VOL_LEN,
    VM_STEREO_INFO_LEN,
    VM_LANGUAGE_MODE,
	 VM_MODE_LEN,
    /* MAX_VM,          */
};

/*----------------------------------------------------------------------------*/
/** @brief: 初始化VM，并打开所有存储单元
    @param: void
    @return:void
    @author:lj
    @note:  void vm_open_all(void
*/
/*----------------------------------------------------------------------------*/
void vm_open_all(void)
{
    u32 i;
    vm_ptr = NULL;
    vm_cache = NULL;
    vm_ptr = (vm_hdl *)malloc_fun(vm_ptr,sizeof(vm_hdl)*VM_MAX_SIZE,0);
    vm_cache = (_VM_CACHE *)malloc_fun(vm_cache,sizeof(_VM_CACHE)*MAX_VM_CACHE,0);
    vm_cache[0].index = VM_SYS_VOL;
    vm_cache[0].dat_len = VM_SYS_VOL_LEN;

    vm_cache[1].index = VM_PC_VOL;
    vm_cache[1].dat_len = VM_PC_VOL_LEN;

    vm_cache[2].index = VM_SYS_EQ;
    vm_cache[2].dat_len = VM_SYS_EQ_LEN;

    for(i = 0; i < VM_MAX_SIZE; i++)
    {
        if(vm_index_len[i])
        {
            vm_ptr[i] = vm_open(i + VM_START_INDEX,vm_index_len[i]);
        }

    }
}

/*----------------------------------------------------------------------------*/
/** @brief: 按照索引号写存储信息
    @param: index：存储单元索引
    @param: data_buf：需要存储的数据的指针
    @return:正确返回长度
    @author:lj
    @note:  vm_err vm_write_api(u8 index ,const void *data_buf)
*/
/*----------------------------------------------------------------------------*/
s32 vm_write_api(u8 index ,const void *data_buf)
{
    return vm_write(vm_ptr[index - VM_START_INDEX],data_buf);
}
/*----------------------------------------------------------------------------*/
/** @brief: 按照索引号读存储信息
    @param: index：存储单元索引
    @param: data_buf：需要读取的数据的指针
    @return:正确返回长度
    @author:lj
    @note:  vm_err vm_read_api(u8 index, void * data_buf)
*/
/*----------------------------------------------------------------------------*/
s32 vm_read_api(u8 index, void * data_buf)
{
	/* puts("vm_read_api\n"); */
    return vm_read(vm_ptr[index - VM_START_INDEX],data_buf);
}
/*----------------------------------------------------------------------------*/
/** @brief: 按照索引号缓存存储信息，但是不直接写入VM
    @param: index：存储单元索引
    @param: data_buf：需要读取的数据的指针
    @param: cnt ：在cnt个节拍之后，将数据存入VM
    @return:vm_err：操作结果
    @author:lj
    @note:  vm_err vm_cache_write(u8 index ,const void *data_buf,s16 cnt)
*/
/*----------------------------------------------------------------------------*/
vm_err vm_cache_write(u8 index ,const void *data_buf,s16 cnt)
{
    u32 i;
    for(i = 0; i < MAX_VM_CACHE; i++)
    {
        if(vm_cache[i].index == index)
        {
            if(-1 != vm_cache[i].cnt)
            {
                if(vm_cache[i].cnt==0)
                {
                    vm_cache[i].buff = (void *)malloc_fun(vm_cache[i].buff,vm_index_len[i],0);
                }
                if(vm_cache[i].buff!=NULL)
                {
                    memcpy(vm_cache[i].buff,data_buf,vm_index_len[i]);
                }

                vm_cache[i].cnt = cnt;
                return VM_ERR_NONE;
            }
            else
            {
                return -1;//VM_BUSY;
            }

        }
    }
    return -VM_WRITE_OVERFLOW;//vm_write(vm_ptr[index],data_buf);
}
/*----------------------------------------------------------------------------*/
/** @brief: 扫描缓存，提交需要写入VM的数据
    @param: void
    @return:vm_err：操作结果
    @author:lj
    @note:  vm_err vm_cache_submit(void)
*/
/*----------------------------------------------------------------------------*/
vm_err vm_cache_submit(void)
{
    u32 i;

    for(i = 0; i < MAX_VM_CACHE; i++)
    {
        if(vm_cache[i].cnt)
        {
            vm_cache[i].cnt--;
            if(0 == vm_cache[i].cnt)
            {
                vm_cache[i].cnt = -1;
                //deg_puts("submit vm\n");
                if( vm_write_api(vm_cache[i].index,vm_cache[i].buff)==vm_cache[i].dat_len)
                {
                    free_fun(&vm_cache[i].buff);
                    vm_cache[i].cnt = 0;
                    puts("submit vm succ\n");
                }
                else
                {
                    puts("submit vm err\n");
                    vm_cache[i].cnt = 1;
                }
            }
        }
    }
    return VM_ERR_NONE;
}


/*----------------------------------------------------------------------------*/
/** @brief: vm test
    @param: void
    @return:
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
void vm_test(void)
{
#if 0
    if(1)
    {
        puts("\n\n\n======================TEST for VM====================\n");

        u8* buf = 0;;
        u8 buf_cnt = 0;
        u8 hd_cnt = 0;
        u32 ret;

	   vm_eraser();
	   /* while(1); */

        while(1)
        {
            if(buf_cnt >= 32)
            {
                buf_cnt = 0;
                hd_cnt++;
                if(hd_cnt>=256)
                {
                    puts("\n\n\n\n======================TEST OVER====================\n");
                    while(1);
                }
            }

            buf = malloc(++buf_cnt);
            ASSERT(buf);
            printf("\nhd_cnt = %d    buf_cnt = %d\n",hd_cnt,buf_cnt);

            memset(buf,0xfe,buf_cnt);
            vm_hdl vm_ptr = vm_open(hd_cnt,buf_cnt);

            ret = vm_write(vm_ptr,buf);
            if(ret != buf_cnt)
            {
                puts("vm_write err\n");
                printf("ret = %d\n",ret);
            }

            memset(buf,0x00,buf_cnt);
            ret = vm_read(vm_ptr,buf);
            if(ret != buf_cnt)
            {
                puts("vm_read err\n");
                printf("ret = %d\n",ret);
            }
            printf_buf(buf,buf_cnt);
            vm_close(vm_ptr);
            free(buf);
        }
    }
#endif
}

/*----------------------------------------------------------------------------*/
/** @brief: 初始化VM的存储模式
    @param: cfg - flash参数
    @return:void
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
vm_err vm_init_api(struct flash_cfg *cfg)
{
    u32 tmp;
    u8 vm_mode;

    if(((cfg->spi_run_mode & 3) == 0) || ((cfg->spi_run_mode & 3) == 1))
    {
        //spi2_parm = SPI_ODD_MODE;
        vm_mode = WINBOND_READ_DATA_MODE;
    }
    else if((cfg->spi_run_mode & 3) == 2)
    {
        //spi2_parm = SPI_DUAL_MODE;
        if(cfg->spi_run_mode & BIT(3))
        {
            vm_mode = WINBOND_FAST_READ_DUAL_OUTPUT_MODE;
        }
        else
        {
            if(cfg->spi_run_mode & BIT(2))
            {
                vm_mode = WINBOND_FAST_READ_DUAL_IO_CONTINUOUS_READ_MODE;
            }
            else
            {
                vm_mode = WINBOND_FAST_READ_DUAL_IO_NORMAL_READ_MODE;
            }
        }
    }
    else
    {
        if(cfg->spi_run_mode & BIT(3))
        {
            vm_mode = WINBOND_FAST_READ_QUAD_OUTPUT_MODE;
        }
        else
        {
            if(cfg->spi_run_mode & BIT(2))
            {
                vm_mode = WINBOND_FAST_READ_QUAD_IO_CONTINUOUS_READ_MODE;
            }
            else
            {
                vm_mode = WINBOND_FAST_READ_QUAD_IO_NORMAL_READ_MODE;
            }
        }
    }

    if(vm_init(app_use_flash_cfg.vmif_addr ,app_use_flash_cfg.vmif_len, cfg->flash_base, vm_mode, 1)!= VM_ERR_NONE)
    {
        puts("\n\n\n\n----WARN-vm_init_err----\n");
		return VM_ERR_INIT;
    }
    puts("\n===vm_init_ok===\n");

    vm_test();

	return VM_ERR_NONE;
}

/*----------------------------------------------------------------------------*/
/**@brief  vm 延时写入task
   @param  p_arg：no used
   @return
   @note   delay time is 2 seconds
*/
/*----------------------------------------------------------------------------*/
extern bool is_bt_state_deal_vm();
static void TaskVmCache(void *p_arg)
{
    puts("********TaskVmCache********\n");
    //printf_buf(MMU_TAG_ADDR0, 128);
    //puts("\n");
    while(1)
    {
        os_time_dly(200);
        if(0 != is_bt_state_deal_vm())
        {
            vm_cache_submit();
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  创建vm定时写入task
   @param  p_arg：no used
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void vm_cache_start(void)
{
//    vm_init_api(&g_sys_cfg.flash_cfg);
//
//    vm_open_all();
    u32 err;
    err = os_task_create_ext(TaskVmCache,
                             (void*)0,
                             SecondPrio,
                             0,
#if OS_TIME_SLICE_EN > 0
                             1,
#endif
                             "VmCache",
                             VM_TASK_STACK_SIZE
                            );
    if(OS_NO_ERR != err)
    {
        printf("TaskVmCache err =%x \n",err);
    }

}




