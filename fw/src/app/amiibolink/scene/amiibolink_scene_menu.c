#include "amiibolink_scene.h"
#include "app_amiibolink.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define ICON_MODE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_HOME 0xe1f0


const char* mode_name[] = {"", "随机", "按序","读写" };

#define AMIIBOLINK_MENU_BACK_EXIT 0
#define AMIIBOLINK_MENU_BACK_MAIN 1
#define AMIIBOLINK_MENU_MODE 2
#define AMIIBOLINK_MENU_AUTO_GENERATE 3


void amiibolink_scene_menu_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                    mui_list_item_t *p_item) {
    app_amiibolink_t *app = p_list_view->user_data;
    int32_t menu_code = (int32_t)p_item->user_data;
    char txt[32];
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (menu_code == AMIIBOLINK_MENU_BACK_MAIN) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBOLINK_SCENE_MAIN);
        } else if (menu_code == AMIIBOLINK_MENU_MODE) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBOLINK_SCENE_MENU_MODE);
        } else if (menu_code == AMIIBOLINK_MENU_BACK_EXIT) {
            mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIBOLINK);
        }else if(menu_code == AMIIBOLINK_MENU_AUTO_GENERATE){
            app->auto_generate = !app->auto_generate;
            NRF_LOG_INFO("auto_generate: %d", app->auto_generate);
            sprintf(txt, "自动随机 [%s]", app->auto_generate ? "开" : "关");
            string_set_str(p_item->text, txt);
        }
    }
}

void amiibolink_scene_menu_on_enter(void *user_data) {
    app_amiibolink_t *app = user_data;

    char txt[32];
    sprintf(txt, "模式 [%s]", mode_name[app->amiibolink_mode]);
    mui_list_view_add_item(app->p_list_view, ICON_MODE, txt, (void *)AMIIBOLINK_MENU_MODE);
    sprintf(txt, "自动随机 [%s]", app->auto_generate ? "开" : "关");
    mui_list_view_add_item(app->p_list_view, ICON_MODE, txt, (void *)AMIIBOLINK_MENU_AUTO_GENERATE);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, "标签详情", (void *)AMIIBOLINK_MENU_BACK_MAIN);
    mui_list_view_add_item(app->p_list_view, ICON_HOME, ">>主菜单<<", (void *)AMIIBOLINK_MENU_BACK_EXIT);

    mui_list_view_set_selected_cb(app->p_list_view, amiibolink_scene_menu_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_LIST);
}

void amiibolink_scene_menu_on_exit(void *user_data) {
    app_amiibolink_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}