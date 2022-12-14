#ifndef APP_BLE_H
#define APP_BLE_H

#include "mini_app_defines.h"
#include "ble_status_view.h"

typedef struct {
    ble_status_view_t *p_ble_view;
    mui_view_dispatcher_t *p_view_dispatcher;
} app_ble_t;

extern const mini_app_t app_ble_info;

#endif