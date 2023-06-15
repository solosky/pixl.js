#include "app_settings.h"
#include "mini_app_launcher.h"
#include "nrf_pwr_mgmt.h"
#include "settings.h"
#include "settings_scene.h"
#include "utils.h"
#include "version2.h"
#include "messages.h"

enum settings_main_menu_t {
    SETTINGS_MAIN_MENU_VERSION,
    SETTINGS_MAIN_MENU_BACK_LIGHT,
    SETTINGS_MAIN_MENU_LI_MODE,
    SETTINGS_MAIN_MENU_ENABLE_HIBERNATE,
    SETTINGS_MAIN_MENU_SKIP_DRIVER_SELECT,
    SETTINGS_MAIN_MENU_SHOW_MEM_USAGE,
    SETTINGS_MAIN_MENU_SLEEP_TIMEOUT,
    SETTINGS_MAIN_MENU_DFU,
    SETTINGS_MAIN_MENU_LANGUAGE,
    SETTINGS_MAIN_MENU_EXIT,
};

static void settings_scene_main_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                      mui_list_item_t *p_item) {
    app_settings_t *app = p_list_view->user_data;
    char txt[32];

    settings_data_t *p_settings = settings_get_data();

    uint32_t selection = (uint32_t)p_item->user_data;
    switch (selection) {
    case SETTINGS_MAIN_MENU_BACK_LIGHT:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SETTINGS_SCENE_LCD_BACKLIGHT);
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
        sprintf(txt, get_message(MESSAGE_ID_AUTO_SELECT_STORAGE_ARG_STRING), p_settings->skip_driver_select ? get_message(MESSAGE_ID_ON) : get_message(MESSAGE_ID_OFF));
        string_set_str(p_item->text, txt);
        mui_update(mui());
        break;

    case SETTINGS_MAIN_MENU_LI_MODE:
        p_settings->bat_mode = !p_settings->bat_mode;
        sprintf(txt, get_message(MESSAGE_ID_LIPO_BATTERY_ARG_STRING), p_settings->bat_mode ? get_message(MESSAGE_ID_ON) : get_message(MESSAGE_ID_OFF));
        string_set_str(p_item->text, txt);
        mui_update(mui());
        break;

    case SETTINGS_MAIN_MENU_SHOW_MEM_USAGE:
        p_settings->show_mem_usage = !p_settings->show_mem_usage;
        sprintf(txt, get_message(MESSAGE_ID_MEMORY_USED_ARG_STRING), p_settings->show_mem_usage ? get_message(MESSAGE_ID_ON) : get_message(MESSAGE_ID_OFF));
        string_set_str(p_item->text, txt);
        mui_update(mui());
        break;

    case SETTINGS_MAIN_MENU_ENABLE_HIBERNATE:
        p_settings->hibernate_enabled = !p_settings->hibernate_enabled;
        sprintf(txt, get_message(MESSAGE_ID_QUICK_WAKE_ARG_STRING), p_settings->hibernate_enabled ? get_message(MESSAGE_ID_ON) : get_message(MESSAGE_ID_OFF));
        string_set_str(p_item->text, txt);
        mui_update(mui());
        break;

    case SETTINGS_MAIN_MENU_LANGUAGE:
        p_settings->language_id = (p_settings->language_id + 1) % get_languages_count();
        set_messages_language(p_settings->language_id);
        sprintf(txt, "Language [%s]", get_language_name(p_settings->language_id));
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
    sprintf(txt, get_message(MESSAGE_ID_VERSION_ARG_STRING), version_get_version(version_get()));
    mui_list_view_add_item(app->p_list_view, 0xe1c7, txt, (void *)SETTINGS_MAIN_MENU_VERSION);

    settings_data_t *p_settings = settings_get_data();
    sprintf(txt, get_message(MESSAGE_ID_AUTO_SELECT_STORAGE_ARG_STRING), p_settings->skip_driver_select ? get_message(MESSAGE_ID_ON) : get_message(MESSAGE_ID_OFF));
    mui_list_view_add_item(app->p_list_view, 0xe146, txt, (void *)SETTINGS_MAIN_MENU_SKIP_DRIVER_SELECT);

    if (p_settings->lcd_backlight == 0) {
        sprintf(txt, get_message(MESSAGE_ID_BACKLIGHT_OFF));
    } else {
        sprintf(txt, get_message(MESSAGE_ID_BACKLIGHT_ON), p_settings->lcd_backlight);
    }
    mui_list_view_add_item(app->p_list_view, 0xe1c8, txt, (void *)SETTINGS_MAIN_MENU_BACK_LIGHT);

    sprintf(txt, get_message(MESSAGE_ID_LIPO_BATTERY_ARG_STRING), p_settings->bat_mode ? get_message(MESSAGE_ID_ON) : get_message(MESSAGE_ID_OFF));
    mui_list_view_add_item(app->p_list_view, 0xe08f, txt, (void *)SETTINGS_MAIN_MENU_LI_MODE);

    sprintf(txt, get_message(MESSAGE_ID_MEMORY_USED_ARG_STRING), p_settings->show_mem_usage ? get_message(MESSAGE_ID_ON) : get_message(MESSAGE_ID_OFF));
    mui_list_view_add_item(app->p_list_view, 0xe1f3, txt, (void *)SETTINGS_MAIN_MENU_SHOW_MEM_USAGE);

    sprintf(txt, get_message(MESSAGE_ID_QUICK_WAKE_ARG_STRING), p_settings->hibernate_enabled ? get_message(MESSAGE_ID_ON) : get_message(MESSAGE_ID_OFF));
    mui_list_view_add_item(app->p_list_view, 0xe232, txt, (void *)SETTINGS_MAIN_MENU_ENABLE_HIBERNATE);

    sprintf(txt, "Language [%s]", get_language_name(p_settings->language_id));
    mui_list_view_add_item(app->p_list_view, 0xe19f, txt, (void *)SETTINGS_MAIN_MENU_LANGUAGE);

    sprintf(txt, get_message(MESSAGE_ID_SLEEP_TIMER_ARG_INT), nrf_pwr_mgmt_get_timeout());
    mui_list_view_add_item(app->p_list_view, 0xe1c9, txt, (void *)SETTINGS_MAIN_MENU_SLEEP_TIMEOUT);
    mui_list_view_add_item(app->p_list_view, 0xe1ca, get_message(MESSAGE_ID_UPDATE_FIRMWARE), (void *)SETTINGS_MAIN_MENU_DFU);
    mui_list_view_add_item(app->p_list_view, 0xe069, get_message(MESSAGE_ID_BACK_TO_MAIN_MENU), (void *)SETTINGS_MAIN_MENU_EXIT);

    mui_list_view_set_selected_cb(app->p_list_view, settings_scene_main_list_view_on_selected);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

void settings_scene_main_on_exit(void *user_data) {
    app_settings_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
}