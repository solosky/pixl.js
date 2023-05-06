#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include <stdint.h>

#include "app_amiibolink.h"

typedef struct {
    bool backlight;
    uint8_t sleep_timeout_sec;
    bool dirty;
    bool auto_gen_amiibo;
    bool auto_gen_amiibolink;
    bool skip_driver_select;
    bool bat_mode;
    ble_amiibolink_ver_t amiibo_link_mode;
    bool hibernate_enabled;
    bool show_mem_usage;
} settings_data_t;

int32_t settings_init();
int32_t settings_save();
settings_data_t *settings_get_data();

#endif