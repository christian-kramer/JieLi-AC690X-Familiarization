 
 
 
CC_ARG = \
-DNOFLOAT \
-DHAVE_MALLOC \
 

INCLUDES = \
-I../include_lib \
-I../include_lib/cpu \
-I../include_lib/cpu/$(PLATFORM) \
-Iinclude/bluetooth \
-Iinclude/echo \
-Iinclude/fm \
-Iinclude/idle \
-Iinclude/linein \
-Iinclude/main \
-Iinclude/music \
-Iinclude/record \
-Iinclude/rtc \
-Iinclude/usb_device \
-Iinclude/fm/bk1080 \
-Iinclude/fm/fm_inside \
-Iinclude/fm/qn8035 \
-Iinclude/fm/rda5807 \
-Iinclude \
-Iinclude/common \
-Iinclude/cpu \
-Iinclude/cpu/$(PLATFORM) \
-Iinclude/cpu/$(PLATFORM)/os_api \
-Iinclude/cpu/$(PLATFORM)/dac \
-Iinclude/cpu/$(PLATFORM)/play_sel \
-Iinclude/cpu/$(PLATFORM)/rcsp \
-Iinclude/cpu/$(PLATFORM)/ui \
-Iinclude/cpu/$(PLATFORM)/vm \
-Iinclude/cpu/$(PLATFORM)/ui/lcd \
-Iinclude/cpu/$(PLATFORM)/ui/led \
 
 
objs = \
cpu/$(PLATFORM)/board/board.o \
cpu/$(PLATFORM)/uart/uart.o \
cpu/$(PLATFORM)/sys_detect.o \
cpu/$(PLATFORM)/clock.o \
cpu/$(PLATFORM)/update.o \
cpu/$(PLATFORM)/ui/lcd/lcd128X64.o \
cpu/$(PLATFORM)/ui/lcd/lcd_disp.o \
cpu/$(PLATFORM)/ui/lcd/lcd_drv_api.o \
cpu/$(PLATFORM)/ui/lcd/lcd_drv_interface.o \
cpu/$(PLATFORM)/ui/lcd/lcd_spi.o \
cpu/$(PLATFORM)/ui/lcd/lcd_ui_menu.o \
cpu/$(PLATFORM)/ui/led/led7_drv.o \
cpu/$(PLATFORM)/ui/led/led.o \
cpu/$(PLATFORM)/ui/ui_api.o \
cpu/$(PLATFORM)/ui/ui_common.o \
cpu/$(PLATFORM)/dac/dac.o \
cpu/$(PLATFORM)/dac/eq.o \
cpu/$(PLATFORM)/iic/iic.o \
cpu/$(PLATFORM)/iis/iis.o \
cpu/$(PLATFORM)/key_drv/key.o \
cpu/$(PLATFORM)/key_drv/key_drv_ad.o \
cpu/$(PLATFORM)/key_drv/key_drv_io.o \
cpu/$(PLATFORM)/key_drv/key_drv_ir.o \
cpu/$(PLATFORM)/key_drv/key_drv_tch.o \
cpu/$(PLATFORM)/key_drv/key_voice.o \
cpu/$(PLATFORM)/ladc/ladc.o \
cpu/$(PLATFORM)/timer/timer.o \
common/ASCII_lib.o \
common/common.o \
common/dev_manage/dev_ctl.o \
common/file_operate/file_bs_deal.o \
common/file_operate/file_operate.o \
common/os_api/os_api.o \
common/os_api/task_manage.o \
common/play_sel/play_sel.o \
common/vm/vm_api.o \
echo/echo.o \
echo/echo_api.o \
echo/echo_key.o \
echo/echo_prompt.o \
echo/echo_ui.o \
fm/fm_api.o \
fm/fm_common.o \
fm/fm_key.o \
fm/fm_prompt.o \
fm/fm_radio.o \
fm/fm_ui.o \
fm/bk1080/Bk1080.o \
fm/fm_inside/fm_inside.o \
fm/qn8035/QN8035.o \
fm/rda5807/RDA5807.o \
idle/idle.o \
idle/idle_key.o \
linein/dev_linein.o \
linein/linein.o \
linein/linein_key.o \
linein/linein_prompt.o \
linein/linein_ui.o \
main/main.o \
main/main_key.o \
music/music.o \
music/music_info.o \
music/music_key.o \
music/music_prompt.o \
music/music_ui.o \
music/music_decrypt.o \
music/music_ab_rpt.o \
music/lyrics_api.o \
record/rec_key.o \
record/rec_ui.o \
record/record.o \
rtc/rtc.o \
rtc/rtc_key.o \
rtc/rtc_setting.o \
rtc/rtc_ui.o \
usb_device/dev_pc.o \
usb_device/pc_prompt.o \
usb_device/pc_ui.o \
usb_device/usb_device.o \
usb_device/usb_key.o \
bluetooth/aec/aec_api.o \
bluetooth/btstack/bt_configs_user.o \
bluetooth/btstack/bt_key_deal.o \
bluetooth/btstack/bt_key_msg.o \
bluetooth/btstack/bt_ui.o \
bluetooth/btstack/fcc_test.o \
common/rcsp/rcsp_event_deal.o \
common/rcsp/rcsp_task_msg_deal.o \
common/rcsp/rcsp_cbk_msg_deal.o \
common/rcsp/rcsp_protocol_deal.o \
common/rcsp/light_task.o \
common/rcsp/light_control/light_main.o \
common/rcsp/light_control/light_pwm.o \
common/rcsp/light_control/light_fade.o \
bluetooth/btstack/le_server_module.o \
 
