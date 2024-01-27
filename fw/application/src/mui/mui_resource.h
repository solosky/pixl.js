
#ifndef MUI_RESOURCE_H
#define MUI_RESOURCE_H

#include <stdint.h>

typedef struct {
    const uint8_t width;
    const uint8_t height;
    const uint8_t *data;
} xbm_t;
extern const xbm_t app_amiibo_database_32x32;
extern const xbm_t app_amiibo_emulator_32x32;
extern const xbm_t app_amiibo_link_32x32;
extern const xbm_t app_ble_transfer_32x32;
extern const xbm_t app_card_emulator_32x32;
extern const xbm_t app_settings_32x32;

#endif
