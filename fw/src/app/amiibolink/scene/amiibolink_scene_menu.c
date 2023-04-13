#include "amiibolink_scene.h"
#include "app_amiibolink.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "amiibo_helper.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "settings.h"

#define ICON_MODE 0xe135
#define ICON_BACK 0xe069
#define ICON_HOME 0xe1f0
#define ICON_AUTO 0xe11f

const char *mode_name[] = {"", "随机", "按序", "读写"};

#define AMIIBOLINK_MENU_BACK_EXIT 0
#define AMIIBOLINK_MENU_BACK_MAIN 1
#define AMIIBOLINK_MENU_MODE 2
#define AMIIBOLINK_MENU_AUTO_GENERATE 3

static void amiibolink_scene_amiibo_detail_menu_msg_box_no_key_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiibolink_t *app = p_msg_box->user_data;
    if (event == MUI_MSG_BOX_EVENT_SELECT_CENTER) {
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_LIST);
    }
}

static void amiibolink_scene_amiibo_detail_no_key_msg(app_amiibolink_t *app) {
    mui_msg_box_set_header(app->p_msg_box, "Amiibo Key未加载");
    mui_msg_box_set_message(app->p_msg_box, "上传文件 key_retail.bin\n到存储根目录下。");
    mui_msg_box_set_btn_text(app->p_msg_box, NULL, "知道了", NULL);
    mui_msg_box_set_btn_focus(app->p_msg_box, 1);
    mui_msg_box_set_event_cb(app->p_msg_box, amiibolink_scene_amiibo_detail_menu_msg_box_no_key_cb);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_MSG_BOX);
}

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
        } else if (menu_code == AMIIBOLINK_MENU_AUTO_GENERATE) {

            if (!amiibo_helper_is_key_loaded() && !app->auto_generate) {
                amiibolink_scene_amiibo_detail_no_key_msg(app);
                return;
            }

            app->auto_generate = !app->auto_generate;
            NRF_LOG_INFO("auto_generate: %d", app->auto_generate);
            sprintf(txt, "自动随机 [%s]", app->auto_generate ? "开" : "关");

            settings_data_t* p_settings = settings_get_data();
            p_settings->auto_gen = app->auto_generate;
            settings_save();

            string_set_str(p_item->text, txt);
        }
    }
}

void amiibolink_scene_menu_on_enter(void *user_data) {
    app_amiibolink_t *app = user_data;

    char txt[32];
    sprintf(txt, "模式 [%s]", mode_name[app->amiibolink_mode]);
    mui_list_view_add_item(app->p_list_view, ICON_MODE, txt, (void *)AMIIBOLINK_MENU_MODE);

    settings_data_t* p_settings = settings_get_data();
    app->auto_generate = p_settings->auto_gen;

    sprintf(txt, "自动随机 [%s]", app->auto_generate ? "开" : "关");
    mui_list_view_add_item(app->p_list_view, ICON_AUTO, txt, (void *)AMIIBOLINK_MENU_AUTO_GENERATE);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, "标签详情", (void *)AMIIBOLINK_MENU_BACK_MAIN);
    mui_list_view_add_item(app->p_list_view, ICON_HOME, ">>主菜单<<", (void *)AMIIBOLINK_MENU_BACK_EXIT);

    mui_list_view_set_selected_cb(app->p_list_view, amiibolink_scene_menu_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_LIST);
}

void amiibolink_scene_menu_on_exit(void *user_data) {
    app_amiibolink_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}