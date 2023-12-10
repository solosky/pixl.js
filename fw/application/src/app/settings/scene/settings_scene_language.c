#include "app_settings.h"
#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "nrf_pwr_mgmt.h"
#include "settings.h"
#include "settings_scene.h"
#include "utils2.h"
#include "version2.h"


static void settings_scene_language_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                          mui_list_item_t *p_item) {

    app_settings_t *app = p_list_view->user_data;
    uint32_t selection = (uint32_t)p_item->user_data;
    settings_data_t *p_settings = settings_get_data();
    if (selection < LANGUAGE_COUNT){
        p_settings->language = selection;
        setLanguage(p_settings->language);
    }

    mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
}

void settings_scene_language_on_enter(void *user_data) {

    app_settings_t *app = user_data;
    for (uint8_t i = 0; i < LANGUAGE_COUNT; i++){
      mui_list_view_add_item(app->p_list_view, 0xe105, getLangDesc(i), (void *)i);
    }
    mui_list_view_add_item(app->p_list_view, 0xe069, getLangString(_L_BACK), (void *)NULL_USER_DATA);

    mui_list_view_set_selected_cb(app->p_list_view, settings_scene_language_list_view_on_selected);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

void settings_scene_language_on_exit(void *user_data) {
    app_settings_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
}
