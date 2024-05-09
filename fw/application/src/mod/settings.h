#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include <stdint.h>

#include "app_amiibolink.h"
#include "i18n/language.h"

typedef struct {
    bool backlight; //deprecated, keep for capability issue
    uint8_t sleep_timeout_sec;
    bool dirty;
    bool auto_gen_amiibo;
    bool auto_gen_amiibolink; //deprecated, keep for capability issue
    bool skip_driver_select;
    bool bat_mode;
    ble_amiibolink_ver_t amiibo_link_ver;
    bool hibernate_enabled;
    bool show_mem_usage;
    uint8_t lcd_backlight;
    uint8_t oled_contrast;
    bool anim_enabled;
    bool qrcode_enabled;
    Language language;
    uint8_t amiidb_data_slot_num;
} settings_data_t;

int32_t settings_init();
int32_t settings_save();
int32_t settings_reset();
settings_data_t *settings_get_data();

#endif