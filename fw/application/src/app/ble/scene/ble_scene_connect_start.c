#include "ble_scene.h"
#include "app_ble.h"
#include "ble_main2.h"
#include "ble_status_view.h"
#include "ble_df_driver.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"


void ble_scene_connect_status_view_event_cb(ble_status_view_event_t event, ble_status_view_t *p_view){
    app_ble_t *app = p_view->user_data;
    if (event == BLE_STATUS_VIEW_BLE_DISABLE) {
        ble_disable();
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_BLE);
    } else if (event == BLE_STATUS_VIEW_EVENT_UPDATE) {
        // NOOP
        ble_status_view_set_page(app->p_ble_view, ble_status_view_get_page(app->p_ble_view));
    }
}


void ble_scene_connect_start_on_enter(void *user_data) {
    app_ble_t *app = user_data;

    ble_init();
    ble_device_mode_prepare(BLE_DEVICE_MODE_PIXLJS);
    ble_adv_start();

    ble_nus_set_handler(ble_on_received_data, ble_on_transmit_ready);

    ble_get_addr_str(ble_status_view_get_ble_addr(app->p_ble_view));
    ble_status_view_set_event_cb(app->p_ble_view, ble_scene_connect_status_view_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, BLE_VIEW_ID_MAIN);
}

void ble_scene_connect_start_on_exit(void *user_data) {

    app_ble_t *app = user_data;
    
}