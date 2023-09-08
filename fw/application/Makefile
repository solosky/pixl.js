PROJECT_NAME     := ble_app_blinky_pca10040_s132
TARGETS          := pixljs
OUTPUT_DIRECTORY := ../_build

SDK_ROOT := $(NRF52_SDK_ROOT)
PROJ_DIR := ./src
COMP_DIR := ../components
U8G2_ROOT := ../components/u8g2/csrc
CHAMELEON_ROOT :=$(COMP_DIR)/chameleon-ultra/firmware

APP_VERSION ?= 1
RELEASE ?= 1
BOARD ?= LCD

$(OUTPUT_DIRECTORY)/pixljs.out: \
  LINKER_SCRIPT  := ../ld/application.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer2.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/timer/drv_rtc.c \
  $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/pwm/app_pwm.c \
  $(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_timer.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_nfct.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spim.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_wdt.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_ppi.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_ppi.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_spi.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp_nfc.c \
  $(SDK_ROOT)/components/libraries/mem_manager/mem_manager.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
  $(SDK_ROOT)/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus/ble_nus.c \
  $(SDK_ROOT)/external/utf_converter/utf.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_saadc.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hmac.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aes.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_error.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_hmac.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_aes.c \
  $(SDK_ROOT)/external/mbedtls/library/aes.c \
	$(SDK_ROOT)/external/mbedtls/library/aesni.c \
	$(SDK_ROOT)/external/mbedtls/library/arc4.c \
	$(SDK_ROOT)/external/mbedtls/library/aria.c \
	$(SDK_ROOT)/external/mbedtls/library/asn1parse.c \
	$(SDK_ROOT)/external/mbedtls/library/asn1write.c \
	$(SDK_ROOT)/external/mbedtls/library/base64.c \
	$(SDK_ROOT)/external/mbedtls/library/bignum.c \
	$(SDK_ROOT)/external/mbedtls/library/blowfish.c \
	$(SDK_ROOT)/external/mbedtls/library/camellia.c \
	$(SDK_ROOT)/external/mbedtls/library/ccm.c \
	$(SDK_ROOT)/external/mbedtls/library/certs.c \
	$(SDK_ROOT)/external/mbedtls/library/chacha20.c \
	$(SDK_ROOT)/external/mbedtls/library/chachapoly.c \
	$(SDK_ROOT)/external/mbedtls/library/cipher.c \
	$(SDK_ROOT)/external/mbedtls/library/cipher_wrap.c \
	$(SDK_ROOT)/external/mbedtls/library/cmac.c \
	$(SDK_ROOT)/external/mbedtls/library/ctr_drbg.c \
	$(SDK_ROOT)/external/mbedtls/library/debug.c \
	$(SDK_ROOT)/external/mbedtls/library/des.c \
	$(SDK_ROOT)/external/mbedtls/library/dhm.c \
	$(SDK_ROOT)/external/mbedtls/library/ecdh.c \
	$(SDK_ROOT)/external/mbedtls/library/ecdsa.c \
	$(SDK_ROOT)/external/mbedtls/library/ecjpake.c \
	$(SDK_ROOT)/external/mbedtls/library/ecp.c \
	$(SDK_ROOT)/external/mbedtls/library/ecp_curves.c \
	$(SDK_ROOT)/external/mbedtls/library/entropy.c \
	$(SDK_ROOT)/external/mbedtls/library/entropy_poll.c \
	$(SDK_ROOT)/external/mbedtls/library/error.c \
	$(SDK_ROOT)/external/mbedtls/library/gcm.c \
	$(SDK_ROOT)/external/mbedtls/library/havege.c \
	$(SDK_ROOT)/external/mbedtls/library/hmac_drbg.c \
	$(SDK_ROOT)/external/mbedtls/library/md.c \
	$(SDK_ROOT)/external/mbedtls/library/md2.c \
	$(SDK_ROOT)/external/mbedtls/library/md4.c \
	$(SDK_ROOT)/external/mbedtls/library/md5.c \
	$(SDK_ROOT)/external/mbedtls/library/md_wrap.c \
	$(SDK_ROOT)/external/mbedtls/library/memory_buffer_alloc.c \
	$(SDK_ROOT)/external/mbedtls/library/net_sockets.c \
	$(SDK_ROOT)/external/mbedtls/library/nist_kw.c \
	$(SDK_ROOT)/external/mbedtls/library/oid.c \
	$(SDK_ROOT)/external/mbedtls/library/padlock.c \
	$(SDK_ROOT)/external/mbedtls/library/pem.c \
	$(SDK_ROOT)/external/mbedtls/library/pk.c \
	$(SDK_ROOT)/external/mbedtls/library/pk_wrap.c \
	$(SDK_ROOT)/external/mbedtls/library/pkcs11.c \
	$(SDK_ROOT)/external/mbedtls/library/pkcs12.c \
	$(SDK_ROOT)/external/mbedtls/library/pkcs5.c \
	$(SDK_ROOT)/external/mbedtls/library/pkparse.c \
	$(SDK_ROOT)/external/mbedtls/library/pkwrite.c \
	$(SDK_ROOT)/external/mbedtls/library/platform.c \
	$(SDK_ROOT)/external/mbedtls/library/platform_util.c \
	$(SDK_ROOT)/external/mbedtls/library/poly1305.c \
	$(SDK_ROOT)/external/mbedtls/library/ripemd160.c \
	$(SDK_ROOT)/external/mbedtls/library/rsa.c \
	$(SDK_ROOT)/external/mbedtls/library/rsa_internal.c \
	$(SDK_ROOT)/external/mbedtls/library/sha1.c \
	$(SDK_ROOT)/external/mbedtls/library/sha256.c \
	$(SDK_ROOT)/external/mbedtls/library/sha512.c \
	$(SDK_ROOT)/external/mbedtls/library/ssl_cache.c \
	$(SDK_ROOT)/external/mbedtls/library/ssl_ciphersuites.c \
	$(SDK_ROOT)/external/mbedtls/library/ssl_cli.c \
	$(SDK_ROOT)/external/mbedtls/library/ssl_cookie.c \
	$(SDK_ROOT)/external/mbedtls/library/ssl_srv.c \
	$(SDK_ROOT)/external/mbedtls/library/ssl_ticket.c \
	$(SDK_ROOT)/external/mbedtls/library/ssl_tls.c \
	$(SDK_ROOT)/external/mbedtls/library/threading.c \
	$(SDK_ROOT)/external/mbedtls/library/version.c \
	$(SDK_ROOT)/external/mbedtls/library/version_features.c \
	$(SDK_ROOT)/external/mbedtls/library/x509.c \
	$(SDK_ROOT)/external/mbedtls/library/x509_create.c \
	$(SDK_ROOT)/external/mbedtls/library/x509_crl.c \
	$(SDK_ROOT)/external/mbedtls/library/x509_crt.c \
	$(SDK_ROOT)/external/mbedtls/library/x509_csr.c \
	$(SDK_ROOT)/external/mbedtls/library/x509write_crt.c \
	$(SDK_ROOT)/external/mbedtls/library/x509write_csr.c \
	$(SDK_ROOT)/external/mbedtls/library/xtea.c \
  $(PROJ_DIR)/main.c \
  $(PROJ_DIR)/ntag/ntag_emu_v2.c \
  $(PROJ_DIR)/ntag/ntag_store.c \
  $(PROJ_DIR)/bat.c \
  $(PROJ_DIR)/utils.c \
  $(PROJ_DIR)/version2.c \
  $(PROJ_DIR)/amiibo_helper.c \
  $(PROJ_DIR)/app_error_handler.c \
  $(PROJ_DIR)/hal/hal_spi_bus.c \
  $(PROJ_DIR)/hal/hal_spi_flash.c \
  $(PROJ_DIR)/hal/hal_internal_flash.c \
  $(PROJ_DIR)/mod/bsp_btn.c \
  $(PROJ_DIR)/mod/settings.c \
  $(PROJ_DIR)/mod/cache.c \
  $(PROJ_DIR)/mod/ble/ble_main.c \
  $(PROJ_DIR)/mod/ble/ble_df_driver.c \
  $(PROJ_DIR)/mod/ble/ble_amiibolink.c \
  $(PROJ_DIR)/mod/cwalk2.c \
  $(PROJ_DIR)/mod/vfs/vfs.c \
  $(PROJ_DIR)/mod/vfs/vfs_meta.c \
  $(PROJ_DIR)/mod/vfs/vfs_driver_spiffs.c \
  $(PROJ_DIR)/mod/vfs/vfs_driver_lfs.c \
  $(PROJ_DIR)/mod/df/df_core.c \
  $(PROJ_DIR)/mod/df/df_proto_info.c \
  $(PROJ_DIR)/mod/df/df_proto_vfs.c \
  $(PROJ_DIR)/mod/df/df_core.c \
  $(PROJ_DIR)/mui/mui_view.c \
  $(PROJ_DIR)/mui/mui_view_port.c \
  $(PROJ_DIR)/mui/mui_view_dispatcher.c \
  $(PROJ_DIR)/mui/mui_scene_dispatcher.c \
  $(PROJ_DIR)/mui/mui_u8g2.c \
  $(PROJ_DIR)/mui/u8g2_font_siji_t_6x10.c \
  $(PROJ_DIR)/mui/u8g2_font_wqy12_t_gb2312a.c \
  $(PROJ_DIR)/mui/u8g2_font_wqy12_t_gb2312a_lite.c \
  $(PROJ_DIR)/mui/mui_core.c \
  $(PROJ_DIR)/mui/mui_canvas.c \
  $(PROJ_DIR)/mui/mui_event.c \
  $(PROJ_DIR)/mui/mui_input.c \
  $(PROJ_DIR)/mui/mui_element.c \
  $(PROJ_DIR)/mui/mui_mem.c \
  $(PROJ_DIR)/mui/mui_anim.c \
  $(PROJ_DIR)/mui/mui_math.c \
  $(PROJ_DIR)/mui/view/mui_list_view.c \
  $(PROJ_DIR)/mui/view/mui_text_input.c \
  $(PROJ_DIR)/mui/view/mui_msg_box.c \
  $(PROJ_DIR)/mui/view/mui_progress_bar.c \
  $(PROJ_DIR)/core/mini_app_launcher.c \
  $(PROJ_DIR)/core/mini_app_registry.c \
  $(PROJ_DIR)/core/mini_app_data.c \
  $(PROJ_DIR)/app/status_bar/app_status_bar.c \
  $(PROJ_DIR)/app/status_bar/view/status_bar_view.c \
  $(PROJ_DIR)/app/ble/app_ble.c \
  $(PROJ_DIR)/app/ble/view/ble_status_view.c \
  $(PROJ_DIR)/app/ble/scene/ble_scene.c \
  $(PROJ_DIR)/app/ble/scene/ble_scene_connect_start.c \
  $(PROJ_DIR)/app/player/app_player.c \
  $(PROJ_DIR)/app/player/view/player_view.c \
  $(PROJ_DIR)/app/player/scene/player_scene.c \
  $(PROJ_DIR)/app/player/scene/player_scene_file_browser.c \
  $(PROJ_DIR)/app/player/scene/player_scene_play.c \
  $(PROJ_DIR)/app/amiibolink/app_amiibolink.c \
  $(PROJ_DIR)/app/amiibolink/view/amiibolink_view.c \
  $(PROJ_DIR)/app/amiibolink/scene/amiibolink_scene.c \
  $(PROJ_DIR)/app/amiibolink/scene/amiibolink_scene_main.c \
  $(PROJ_DIR)/app/amiibolink/scene/amiibolink_scene_menu.c \
  $(PROJ_DIR)/app/amiibolink/scene/amiibolink_scene_menu_mode.c \
  $(PROJ_DIR)/app/amiibolink/scene/amiibolink_scene_menu_ver.c \
  $(PROJ_DIR)/app/amiidb/app_amiidb.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_main.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_game_list.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_amiibo_detail.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_amiibo_detail_menu.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_amiibo_search.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_fav_list.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_fav_select.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_fav_list_menu.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_data_list.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_data_select.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_data_list_menu.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_settings.c \
  $(PROJ_DIR)/app/amiidb/scene/amiidb_scene_settings_slot_num.c \
  $(PROJ_DIR)/app/amiidb/api/amiidb_api_slot.c \
  $(PROJ_DIR)/app/amiidb/api/amiidb_api_fav.c \
  $(PROJ_DIR)/app/amiidb/api/amiidb_api_db.c \
  $(PROJ_DIR)/app/amiidb/view/amiibo_view.c \
  $(PROJ_DIR)/app/chameleon/app_chameleon.c \
  $(PROJ_DIR)/app/chameleon/view/chameleon_view.c \
  $(PROJ_DIR)/app/chameleon/scene/chameleon_scene_main.c \
  $(PROJ_DIR)/app/chameleon/scene/chameleon_scene_menu.c \
  $(PROJ_DIR)/app/chameleon/scene/chameleon_scene.c \
  $(PROJ_DIR)/app/chameleon/port/fds_utils.c \
  $(PROJ_DIR)/app/desktop/app_desktop.c \
  $(PROJ_DIR)/app/amiibo/app_amiibo.c \
  $(PROJ_DIR)/app/amiibo/view/amiibo_detail_view.c \
  $(PROJ_DIR)/app/amiibo/scene/amiibo_scene.c \
  $(PROJ_DIR)/app/amiibo/scene/amiibo_scene_storage_list.c \
  $(PROJ_DIR)/app/amiibo/scene/amiibo_scene_storage_list_menu.c \
  $(PROJ_DIR)/app/amiibo/scene/amiibo_scene_amiibo_detail.c \
  $(PROJ_DIR)/app/amiibo/scene/amiibo_scene_amiibo_detail_menu.c \
  $(PROJ_DIR)/app/amiibo/scene/amiibo_scene_file_browser.c \
  $(PROJ_DIR)/app/amiibo/scene/amiibo_scene_file_browser_menu.c \
  $(PROJ_DIR)/app/settings/app_settings.c \
  $(PROJ_DIR)/app/settings/scene/settings_scene.c \
  $(PROJ_DIR)/app/settings/scene/settings_scene_main.c \
  $(PROJ_DIR)/app/settings/scene/settings_scene_version.c \
  $(PROJ_DIR)/app/settings/scene/settings_scene_sleep_timeout.c \
  $(PROJ_DIR)/app/settings/scene/settings_scene_lcd_backlight.c \
  $(PROJ_DIR)/app/settings/scene/settings_scene_oled_contrast.c \
  $(PROJ_DIR)/app/settings/scene/settings_scene_language.c \
  $(PROJ_DIR)/i18n/en_US.c \
  $(PROJ_DIR)/i18n/zh_Hans.c \
  $(PROJ_DIR)/i18n/zh_TW.c \
  $(PROJ_DIR)/i18n/language.c \
  $(PROJ_DIR)/hal/hal_nfc_t2t.c \
  $(PROJ_DIR)/amiidb/db_amiibo.c \
  $(PROJ_DIR)/amiidb/db_game.c \
  $(PROJ_DIR)/amiidb/db_link.c \
  $(PROJ_DIR)/amiidb/db_search.c \
  $(PROJ_DIR)/sdk/nrfx/nrfx_nfct.c \
  $(PROJ_DIR)/sdk/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \
  $(SDK_ROOT)/components/nfc/platform/nfc_platform.c \
  $(COMP_DIR)/amiitool/src/amiibo.c \
  $(COMP_DIR)/amiitool/src/drbg.c \
  $(COMP_DIR)/amiitool/src/keygen.c \
  $(COMP_DIR)/amiitool/src/version.c \
  $(COMP_DIR)/tlsf/tlsf.c \
  $(COMP_DIR)/spiffs/src/spiffs_cache.c \
  $(COMP_DIR)/spiffs/src/spiffs_check.c \
  $(COMP_DIR)/spiffs/src/spiffs_gc.c \
  $(COMP_DIR)/spiffs/src/spiffs_hydrogen.c \
  $(COMP_DIR)/spiffs/src/spiffs_nucleus.c \
  $(CHAMELEON_ROOT)/application/src/rfid/nfctag/hf/crypto1_helper.c \
  $(CHAMELEON_ROOT)/application/src/rfid/nfctag/hf/nfc_14a.c \
  $(CHAMELEON_ROOT)/application/src/rfid/nfctag/hf/nfc_mf1.c \
  $(CHAMELEON_ROOT)/application/src/rfid/nfctag/hf/nfc_ntag.c \
  $(CHAMELEON_ROOT)/application/src/rfid/crc_utils.c \
  $(CHAMELEON_ROOT)/application/src/rfid/hex_utils.c \
  $(CHAMELEON_ROOT)/application/src/rfid/mf1_crapto1.c \
  $(CHAMELEON_ROOT)/application/src/rfid/mf1_crypto1.c \
  $(CHAMELEON_ROOT)/application/src/rfid/parity.c \
  $(CHAMELEON_ROOT)/application/src/rfid/nfctag/tag_emulation.c \
  $(CHAMELEON_ROOT)/application/src/rfid/nfctag/tag_persistence.c \
  $(COMP_DIR)/littlefs/lfs.c \
  $(COMP_DIR)/littlefs/lfs_util.c \
  $(COMP_DIR)/cwalk/src/cwalk.c \
  $(U8G2_ROOT)/u8g2_bitmap.c \
  $(U8G2_ROOT)/u8g2_box.c \
  $(U8G2_ROOT)/u8g2_buffer.c \
  $(U8G2_ROOT)/u8g2_circle.c \
  $(U8G2_ROOT)/u8g2_cleardisplay.c \
  $(U8G2_ROOT)/u8g2_d_memory.c \
  $(U8G2_ROOT)/u8g2_d_setup.c \
  $(U8G2_ROOT)/u8g2_font.c \
  $(U8G2_ROOT)/u8g2_hvline.c \
  $(U8G2_ROOT)/u8g2_input_value.c \
  $(U8G2_ROOT)/u8g2_intersection.c \
  $(U8G2_ROOT)/u8g2_kerning.c \
  $(U8G2_ROOT)/u8g2_line.c \
  $(U8G2_ROOT)/u8g2_ll_hvline.c \
  $(U8G2_ROOT)/u8g2_message.c \
  $(U8G2_ROOT)/u8g2_polygon.c \
  $(U8G2_ROOT)/u8g2_selection_list.c \
  $(U8G2_ROOT)/u8g2_setup.c \
  $(U8G2_ROOT)/u8log.c \
  $(U8G2_ROOT)/u8log_u8g2.c \
  $(U8G2_ROOT)/u8log_u8x8.c \
  $(U8G2_ROOT)/u8x8_8x8.c \
  $(U8G2_ROOT)/u8x8_byte.c \
  $(U8G2_ROOT)/u8x8_cad.c \
  $(U8G2_ROOT)/u8x8_capture.c \
  $(U8G2_ROOT)/u8x8_d_st7567.c \
  $(U8G2_ROOT)/u8x8_debounce.c \
  $(U8G2_ROOT)/u8x8_display.c \
  $(U8G2_ROOT)/u8x8_fonts.c \
  $(U8G2_ROOT)/u8x8_gpio.c \
  $(U8G2_ROOT)/u8x8_input_value.c \
  $(U8G2_ROOT)/u8x8_message.c \
  $(U8G2_ROOT)/u8x8_selection_list.c \
  $(U8G2_ROOT)/u8x8_setup.c \
  $(U8G2_ROOT)/u8x8_string.c \
  $(U8G2_ROOT)/u8x8_u16toa.c \
  $(U8G2_ROOT)/u8x8_u8toa.c \
  $(U8G2_ROOT)/u8x8_d_ssd1306_128x64_noname.c

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components/nfc/ndef/generic/message \
  $(SDK_ROOT)/components/nfc/t2t_lib \
  $(SDK_ROOT)/components/nfc/t4t_parser/hl_detection_procedure \
  $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias_c \
  $(SDK_ROOT)/components/libraries/pwm \
  $(SDK_ROOT)/components/softdevice/s112/headers/nrf52 \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/generic \
  $(SDK_ROOT)/components/libraries/usbd/class/msc \
  $(SDK_ROOT)/components/libraries/usbd/class/hid \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/le_oob_rec_parser \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/ble/ble_link_ctx_manager \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/components/nfc/ndef/text \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/gpiote \
  $(SDK_ROOT)/components/libraries/bootloader/ble_dfu \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/common \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/nfc/ndef/generic/record \
  $(SDK_ROOT)/components/nfc/t4t_parser/cc_file \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/external/utf_converter \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas_c \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/components/libraries/experimental_task_manager \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs_c \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/le_oob_rec \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/ble_services/ble_lls \
  $(SDK_ROOT)/components/nfc/platform \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/ac_rec \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/libraries/mpu \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/softdevice/s112/headers \
  $(SDK_ROOT)/components/ble/ble_services/ble_ans_c \
  $(SDK_ROOT)/components/libraries/slip \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/libraries/csense_drv \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus_c \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/mouse \
  $(SDK_ROOT)/components/libraries/low_power_pwm \
  $(SDK_ROOT)/components/libraries/bootloader \
  $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/ble_oob_advdata_parser \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/components/libraries/svc \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/cli \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hts \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/nfc/t4t_parser/apdu \
  $(SDK_ROOT)/components/libraries/util \
  ./config \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc \
  $(SDK_ROOT)/components/libraries/csense \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/ecc \
  $(SDK_ROOT)/components/libraries/hardfault \
  $(SDK_ROOT)/components/ble/ble_services/ble_cscs \
  $(SDK_ROOT)/components/libraries/hci \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/components/nfc/t4t_parser/tlv \
  $(SDK_ROOT)/components/libraries/sortlist \
  $(SDK_ROOT)/components/libraries/spi_mngr \
  $(SDK_ROOT)/components/libraries/led_softblink \
  $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser \
  $(SDK_ROOT)/components/libraries/sdcard \
  $(SDK_ROOT)/components/nfc/ndef/parser/record \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/components/ble/ble_services/ble_cts_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/components/libraries/twi_mngr \
  $(SDK_ROOT)/components/ble/ble_services/ble_hids \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_oob_advdata \
  $(SDK_ROOT)/components/nfc/t2t_parser \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_pair_msg \
  $(SDK_ROOT)/components/libraries/usbd/class/audio \
  $(SDK_ROOT)/components/nfc/t4t_lib \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/components/ble/ble_services/ble_tps \
  $(SDK_ROOT)/components/nfc/ndef/parser/message \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/nfc/ndef/uri \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/libraries/gfx \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/components/libraries/twi_sensor \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/kbd \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/ep_oob_rec \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_pair_lib \
  $(SDK_ROOT)/components/libraries/crypto \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310 \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon \
  $(SDK_ROOT)/components/libraries/crypto/backend/optiga \
  $(SDK_ROOT)/components/libraries/crypto/backend/cifra \
  $(SDK_ROOT)/components/ble/ble_racp \
  $(SDK_ROOT)/components/nfc/ndef/launchapp \
  $(SDK_ROOT)/components/libraries/atomic_flags \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/hs_rec \
  $(SDK_ROOT)/components/libraries/usbd \
  $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/ac_rec_parser \
  $(SDK_ROOT)/components/libraries/stack_guard \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/external/mbedtls/include \
  $(PROJ_DIR) \
  $(PROJ_DIR)/hal \
  $(PROJ_DIR)/mui \
  $(PROJ_DIR)/mui/view \
  $(PROJ_DIR)/core \
  $(PROJ_DIR)/ntag \
  $(PROJ_DIR)/mod \
  $(PROJ_DIR)/mod/vfs \
  $(PROJ_DIR)/mod/ble \
  $(PROJ_DIR)/mod/df \
  $(PROJ_DIR)/amiidb \
  $(PROJ_DIR)/app/status_bar \
  $(PROJ_DIR)/app/status_bar/view \
  $(PROJ_DIR)/app/ble \
  $(PROJ_DIR)/app/ble/scene \
  $(PROJ_DIR)/app/ble/view \
  $(PROJ_DIR)/app/desktop \
  $(PROJ_DIR)/app/settings \
  $(PROJ_DIR)/app/settings/scene \
  $(PROJ_DIR)/app/amiibo \
  $(PROJ_DIR)/app/amiibo/view \
  $(PROJ_DIR)/app/amiibo/scene \
  $(PROJ_DIR)/app/player \
  $(PROJ_DIR)/app/player/view \
  $(PROJ_DIR)/app/player/scene \
  $(PROJ_DIR)/app/amiibolink \
  $(PROJ_DIR)/app/amiibolink/view \
  $(PROJ_DIR)/app/amiibolink/scene \
  $(PROJ_DIR)/app/amiidb \
  $(PROJ_DIR)/app/amiidb/scene \
  $(PROJ_DIR)/app/amiidb/view \
  $(PROJ_DIR)/app/amiidb/api \
  $(PROJ_DIR)/app/chameleon \
  $(PROJ_DIR)/app/chameleon/view \
  $(PROJ_DIR)/app/chameleon/scene \
  $(PROJ_DIR)/sdk/pwr_mgmt \
  $(COMP_DIR)/amiitool/include \
  $(COMP_DIR)/mlib \
  $(COMP_DIR)/u8g2/csrc \
  $(COMP_DIR)/spiffs/src \
  $(COMP_DIR)/littlefs \
  $(COMP_DIR)/tlsf \
  $(COMP_DIR)/cwalk/include \
  $(SDK_ROOT)/components/libraries/bsp \
  $(CHAMELEON_ROOT)/application/src/rfid \
  $(CHAMELEON_ROOT)/application/src/rfid/nfctag \
  $(CHAMELEON_ROOT)/application/src/rfid/nfctag/hf \
  $(CHAMELEON_ROOT)/application/src/utils \
  $(CHAMELEON_ROOT)/application/src \
  $(CHAMELEON_ROOT)/application/src/bsp


# Libraries common to all targets
LIB_FILES += \
  $(SDK_ROOT)/components/nfc/t2t_lib/nfc_t2t_lib_gcc.a \
# Optimization flags
ifneq ($(RELEASE), 1)
OPT = -O2 -g3
else
OPT = -Os -ffunction-sections -fdata-sections
endif
# Uncomment the line below to enable link time optimization
# OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
#CFLAGS += -DBOARD_PCA10040
CFLAGS += -DCUSTOM_BOARD_INC=boards_defines
#CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF52
CFLAGS += -DNRF52832
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DNRF_SD_BLE_API_VERSION=7
CFLAGS += -DS112
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DLV_LVGL_H_INCLUDE_SIMPLE 
CFLAGS += -DBOARD_$(BOARD)
ifneq ($(RELEASE), 1)
CFLAGS += -DDEBUG
CFLAGS += -DEBUG_NRF
else
CFLAGS += -DNDEBUG
CFLAGS += -DU8G2_USE_LARGE_GB2312_FONT
endif
CFLAGS += -DMBEDTLS_CONFIG_FILE=\"nrf_crypto_mbedtls_config.h\"
CFLAGS += -DM_USE_THREAD_BACKEND=0
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
#CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)
# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DAPP_TIMER_V2
ASMFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
# ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DCUSTOM_BOARD_INC=am_board
# ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52832
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=7
ASMFLAGS += -DS112
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DM_USE_THREAD_BACKEND=0

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs
LDFLAGS += -u _printf_float

pixljs: CFLAGS += -D__HEAP_SIZE=8192
pixljs: CFLAGS += -D__STACK_SIZE=8192
pixljs: ASMFLAGS += -D__HEAP_SIZE=8192
pixljs: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: version pixljs
all: version pixljs

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		pixljs
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_softdevice erase

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/pixljs.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/pixljs.hex --sectorerase --log
	nrfjprog -f nrf52 --reset --log

# Flash softdevice
flash_softdevice:
	@echo Flashing: s112_nrf52_7.2.0_softdevice.hex
	nrfjprog -f nrf52 --program $(SDK_ROOT)/components/softdevice/s112/hex/s112_nrf52_7.2.0_softdevice.hex --sectorerase --log
	nrfjprog -f nrf52 --reset --log

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ./config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)

version:
	@cd ../ && python3 scripts/version_gen.py

settingsgen:
	nrfutil settings generate --family NRF52 --application $(OUTPUT_DIRECTORY)/pixljs.hex --application-version $(APP_VERSION) --bootloader-version 1 --bl-settings-version 1 $(OUTPUT_DIRECTORY)/settings.hex

ota: 
	nrfutil pkg generate --application $(OUTPUT_DIRECTORY)/pixljs.hex --application-version $(APP_VERSION) --hw-version 52 --sd-req 0x0103 --key-file ../bootloader/priv.pem $(OUTPUT_DIRECTORY)/pixjs_ota_v$(APP_VERSION).zip

full: settingsgen
	mergehex --merge $(OUTPUT_DIRECTORY)/settings.hex $(OUTPUT_DIRECTORY)/bootloader.hex $(SDK_ROOT)/components/softdevice/s112/hex/s112_nrf52_7.2.0_softdevice.hex $(OUTPUT_DIRECTORY)/pixljs.hex --output $(OUTPUT_DIRECTORY)/pixljs_all.hex
