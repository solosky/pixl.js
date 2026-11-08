#include "amiibolink_scene.h"
#include "app_amiibolink.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "amiibo_helper.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "i18n/language.h"
#include "settings.h"

#define ICON_MODE 0xe135
#define ICON_BACK 0xe069
#define ICON_HOME 0xe1f0
#define ICON_AUTO 0xe11f
#define ICON_PROTO 0xe042
#define ICON_VER 0xe0be

#define AMIIBOLINK_MENU_BACK_EXIT 0
#define AMIIBOLINK_MENU_BACK_MAIN 1
#define AMIIBOLINK_MENU_MODE 2
#define AMIIBOLINK_MENU_VER 3
#define AMIIBOLINK_MENU_AUTO_GENERATE 4

void amiibolink_scene_menu_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                    mui_list_item_t *p_item) {
    app_amiibolink_t *app = p_list_view->user_data;
    int32_t menu_code = (int32_t)p_item->user_data;
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (menu_code == AMIIBOLINK_MENU_BACK_MAIN) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBOLINK_SCENE_MAIN);
        } else if (menu_code == AMIIBOLINK_MENU_MODE) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBOLINK_SCENE_MENU_MODE);
        } else if (menu_code == AMIIBOLINK_MENU_VER) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBOLINK_SCENE_MENU_VER);
        } else if (menu_code == AMIIBOLINK_MENU_BACK_EXIT) {
            mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIBOLINK);
        }
    }
}

void amiibolink_scene_menu_on_enter(void *user_data) {
    app_amiibolink_t *app = user_data;
    const char *mode_name = "";
    if (app->amiibolink_mode == BLE_AMIIBOLINK_MODE_RANDOM) {
        mode_name = getLangString(_L_MODE_RANDOM);
    } else if (app->amiibolink_mode == BLE_AMIIBOLINK_MODE_CYCLE) {
        mode_name = getLangString(_L_MODE_CYCLE);
    } else if (app->amiibolink_mode == BLE_AMIIBOLINK_MODE_NTAG) {
        mode_name = getLangString(_L_MODE_NTAG);
    } else if (app->amiibolink_mode == BLE_AMIIBOLINK_MODE_RANDOM_AUTO_GEN) {
        mode_name = getLangString(_L_MODE_RANDOM_AUTO_GEN);
    }

    string_t sub_text;
    string_init(sub_text);

    string_printf(sub_text, "[%s]", mode_name);
    mui_list_view_add_item_ext(app->p_list_view, ICON_MODE, getLangString(_L_MODE), string_get_cstr(sub_text),
                               (void *)AMIIBOLINK_MENU_MODE);

    settings_data_t *p_settings = settings_get_data();

    const char *version_name = p_settings->amiibo_link_ver == BLE_AMIIBOLINK_VER_V2
                                   ? "V2"
                                   : (p_settings->amiibo_link_ver == BLE_AMIIBOLINK_VER_V1 ? "V1" : "AmiLoop");
    string_printf(sub_text, "[%s]", version_name);
    mui_list_view_add_item_ext(app->p_list_view, ICON_PROTO, getLangString(_L_COMPATIBLE_MODE),
                               string_get_cstr(sub_text),
                               (void *)AMIIBOLINK_MENU_VER);
    string_clear(sub_text);

    mui_list_view_add_item_ext(app->p_list_view, ICON_VER, getLangString(_L_TAG_DETAILS), NULL,
                               (void *)AMIIBOLINK_MENU_BACK_MAIN);

    mui_list_view_add_item(app->p_list_view, ICON_HOME, getLangString(_L_MAIN_MENU), (void *)AMIIBOLINK_MENU_BACK_EXIT);

    mui_list_view_set_selected_cb(app->p_list_view, amiibolink_scene_menu_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_LIST);
}

void amiibolink_scene_menu_on_exit(void *user_data) {
    app_amiibolink_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}
