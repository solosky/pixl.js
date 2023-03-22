#include "amiibolink_scene.h"
#include "app_amiibolink.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb

void amiibolink_scene_menu_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                    mui_list_item_t *p_item) {
    app_amiibolink_t *app = p_list_view->user_data;
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (p_item->icon == ICON_FILE) {
            //
        } else if (p_item->icon == ICON_BACK) {
            mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIBOLINK);
        }
    } else {
    }
}

void amiibolink_scene_menu_on_enter(void *user_data) {
    app_amiibolink_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, ICON_BACK, ">>返回主菜单<<", (void *)-1);
    mui_list_view_add_item(app->p_list_view, ICON_FILE, "随机模式", (void *)-1);
    mui_list_view_add_item(app->p_list_view, ICON_FILE, "按序模式", (void *)-1);
    mui_list_view_add_item(app->p_list_view, ICON_FILE, "读写模式", (void *)-1);

    mui_list_view_set_selected_cb(app->p_list_view, amiibolink_scene_menu_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_LIST);
}

void amiibolink_scene_menu_on_exit(void *user_data) {
    app_amiibolink_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}