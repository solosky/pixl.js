#include "app_settings.h"
#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "nrf_pwr_mgmt.h"
#include "settings.h"
#include "settings_scene.h"
#include "utils2.h"
#include "version2.h"

enum settings_sleep_timeout_menu_t {
    SETTINGS_LANGUAGE_ZH_HANS,
    SETTINGS_LANGUAGE_EN_US,
    SETTINGS_LANGUAGE_ZH_TW,
    SETTINGS_LANGUAGE_ES_CO,
    SETTINGS_LANGUAGE_KO_KR,
    SETTINGS_LANGUAGE_MENU_EXIT,
};

static void settings_scene_language_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                          mui_list_item_t *p_item) {
    app_settings_t *app = p_list_view->user_data;
    uint32_t selection = (uint32_t)p_item->user_data;
    settings_data_t *p_settings = settings_get_data();
    switch (selection) {
    case SETTINGS_LANGUAGE_ZH_HANS:
        p_settings->language = LANGUAGE_ZH_HANS;
        setLanguage(p_settings->language);
        break;

    case SETTINGS_LANGUAGE_EN_US:
        p_settings->language = LANGUAGE_EN_US;
        setLanguage(p_settings->language);
        break;

    case SETTINGS_LANGUAGE_ZH_TW:
        p_settings->language = LANGUAGE_ZH_TW;
        setLanguage(p_settings->language);
        break;

    case SETTINGS_LANGUAGE_ES_CO:
        p_settings->language = LANGUAGE_ES_CO;
        setLanguage(p_settings->language);
        break;
    }
    mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
}

void settings_scene_language_on_enter(void *user_data) {

    app_settings_t *app = user_data;
    mui_list_view_add_item(app->p_list_view, 0xe105, "简体中文", (void *)SETTINGS_LANGUAGE_ZH_HANS);
    mui_list_view_add_item(app->p_list_view, 0xe105, "繁體中文(臺灣)", (void *)SETTINGS_LANGUAGE_ZH_TW);
    mui_list_view_add_item(app->p_list_view, 0xe105, "English", (void *)SETTINGS_LANGUAGE_EN_US);
    mui_list_view_add_item(app->p_list_view, 0xe105, getLangDesc(LANGUAGE_ES_CO), (void *)SETTINGS_LANGUAGE_ES_CO);
    mui_list_view_add_item(app->p_list_view, 0xe069, getLangString(_L_BACK), (void *)SETTINGS_LANGUAGE_MENU_EXIT);

    mui_list_view_set_selected_cb(app->p_list_view, settings_scene_language_list_view_on_selected);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

void settings_scene_language_on_exit(void *user_data) {
    app_settings_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
}