obj_ls = \
 
 
obj_bs = \
cpu/$(PLATFORM)/version.o\
 
 

LIBS = \
--start-group ../include_lib/rtos_lib$(SUFFIX).a ../include_lib/mem_lib$(SUFFIX).a --end-group \
-whole-archive ../include_lib/bluetooth_lib$(SUFFIX).a -no-whole-archive \
../include_lib/dac_lib$(SUFFIX).a  \
../include_lib/aec_lib$(SUFFIX).a  \
../include_lib/dec_lib$(SUFFIX).a  \
../include_lib/dev_manage_lib$(SUFFIX).a  \
../include_lib/fat_lib$(SUFFIX).a  \
../include_lib/sydf_lib$(SUFFIX).a  \
../include_lib/rtos_lib$(SUFFIX).a  \
../include_lib/mem_lib$(SUFFIX).a  \
../include_lib/vm_lib$(SUFFIX).a  \
../include_lib/usb_lib$(SUFFIX).a  \
../include_lib/sdmmc_lib$(SUFFIX).a  \
../include_lib/cbuf_lib$(SUFFIX).a  \
../include_lib/encode_lib$(SUFFIX).a  \
../include_lib/rtc_lib$(SUFFIX).a  \
../include_lib/echo_lib$(SUFFIX).a  \
../include_lib/bluetooth_lib$(SUFFIX).a  \
../include_lib/periphy_lib$(SUFFIX).a  \
--start-group \
../include_lib/bluetooth/br16_ccm_aes.a  \
../include_lib/cpu_lib_$(PLATFORM).a  \
../include_lib/common_lib$(SUFFIX).a  \
--end-group \
../include_lib/dec/sbc_dec_lib_for_EngDect.a  \
../include_lib/bluetooth/bt_syn_lib.a  \
../include_lib/bluetooth/bt_tws_lib.a  \
../include_lib/bluetooth/crc_ibm_lib.a  \
../include_lib/usb/lib_usb_syn.a  \
../include_lib/dac/libLimiter.a  \
../include_lib/dac/libSOSIIR.a  \
../include_lib/dec/flac_dec_lib.a  \
../include_lib/dec/wav_dec_lib.a  \
../include_lib/dec/wma_dec_lib.a  \
../include_lib/dec/ape_dec_lib.a  \
../include_lib/dec/mp3_dec_lib.a  \
../include_lib/dec/sbc_dec_lib.a  \
../include_lib/dec/aac_dec_lib.a  \
../include_lib/echo/lib_audio_reverb_H.a \
../include_lib/echo/lib_audio_howlingsupression.a \
../include_lib/encode/lib_adpcm_encode.a \
../include_lib/encode/lib_mp2_encode.a \
../include_lib/encode/mp3_enc_lib.a \
../include_lib/aec/lib_esco_repair.a \
../include_lib/aec/lib_esco_sync.a \
../include_lib/aec/libBtCalling.a \
../include_lib/aec/libDAGC.a \
../include_lib/aec/libSpeech_dection.a \
../include_lib/rcsp_lib$(SUFFIX).a  \
../include_lib/font_lib$(SUFFIX).a \
../include_lib/uicon_lib$(SUFFIX).a \


GEN_LIB=n
-include $(MAKE_RULE)
 
 
