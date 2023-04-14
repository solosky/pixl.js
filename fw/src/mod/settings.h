#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool backlight;
    uint8_t sleep_timeout_sec;
    bool dirty;
    bool auto_gen_amiibo;
    bool auto_gen_amiibolink;
} settings_data_t;

int32_t settings_init();
int32_t settings_save();
settings_data_t *settings_get_data();

#endif