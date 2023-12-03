#include "amiibolink_scene.h"
#include "app_amiibolink.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "settings.h"
#include "i18n/language.h"

#define ICON_MODE 0xe1ed
#define ICON_BACK 0xe069

void amiibolink_scene_menu_ver_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                         mui_list_item_t *p_item) {
    app_amiibolink_t *app = p_list_view->user_data;
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (p_item->icon == ICON_MODE) {
            settings_data_t *p_settings = settings_get_data();
            p_settings->amiibo_link_ver = (ble_amiibolink_ver_t) p_item->user_data;
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        } else if (p_item->icon == ICON_BACK) {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    }
}

void amiibolink_scene_menu_ver_on_enter(void *user_data) {
    app_amiibolink_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, ICON_MODE, getLangString(_L_AMIIBOLINK_V1), (void *)BLE_AMIIBOLINK_VER_V1);
    mui_list_view_add_item(app->p_list_view, ICON_MODE, getLangString(_L_AMIIBOLINK_V2), (void *)BLE_AMIIBOLINK_VER_V2);
    mui_list_view_add_item(app->p_list_view, ICON_MODE, getLangString(_L_AMILOOP), (void *)BLE_AMILOOP);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, getLangString(_L_MAIN_RETURN), (void *)-1);

    settings_data_t *p_settings = settings_get_data();
    mui_list_view_set_selected_cb(app->p_list_view, amiibolink_scene_menu_ver_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_LIST);
}

void amiibolink_scene_menu_ver_on_exit(void *user_data) {
    app_amiibolink_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}