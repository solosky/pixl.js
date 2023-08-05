#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vfs.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "settings.h"

#define ICON_BACK 0xe069
#define ICON_DRIVE 0xe1bb
#define ICON_HOME 0xe1f0

typedef enum {
    AMIIDB_MAIN_MENU_BACK_HOME,
}amiidb_main_menu_t;

static void amiidb_scene_main_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                    mui_list_item_t *p_item){
    amiidb_main_menu_t menu = (amiidb_main_menu_t) p_item->user_data;
    switch(menu){
    case AMIIDB_MAIN_MENU_BACK_HOME:
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIDB);
        break;
    }
}


void amiidb_scene_main_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *) user_data;
    mui_list_view_add_item(app->p_list_view, 0xe1c7, "按游戏浏览..", NULL);
    mui_list_view_add_item(app->p_list_view, 0xe1c7, "返回主菜单", AMIIDB_MAIN_MENU_BACK_HOME);
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_main_list_view_on_selected);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_main_on_exit(void *user_data) {

}