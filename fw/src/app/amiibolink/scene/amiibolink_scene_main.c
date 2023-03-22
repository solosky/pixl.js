#include "amiibolink_scene.h"
#include "app_amiibolink.h"
#include "amiibolink_view.h"
#include "vfs.h"
#include "vfs_meta.h"
#include "ble_main.h"
#include "ble_amiibolink.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb

static void amiibolink_scene_main_event_cb(amiibolink_view_event_t event, amiibolink_view_t *p_view) {
    app_amiibolink_t *app = p_view->user_data;
    mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBOLINK_SCENE_MENU);
}

static void amiibolink_scene_ble_event_handler(ble_amiibolink_event_type_t event_type, uint8_t * data, size_t size){
    if(event_type == BLE_AMIIBOLINK_EVENT_TAG_UPDATED){
        mui_update(mui());
    }
}

void amiibolink_scene_main_on_enter(void *user_data) {
    app_amiibolink_t *app = user_data;

    //enable ble
    ble_init();
    ble_set_device_name("amiibolink");
    ble_nus_set_handler(ble_amiibolink_received_data, NULL);

    ble_amiibolink_init();
    ble_amiibolink_set_event_handler(amiibolink_scene_ble_event_handler);


    amiibolink_view_set_event_cb(app->p_amiibolink_view, amiibolink_scene_main_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_MAIN);
}

void amiibolink_scene_main_on_exit(void *user_data) { app_amiibolink_t *app = user_data;

    ble_disable();
    ble_nus_set_handler(NULL, NULL);

}