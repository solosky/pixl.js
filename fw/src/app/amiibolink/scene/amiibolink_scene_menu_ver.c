#include "amiibolink_scene.h"
#include "app_amiibolink.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "settings.h"

#define ICON_MODE 0xe1ed
#define ICON_BACK 0xe069

void amiibolink_scene_menu_ver_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                         mui_list_item_t *p_item) {
    app_amiibolink_t *app = p_list_view->user_data;
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (p_item->icon == ICON_MODE) {
            settings_data_t *p_settings = settings_get_data();
            p_settings->amiibo_link_mode = (ble_amiibolink_ver_t) p_item->user_data;
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBOLINK_SCENE_MAIN);
        } else if (p_item->icon == ICON_BACK) {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    }
}

void amiibolink_scene_menu_ver_on_enter(void *user_data) {
    app_amiibolink_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, ICON_BACK, ">>返回<<", (void *)-1);

    mui_list_view_add_item(app->p_list_view, ICON_MODE, "V1(历史版本)", (void *)BLE_AMIIBOLINK_VER_V1);
    mui_list_view_add_item(app->p_list_view, ICON_MODE, "V2(最新版本)", (void *)BLE_AMIIBOLINK_VER_V2);
    settings_data_t *p_settings = settings_get_data();
    mui_list_view_set_focus(app->p_list_view, p_settings->amiibo_link_mode);
    mui_list_view_set_selected_cb(app->p_list_view, amiibolink_scene_menu_ver_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_LIST);
}

void amiibolink_scene_menu_ver_on_exit(void *user_data) {
    app_amiibolink_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}