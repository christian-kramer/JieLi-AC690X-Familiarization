MEMORY
{
    flash(x)        : ORIGIN = 0x0001000000, LENGTH = 16M
    ram0            : ORIGIN = 0x300000, LENGTH = 104k
    ram1            : ORIGIN = 0x340080, LENGTH = 24k-20-64-128
    zebra_ram       : ORIGIN = 0xf0000+0x5600,    LENGTH = 12k-256
}

ENTRY(_start)

SECTIONS
{
  . = ORIGIN(ram0);
  .data :SUBALIGN(4)
  {
      *(.data*)
  } > ram0

  .bss (NOLOAD) :SUBALIGN(4)
  {
      *(.bss)
      *(.dac_buf_sec)
		bredr_base_addr = .;
      *(.bt_classic_base)
      *(.bt_sco_codec_data)
      *(.bt_classic_data)
  } > ram0

  SDRAM_BEGIN_ADDR = ORIGIN( ram0);
  SRAM_END_ADDR  = . ;
  sram_end  = . - ORIGIN(ram0);

   . = ORIGIN(ram1);

  .data1 :SUBALIGN(4)
  {
       . = ALIGN(4);
       
	   bredr_base_updata_ram1 = .;

	   _bredr_nv_mem_begin = .;
       *(.ram1_data)
       *(.reg_buf)
	   _bredr_nv_mem_end = .;

	   _ble_nv_mem_begin = .;
       *(.bt_nv_data)
	   _ble_nv_mem_end = .;

      *(.eq_buf_sec)
      *(.sd_buf)
      *(.usb_buf)

	  *(.common)

      *memcpy.o(.text .rodata*)
      *memset.o(.text .rodata*)
      *icache_hw.o(.text)
	   _system_cfg_begin = .;
      *(.system_cfg)
      _system_cfg_end = .;

      *(.fm_code)
	  *(.powerdown_text)
      *(.zebra_data)

  } > ram1

  .bss_ram1 (NOLOAD) :SUBALIGN(4)
  {
      *(.stack)
	  *(.ram1_bss)
  } > ram1

  . = ORIGIN(zebra_ram);
  .data3 :SUBALIGN(4)
  {
	  *(.zb_core_ram)
  } > zebra_ram 


  . = ORIGIN(flash) ;
  .text  ALIGN(4):
  {
	  *startup.o(.text)
      *(.const_tab)
        _VM_CODE_START = . ;
        *(.spi0_drv)
        *(.icache_drv)
        *spi_hw.o(.text)
        *winbond_flash.o(.text)
        *vm_common.o(.text)
        *vm_printf.o(.text)
        *(.vm)
        _VM_CODE_END = . ;

	  *(.ver_magic)
	  *(.version)

      *(.text* .rodata*)
         /*  -----蓝牙的链表接口定义一定要确保对齐,其它代码段不要在中间定义-----  */
         /*begin*/
        . = ALIGN(4);
	lmp_hci_handler_begin = .;
		*(.lmp_hci_handler)
	lmp_hci_handler_end = .;

        a2dp_sink_media_probe_begin = .;
		*(.a2dp_sink_media_probe)
	a2dp_sink_media_probe_end = .;

	a2dp_sink_media_codec_begin = .;
		*(.a2dp_sink_media_codec)
	a2dp_sink_media_codec_end = .;

	a2dp_source_media_codec_begin = .;
		*(.a2dp_source_media_codec)
	a2dp_source_media_codec_end = .;

	a2dp_event_handler_begin = .;
		KEEP(*(.a2dp_event_handler))
	a2dp_event_handler_end = .;

        clock_switcher_begin = . ;
        *(.clock_switch.fun)
        clock_switcher_end = . ;

        bt_sleep_begin = .;
          *(.bt_sleep)
        bt_sleep_end = .;

        /*  -----end-----  */
        *(.ble)
		*(.bredr)
		*(.bt_power)
        *(.poweroff_flash)

        ble_text_begin = . ;
        *(.ble_irq_text)
        ble_text_end = . ;

        bredr_text_begin = . ;
        *(.bredr_irq_text)
        bredr_text_end = . ;
	
	    *(.zebra)
        *(.sys_cfg)
        *(.ape_text)
        _task_list_begin = .;
          *(.task_info)
        _task_list_end = .;

        poweroff_text_begin = . ;
        *(.poweroff_text)
        poweroff_text_end = . ;

  } > flash

  text_end = . ;

  .version : {
	  *(.ver_magic_start)
	  *(.version)
	  *(.ver_magic_end)
  } > flash


  data_addr  = ADDR(.data) ;
  data_begin = text_end ;
  data1_size =  SIZEOF(.data) ;

  bss_begin = ADDR(.bss ) ;
  bss_size  = SIZEOF(.bss);


  ram1_addr  = ADDR(.data1) ;
  ram1_begin = text_end + SIZEOF(.data) ;
  ram1_size  = SIZEOF(.data1) ;

  bss_ram1_begin = ADDR(.bss_ram1 ) ;
  bss_ram1_size  = SIZEOF(.bss_ram1);
}
