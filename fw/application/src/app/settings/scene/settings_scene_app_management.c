#include "app_settings.h"
#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "mui_icons.h"
#include "settings.h"
#include "settings_scene.h"
#include "settings_scene_app_management.h"
#include "utils2.h"

static void settings_scene_app_management_reload(void *user_data);


static void settings_scene_app_management_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                      mui_list_item_t *p_item) {
    app_settings_t *app = p_list_view->user_data;
    settings_data_t *p_settings = settings_get_data();

    // Check if the item is the back button (icon ICON_BACK)
    if (p_item->icon == ICON_BACK) {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } else {
        uint32_t app_id = (uint32_t)p_item->user_data;
        // Settings app cannot be disabled
        if (app_id != MINI_APP_ID_SETTINGS) {
            p_settings->app_enable_bits ^= (1 << app_id);
        }
        settings_scene_app_management_reload(app);
    }
}

static void settings_scene_app_management_reload(void *user_data) {
    app_settings_t *app = user_data;
    settings_data_t *p_settings = settings_get_data();
    char txt[64];

    uint16_t scroll_offset = mui_list_view_get_scroll_offset(app->p_list_view);
    uint16_t foucs_index = mui_list_view_get_focus(app->p_list_view);

    mui_list_view_clear_items(app->p_list_view);

    const mini_app_t *p_app;
    uint32_t app_count = mini_app_registry_get_app_num();
    for (uint32_t i = 0; i < app_count; i++) {
        p_app = mini_app_registry_find_by_index(i);
        uint32_t app_id = p_app->id;
        bool is_enabled = (p_settings->app_enable_bits & (1 << app_id)) != 0;
        // Settings app cannot be disabled
        if (p_app->id != MINI_APP_ID_SETTINGS && !p_app->sys) {
            mui_list_view_add_item_ext(app->p_list_view, 0xe1d0, getLangString(p_app->name_i18n_key),is_enabled ? _T(ON_F) : _T(OFF_F), (void *)app_id);
        }
    }

    mui_list_view_add_item(app->p_list_view, ICON_BACK, _T(BACK), (void *)0);
    mui_list_view_set_focus(app->p_list_view, foucs_index);
    mui_list_view_set_scroll_offset(app->p_list_view, scroll_offset);
}

void settings_scene_app_management_on_enter(void *user_data) {
    app_settings_t *app = user_data;
    settings_scene_app_management_reload(user_data);
    mui_list_view_set_selected_cb(app->p_list_view, settings_scene_app_management_list_view_on_selected);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

void settings_scene_app_management_on_exit(void *user_data) {
    app_settings_t *app = user_data;
    settings_save();
    mui_list_view_clear_items(app->p_list_view);
}