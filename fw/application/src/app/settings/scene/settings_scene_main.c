#include "app_settings.h"
#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "nrf_pwr_mgmt.h"
#include "settings.h"
#include "settings_scene.h"
#include "utils2.h"
#include "version2.h"
#include "vfs.h"

enum settings_main_menu_t {
    SETTINGS_MAIN_MENU_VERSION,
    SETTINGS_MAIN_MENU_BACK_LIGHT,
    SETTINGS_MAIN_MENU_OLED_CONTRAST,
    SETTINGS_MAIN_MENU_LI_MODE,
    SETTINGS_MAIN_MENU_ENABLE_HIBERNATE,
    SETTINGS_MAIN_MENU_STORAGE,
    SETTINGS_MAIN_MENU_LANGUAGE,
    SETTINGS_MAIN_MENU_SHOW_MEM_USAGE,
    SETTINGS_MAIN_MENU_SLEEP_TIMEOUT,
    SETTINGS_MAIN_MENU_ANIM_ENABLED,
    SETTINGS_MAIN_MENU_GO_SLEEP,
    SETTINGS_MAIN_MENU_DFU,
    SETTINGS_MAIN_MENU_REBOOT,
    SETTINGS_MAIN_MENU_RESET_DEFAULT,
    SETTINGS_MAIN_MENU_ABOUT,
    SETTINGS_MAIN_MENU_EXIT
};

static void settings_scene_main_reload(void *user_data);
static void settings_reset_default(void *user_data) {
    app_settings_t *app = user_data;
    settings_data_t *p_settings = settings_get_data();
    mui_u8g2_set_contrast_level(p_settings->oled_contrast);
#ifdef LCD_SCREEN
    mui_u8g2_set_backlight_level(p_settings->lcd_backlight);
#endif
    nrf_pwr_mgmt_set_timeout(p_settings->sleep_timeout_sec);
    setLanguage(p_settings->language);

    mui_toast_view_show(app->p_toast_view, _T(APP_SET_RESET_DEFAULT_SUCCESS));
}

static void settings_scene_main_msg_box_reset_settings_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_settings_t *app = p_msg_box->user_data;
    if (event == MUI_MSG_BOX_EVENT_SELECT_LEFT) {
        settings_reset();
        settings_reset_default(app);
        settings_scene_main_reload(app);
    }
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

static void settings_scene_main_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                      mui_list_item_t *p_item) {
    app_settings_t *app = p_list_view->user_data;
    char txt[32];

    settings_data_t *p_settings = settings_get_data();

    uint32_t selection = (uint32_t)p_item->user_data;
    switch (selection) {
#ifdef LCD_SCREEN
    case SETTINGS_MAIN_MENU_BACK_LIGHT:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SETTINGS_SCENE_LCD_BACKLIGHT);
        break;
#endif

    case SETTINGS_MAIN_MENU_OLED_CONTRAST:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SETTINGS_SCENE_OLED_CONTRAST);
        break;

    case SETTINGS_MAIN_MENU_VERSION:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SETTINGS_SCENE_VERSION);
        break;

    case SETTINGS_MAIN_MENU_SLEEP_TIMEOUT:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SETTINGS_SCENE_SLEEP_TIMEOUT);
        break;

    case SETTINGS_MAIN_MENU_LANGUAGE:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SETTINGS_SCENE_LANGUAGE);
        break;

    case SETTINGS_MAIN_MENU_GO_SLEEP:
        go_sleep();
        break;

    case SETTINGS_MAIN_MENU_DFU:
        enter_dfu();
        break;

    case SETTINGS_MAIN_MENU_STORAGE:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SETTINGS_SCENE_STORAGE);
        break;

    case SETTINGS_MAIN_MENU_LI_MODE:
        p_settings->bat_mode = !p_settings->bat_mode;
        settings_scene_main_reload(app);
        break;

    case SETTINGS_MAIN_MENU_SHOW_MEM_USAGE:
        p_settings->show_mem_usage = !p_settings->show_mem_usage;
        settings_scene_main_reload(app);
        break;

    case SETTINGS_MAIN_MENU_ENABLE_HIBERNATE:
        p_settings->hibernate_enabled = !p_settings->hibernate_enabled;
        settings_scene_main_reload(app);
        break;

    case SETTINGS_MAIN_MENU_ANIM_ENABLED:
        p_settings->anim_enabled = !p_settings->anim_enabled;
        settings_scene_main_reload(app);
        break;

    case SETTINGS_MAIN_MENU_EXIT:
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_SETTINGS);
        break;

    case SETTINGS_MAIN_MENU_REBOOT:
        settings_save();
        system_reboot();
        break;

    case SETTINGS_MAIN_MENU_RESET_DEFAULT: {
        mui_msg_box_set_header(app->p_msg_box, _T(APP_SET_RESET_DEFAULT));
        mui_msg_box_set_message(app->p_msg_box, _T(APP_SET_RESET_DEFAULT_CONFIRM));
        mui_msg_box_set_btn_text(app->p_msg_box, _T(CONFIRM), NULL, _T(CANCEL));
        mui_msg_box_set_btn_focus(app->p_msg_box, 2);
        mui_msg_box_set_event_cb(app->p_msg_box, settings_scene_main_msg_box_reset_settings_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MSG_BOX);
    } break;

    case SETTINGS_MAIN_MENU_ABOUT: {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SETTINGS_SCENE_ABOUT);
    } break;
    }
}

