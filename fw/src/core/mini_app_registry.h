#ifndef MINI_APP_REGISTRY_H
#define MINI_APP_REGISTRY_H

#include "mini_app_defines.h"

#include "app_status_bar.h"

typedef enum { MINI_APP_ID_STATUS_BAR, MINI_APP_ID_DESKTOP } mini_app_id_t;

mini_app_t *mini_app_registry_find_by_id(uint32_t id);
uint8_t mini_app_registry_get_app_num();
mini_app_t *mini_app_registry_get_app_array();

#endif