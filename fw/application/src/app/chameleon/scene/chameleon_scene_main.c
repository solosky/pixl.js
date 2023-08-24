#include "chameleon_view.h"
#include "app_chameleon.h"
#include "ble_main.h"
#include "chameleon_scene.h"
#include "vfs.h"

#include "amiibo_helper.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "ntag_emu.h"
#include "ntag_store.h"

#include "nrf_log.h"

#include "settings.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb

#define DEFAULT_NTAG_INDEX 99
#define MAX_NTAG_INDEX 26


static void chameleon_scene_main_event_cb(chameleon_view_event_t event, chameleon_view_t *p_view) {
    app_amiibolink_t *app = p_view->user_data;
    if (event == AMIIBOLINK_VIEW_EVENT_MENU) {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU);
    } else if (event == AMIIBOLINK_VIEW_EVENT_UPDATE) {

    }
}


void chameleon_scene_main_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;
    chameleon_view_set_event_cb(app->p_chameleon_view, chameleon_scene_main_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_MAIN);
}

void chameleon_scene_main_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;

    ble_disable();
    ble_nus_set_handler(NULL, NULL);
    ntag_emu_set_update_cb(NULL, NULL);
}