static void settings_scene_main_reload(void *user_data) {

    app_settings_t *app = user_data;
    settings_data_t *p_settings = settings_get_data();
    char txt[64];

    uint16_t scroll_offset = mui_list_view_get_scroll_offset(app->p_list_view);
    uint16_t foucs_index = mui_list_view_get_focus(app->p_list_view);

    mui_list_view_clear_items(app->p_list_view);

    snprintf(txt, sizeof(txt), "[%s]", version_get_version(version_get()));
    mui_list_view_add_item_ext(app->p_list_view, 0xe1c7, _T(APP_SET_VERSION), txt, (void *)SETTINGS_MAIN_MENU_VERSION);

    snprintf(txt, sizeof(txt), "[%s]", getLangDesc(p_settings->language));
    mui_list_view_add_item_ext(app->p_list_view, 0xe26d, _T(APP_SET_LANGUAGE), txt,
                               (void *)SETTINGS_MAIN_MENU_LANGUAGE);

    vfs_driver_t *p_driver = vfs_get_default_driver();
    vfs_stat_t stat = {0};
    int32_t res = p_driver->stat(&stat);
    if (stat.avaliable) {
        snprintf(txt, sizeof(txt), "[%s %02d%%]", _T(APP_SET_STORAGE_USED),
                 (uint8_t)((stat.total_bytes - stat.free_bytes) / (float)stat.total_bytes * 100));
    } else {
        snprintf(txt, sizeof(txt), "[%s]", _T(NOT_MOUNTED));
    }
    mui_list_view_add_item_ext(app->p_list_view, 0xe146, _T(APP_SET_STORAGE), txt, (void *)SETTINGS_MAIN_MENU_STORAGE);

    snprintf(txt, sizeof(txt), "[%d%%]", p_settings->oled_contrast);
    mui_list_view_add_item_ext(app->p_list_view, 0xe1c8, _T(APP_SET_OLED_CONTRAST), txt,
                               (void *)SETTINGS_MAIN_MENU_OLED_CONTRAST);

#ifdef LCD_SCREEN
    if (p_settings->lcd_backlight == 0) {
        snprintf(txt, sizeof(txt), "%s", getLangString(_L_OFF_F));
    } else {
        snprintf(txt, sizeof(txt), "[%d%%]", p_settings->lcd_backlight);
    }
    mui_list_view_add_item_ext(app->p_list_view, 0xe1c8, _T(APP_SET_LCD_BACKLIGHT), txt,
                               (void *)SETTINGS_MAIN_MENU_BACK_LIGHT);
#endif

    mui_list_view_add_item_ext(app->p_list_view, 0xe1dc, _T(APP_SET_ANIM),
                               p_settings->anim_enabled ? _T(ON_F) : _T(OFF_F),
                               (void *)SETTINGS_MAIN_MENU_ANIM_ENABLED);

    mui_list_view_add_item_ext(app->p_list_view, 0xe08f, _T(APP_SET_LIPO_BAT),
                               p_settings->bat_mode ? _T(ON_F) : _T(OFF_F), (void *)SETTINGS_MAIN_MENU_LI_MODE);

    mui_list_view_add_item_ext(app->p_list_view, 0xe1f3, _T(APP_SET_SHOW_MEM_USAGE),
                               p_settings->show_mem_usage ? _T(ON_F) : _T(OFF_F),
                               (void *)SETTINGS_MAIN_MENU_SHOW_MEM_USAGE);

    mui_list_view_add_item_ext(app->p_list_view, 0xe232, _T(APP_SET_HIBERNATE),
                               p_settings->hibernate_enabled ? _T(ON_F) : _T(OFF_F),
                               (void *)SETTINGS_MAIN_MENU_ENABLE_HIBERNATE);

    if (nrf_pwr_mgmt_get_timeout() == 0) {
        snprintf(txt, sizeof(txt), "%s", getLangString(_L_OFF_F));
    } else if (nrf_pwr_mgmt_get_timeout() > 0 && nrf_pwr_mgmt_get_timeout() < 60) {
        snprintf(txt, sizeof(txt), "[%ds]", nrf_pwr_mgmt_get_timeout());
    } else {
        snprintf(txt, sizeof(txt), "[%dm]", nrf_pwr_mgmt_get_timeout() / 60);
    }
    mui_list_view_add_item_ext(app->p_list_view, 0xe1c9, _T(APP_SET_SLEEP_TIMEOUT), txt,
                               (void *)SETTINGS_MAIN_MENU_SLEEP_TIMEOUT);

    mui_list_view_add_item(app->p_list_view, 0xe1c9, _T(APP_SET_GO_SLEEP), (void *)SETTINGS_MAIN_MENU_GO_SLEEP);
    mui_list_view_add_item(app->p_list_view, 0xe1ca, _T(APP_SET_DFU), (void *)SETTINGS_MAIN_MENU_DFU);
    mui_list_view_add_item(app->p_list_view, 0xe1cb, _T(APP_SET_REBOOT), (void *)SETTINGS_MAIN_MENU_REBOOT);
    mui_list_view_add_item(app->p_list_view, 0xe1ce, _T(APP_SET_RESET_DEFAULT),
                           (void *)SETTINGS_MAIN_MENU_RESET_DEFAULT);

    mui_list_view_add_item(app->p_list_view, 0xe1cf, _T(APP_SET_ABOUT), (void *)SETTINGS_MAIN_MENU_ABOUT);

    mui_list_view_add_item(app->p_list_view, 0xe069, _T(BACK_TO_MAIN_MENU), (void *)SETTINGS_MAIN_MENU_EXIT);

    mui_list_view_set_focus(app->p_list_view, foucs_index);
    mui_list_view_set_scroll_offset(app->p_list_view, scroll_offset);
}

void settings_scene_main_on_enter(void *user_data) {
    app_settings_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
    settings_scene_main_reload(user_data);
    mui_list_view_set_selected_cb(app->p_list_view, settings_scene_main_list_view_on_selected);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

void settings_scene_main_on_exit(void *user_data) {
    app_settings_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
}