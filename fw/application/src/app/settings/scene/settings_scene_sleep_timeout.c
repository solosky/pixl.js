#include "app_settings.h"
#include "mini_app_launcher.h"
#include "nrf_pwr_mgmt.h"
#include "settings.h"
#include "settings_scene.h"
#include "utils2.h"
#include "version2.h"
#include "i18n/language.h"

enum settings_sleep_timeout_menu_t {
    SETTINGS_SLEEP_TIMEOUT_MENU_15S,
    SETTINGS_SLEEP_TIMEOUT_MENU_30S,
    SETTINGS_SLEEP_TIMEOUT_MENU_45S,
    SETTINGS_SLEEP_TIMEOUT_MENU_1MIN,
    SETTINGS_SLEEP_TIMEOUT_MENU_2MIN,
    SETTINGS_SLEEP_TIMEOUT_MENU_3MIN,
    SETTINGS_SLEEP_TIMEOUT_MENU_EXIT
};

static void settings_scene_sleep_timeout_list_view_on_selected(mui_list_view_event_t event,
                                                               mui_list_view_t *p_list_view, mui_list_item_t *p_item) {
    app_settings_t *app = p_list_view->user_data;
    uint32_t selection = (uint32_t)p_item->user_data;
    settings_data_t *p_settings = settings_get_data();
    switch (selection) {
    case SETTINGS_SLEEP_TIMEOUT_MENU_15S:
        nrf_pwr_mgmt_set_timeout(15);
        p_settings->sleep_timeout_sec = 15;
        break;

    case SETTINGS_SLEEP_TIMEOUT_MENU_30S:
        nrf_pwr_mgmt_set_timeout(30);
        p_settings->sleep_timeout_sec = 30;
        break;

    case SETTINGS_SLEEP_TIMEOUT_MENU_45S:
        p_settings->sleep_timeout_sec = 45;
        nrf_pwr_mgmt_set_timeout(45);
        break;

    case SETTINGS_SLEEP_TIMEOUT_MENU_1MIN:
        p_settings->sleep_timeout_sec = 60;
        nrf_pwr_mgmt_set_timeout(60);
        break;

    case SETTINGS_SLEEP_TIMEOUT_MENU_2MIN:
        p_settings->sleep_timeout_sec = 120;
        nrf_pwr_mgmt_set_timeout(120);
        break;

    case SETTINGS_SLEEP_TIMEOUT_MENU_3MIN:
        p_settings->sleep_timeout_sec = 180;
        nrf_pwr_mgmt_set_timeout(180);
        break;
    }
    mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
}

void settings_scene_sleep_timeout_on_enter(void *user_data) {

    app_settings_t *app = user_data;
    mui_list_view_add_item(app->p_list_view, 0xe105, getLangString(_L_15S), (void *)SETTINGS_SLEEP_TIMEOUT_MENU_15S);
    mui_list_view_add_item(app->p_list_view, 0xe105, getLangString(_L_30S), (void *)SETTINGS_SLEEP_TIMEOUT_MENU_30S);
    mui_list_view_add_item(app->p_list_view, 0xe105, getLangString(_L_45S), (void *)SETTINGS_SLEEP_TIMEOUT_MENU_45S);
    mui_list_view_add_item(app->p_list_view, 0xe105, getLangString(_L_1MIN), (void *)SETTINGS_SLEEP_TIMEOUT_MENU_1MIN);
    mui_list_view_add_item(app->p_list_view, 0xe105, getLangString(_L_2MIN), (void *)SETTINGS_SLEEP_TIMEOUT_MENU_2MIN);
    mui_list_view_add_item(app->p_list_view, 0xe105, getLangString(_L_3MIN), (void *)SETTINGS_SLEEP_TIMEOUT_MENU_3MIN);
    mui_list_view_add_item(app->p_list_view, 0xe069, getLangString(_L_BACK), (void *)SETTINGS_SLEEP_TIMEOUT_MENU_EXIT);

    mui_list_view_set_selected_cb(app->p_list_view, settings_scene_sleep_timeout_list_view_on_selected);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

void settings_scene_sleep_timeout_on_exit(void *user_data) {
    app_settings_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
}