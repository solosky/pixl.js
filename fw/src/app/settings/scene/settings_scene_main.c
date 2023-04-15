#include "app_settings.h"
#include "mini_app_launcher.h"
#include "nrf_pwr_mgmt.h"
#include "settings_scene.h"
#include "utils.h"
#include "version2.h"
#include "settings.h"

enum settings_main_menu_t {
    SETTINGS_MAIN_MENU_VERSION,
    SETTINGS_MAIN_MENU_BACK_LIGHT,
    SETTINGS_MAIN_MENU_SKIP_DRIVER_SELECT,
    SETTINGS_MAIN_MENU_SLEEP_TIMEOUT,
    SETTINGS_MAIN_MENU_DFU,
    SETTINGS_MAIN_MENU_EXIT
};

static void settings_scene_main_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                      mui_list_item_t *p_item) {
    app_settings_t *app = p_list_view->user_data;
    char txt[32];

    settings_data_t* p_settings = settings_get_data();

    uint32_t selection = (uint32_t)p_item->user_data;
    switch (selection) {
    case SETTINGS_MAIN_MENU_BACK_LIGHT:
        mui_u8g2_set_backlight(!mui_u8g2_get_backlight());
        sprintf(txt, "背光设置 [%s]", mui_u8g2_get_backlight() ? "开" : "关");
        p_settings->backlight = mui_u8g2_get_backlight();
        string_set_str(p_item->text, txt);
        mui_update(mui());
        break;

    case SETTINGS_MAIN_MENU_VERSION:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SETTINGS_SCENE_VERSION);
        break;

    case SETTINGS_MAIN_MENU_SLEEP_TIMEOUT:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SETTINGS_SCENE_SLEEP_TIMEOUT);
        break;

    case SETTINGS_MAIN_MENU_DFU:
        enter_dfu();
        break;

    case SETTINGS_MAIN_MENU_SKIP_DRIVER_SELECT:
        p_settings->skip_driver_select = !p_settings->skip_driver_select;
        sprintf(txt, "自动选择存储 [%s]", p_settings->skip_driver_select ? "开" : "关");
        string_set_str(p_item->text, txt);
        mui_update(mui());
        break;

    case SETTINGS_MAIN_MENU_EXIT:
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_SETTINGS);
        break;
    }
}

void settings_scene_main_on_enter(void *user_data) {

    app_settings_t *app = user_data;
    char txt[32];
    sprintf(txt, "版本 [%s]", version_get_version(version_get()));
    mui_list_view_add_item(app->p_list_view, 0xe1c7, txt, (void *)SETTINGS_MAIN_MENU_VERSION);

    settings_data_t* p_settings = settings_get_data();
    sprintf(txt, "自动选择存储 [%s]", p_settings->skip_driver_select ? "开" : "关");
    mui_list_view_add_item(app->p_list_view, 0xe1c8, txt, (void *)SETTINGS_MAIN_MENU_SKIP_DRIVER_SELECT);

    sprintf(txt, "背光设置 [%s]", mui_u8g2_get_backlight() ? "开" : "关");
    mui_list_view_add_item(app->p_list_view, 0xe1c8, txt, (void *)SETTINGS_MAIN_MENU_BACK_LIGHT);

    sprintf(txt, "休眠时间 [%ds]", nrf_pwr_mgmt_get_timeout());
    mui_list_view_add_item(app->p_list_view, 0xe1c9, txt, (void *)SETTINGS_MAIN_MENU_SLEEP_TIMEOUT);
    mui_list_view_add_item(app->p_list_view, 0xe1ca, "固件更新", (void *)SETTINGS_MAIN_MENU_DFU);
    mui_list_view_add_item(app->p_list_view, 0xe069, "返回主菜单", (void *)SETTINGS_MAIN_MENU_EXIT);

    mui_list_view_set_selected_cb(app->p_list_view, settings_scene_main_list_view_on_selected);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

void settings_scene_main_on_exit(void *user_data) {
    app_settings_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
}