#include "ble_scene.h"
#include "app_ble.h"
#include "ble_main.h"
#include "ble_status_view.h"


void ble_scene_connect_start_on_enter(void *user_data) {
    app_ble_t *app = user_data;

    ble_init();

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, BLE_VIEW_ID_MAIN);
}

void ble_scene_connect_start_on_exit(void *user_data) {
    app_ble_t *app = user_data;
    
}