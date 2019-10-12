#ifndef __FLASH_CFG_H__
#define __FLASH_CFG_H__

#include "typedef.h"
typedef u32 UINT;
struct flash_cfg
{
    UINT flash_id;
    //flash容量
    UINT flash_size;
    //代码、数据、配置区的总大小
    UINT flash_file_size;
    UINT sdfile_head_addr;
    //[1:0]data width 0--1wire_1bit 1--2wire_1bit 2--2wire_2bit 3/4--4wire_4bit
    //[2] SPI_IS_CONTINUE_READ
    //[3] SPI_IS_OUTPUT
    //[4] SPI_NWIRE_SEND_CMD
    //[8:5] SPI_CS_DESELECT
    UINT spi_run_mode;
    UINT spi_div;
    UINT flash_base;
    //低16有效，对应flash reg0和reg1
    UINT protected_arg;
    //配置区起始地址
    UINT cfg_zone_addr;
    //配置区大小
    UINT cfg_zone_size;
};
typedef struct _APP_USE_FLASH_SYS_CFG
{
    UINT cfg_zone_addr;
    UINT flash_size;
    UINT cfg_zone_size;
    UINT btif_addr;
    UINT btif_len;
    UINT vmif_addr;
    UINT vmif_len;
}APP_USE_FLASH_SYS_CFG;
struct sdr_cfg
{
    UINT sdram_refresh_cycle;
    UINT sdram_refresh_time;
    UINT sdram_cas_latency;
    UINT sdram_trfc;
    UINT sdram_trp;
    UINT sdram_trcd;
    UINT sdram_colum;
    UINT sdram_clk_hd_en;
    UINT sdram_trrd;
    UINT sdram_clkoe;
    UINT sdram_size;
    UINT sdram_fast_clock_rate;
};
struct clk_cfg
{
    UINT pll_sel;
    UINT osc_freq;
    UINT osc_src;
    UINT osc_hc_en;
    UINT osc_1pin_en;
};

typedef struct _FLASH_SYS_CFG
{
    struct flash_cfg flash_cfg;
//    struct sdr_cfg sdr_cfg;
    struct clk_cfg clk_cfg;
}FLASH_SYS_CFG;

extern APP_USE_FLASH_SYS_CFG app_use_flash_cfg;

#endif/*__FLASH_CFG_H__*/
