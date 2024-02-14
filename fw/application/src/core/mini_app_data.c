#include "mini_app_defines.h"
#include "app_status_bar.h"
#include "app_desktop.h"
#include "app_amiibo.h"
#include "app_ble.h"
#include "app_player.h"
#include "app_settings.h"
#include "app_amiibolink.h"
#include "app_amiidb.h"
#include "app_reader.h"
#include "app_chameleon.h"
#include <stddef.h>

const mini_app_t* mini_app_registry[] = {
    &app_status_bar_info,
    &app_desktop_info,
#ifdef APP_LEGLAMIIBO_ENABLE
    &app_amiibo_info,
#endif
    &app_amiidb_info,
    &app_amiibolink_info,
    &app_chameleon_info,
#ifdef RC522
    &app_reader_info,
#endif
#ifdef APP_PLAYER_ENABLE
    &app_player_info,
#endif
#ifdef APP_LEGLAMIIBO_ENABLE
    &app_ble_info, 
#endif
    &app_settings_info
};

const uint32_t mini_app_num = sizeof(mini_app_registry) / sizeof(mini_app_registry[0]);

