#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vfs.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "settings.h"

#include "mui_icons.h"

typedef enum {
    AMIIDB_MAIN_MENU_VIEW,
    AMIIDB_MAIN_MENU_SEARCH,
    AMIIDB_MAIN_MENU_SETTINGS,
    AMIIDB_MAIN_MENU_DATA,
    AMIIDB_MAIN_MENU_FAVORITE,
    AMIIDB_MAIN_MENU_EXIT,
} amiidb_main_menu_t;

static void amiidb_scene_main_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                    mui_list_item_t *p_item) {
    amiidb_main_menu_t menu = (amiidb_main_menu_t)p_item->user_data;
    app_amiidb_t *app = mui_list_view_get_user_data(p_list_view);
    switch (menu) {
    case AMIIDB_MAIN_MENU_VIEW:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_GAME_LIST);
        break;

    case AMIIDB_MAIN_MENU_SEARCH:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_SEARCH);
        break;

    case AMIIDB_MAIN_MENU_FAVORITE:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_FAV_LIST);
        break;

    case AMIIDB_MAIN_MENU_DATA:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_DATA_LIST);
        break;

    case AMIIDB_MAIN_MENU_SETTINGS: {
        char text[32];
        static uint8_t i = 0;
        sprintf(text, "%s %d", "Settings 中文显示", ++i);
        // mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_SETTINGS);
        mui_toast_view_show(app->p_toast_view, text);
    }
        break;

    case AMIIDB_MAIN_MENU_EXIT:
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIDB);
        break;
    }
}

void amiidb_scene_main_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    mui_list_view_add_item(app->p_list_view, ICON_VIEW,  getLangString(_L_APP_AMMIDB_BROWSER), (void *)AMIIDB_MAIN_MENU_VIEW);
    mui_list_view_add_item(app->p_list_view, ICON_SEARCH, getLangString(_L_APP_AMIIDB_SEARCH), (void *)AMIIDB_MAIN_MENU_SEARCH);
    mui_list_view_add_item(app->p_list_view, ICON_FAVORITE, getLangString(_L_APP_AMIIDB_MY_FAVORITES), (void *)AMIIDB_MAIN_MENU_FAVORITE);
    mui_list_view_add_item(app->p_list_view, ICON_DATA, getLangString(_L_APP_AMIIDB_MY_TAGS), (void *)AMIIDB_MAIN_MENU_DATA);
    mui_list_view_add_item(app->p_list_view, ICON_SETTINGS, getLangString(_L_APP_AMIIDB_SETTINGS), (void *)AMIIDB_MAIN_MENU_SETTINGS);
    mui_list_view_add_item(app->p_list_view, ICON_EXIT, getLangString(_L_APP_AMIIDB_EXIT), (void *)AMIIDB_MAIN_MENU_EXIT);
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_main_list_view_on_selected);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_main_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    mui_list_view_clear_items(app->p_list_view);
}