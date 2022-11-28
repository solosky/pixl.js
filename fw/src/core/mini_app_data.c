#include "mini_app_defines.h"
#include "app_status_bar.h"
#include "app_desktop.h"
#include "app_amiibo.h"
#include <stddef.h>

const mini_app_t* mini_app_registry[] = {
    &app_status_bar_info,
    &app_desktop_info,
    &app_amiibo_info
};

const uint32_t mini_app_num = sizeof(mini_app_registry) / sizeof(mini_app_registry[0]);

