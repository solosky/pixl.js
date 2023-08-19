#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"

#include "amiidb_api_fav.h"
#include "db_header.h"
#include "mui_icons.h"
#include "settings.h"
#include "vfs.h"
#include <math.h>

#define LINK_MAX_DISPLAY_CNT 100

static void amiidb_scene_fav_select_reload(app_amiidb_t *app);

static void amiidb_scene_fav_select_msg_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiidb_t *app = p_msg_box->user_data;
    mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL);
}

static void amiidb_scene_fav_select_msg_show(app_amiidb_t *app, const char *message) {
    mui_msg_box_set_header(app->p_msg_box, "提示");
    mui_msg_box_set_message(app->p_msg_box, message);
    mui_msg_box_set_btn_text(app->p_msg_box, NULL, "确定", NULL);
    mui_msg_box_set_btn_focus(app->p_msg_box, 1);
    mui_msg_box_set_event_cb(app->p_msg_box, amiidb_scene_fav_select_msg_cb);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_MSG_BOX);
}

static void amiidb_scene_fav_select_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                          mui_list_item_t *p_item) {
    uint16_t icon = p_item->icon;
    app_amiidb_t *app = mui_list_view_get_user_data(p_list_view);

    switch (icon) {
    case ICON_EXIT:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;

    case ICON_FOLDER: {
        uint8_t cur_game_id = app->game_id_path[app->game_id_index];
        const db_amiibo_t *p_amiibo = app->cur_amiibo;
        db_link_t link = {.game_id = cur_game_id, .head = p_amiibo->head, .tail = p_amiibo->tail};
        int res = amiidb_api_fav_add(string_get_cstr(p_item->text), &link);
        if (res == VFS_OK) {
            amiidb_scene_fav_select_msg_show(app, "收藏成功");
        } else {
            amiidb_scene_fav_select_msg_show(app, "收藏失败");
        }
    } break;
    }
}

static void amiidb_scene_fav_select_reload(app_amiidb_t *app) {
    settings_data_t *p_settings_data = settings_get_data();
    char txt[64];
    vfs_driver_t *p_vfs_driver;
    vfs_dir_t dir;
    vfs_obj_t obj;

    // clear list view
    mui_list_view_clear_items(app->p_list_view);

    mui_list_view_add_item(app->p_list_view, ICON_FAVORITE, "选择收藏夹..", (void *)0);
    p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);

    int32_t res = p_vfs_driver->open_dir("/amiibo/fav", &dir);
    if (res == VFS_OK) {
        while (p_vfs_driver->read_dir(&dir, &obj) == VFS_OK) {
            if (obj.type == VFS_TYPE_DIR) {
                uint16_t icon = obj.type == VFS_TYPE_DIR ? ICON_FOLDER : ICON_FILE;
                mui_list_view_add_item(app->p_list_view, icon, obj.name, (void *)-1);
            }
        }
        p_vfs_driver->close_dir(&dir);
    }

    mui_list_view_add_item(app->p_list_view, ICON_EXIT, "[返回]", (void *)0);
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_fav_select_list_view_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_fav_select_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    amiidb_scene_fav_select_reload(app);
}

void amiidb_scene_fav_select_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    mui_list_view_clear_items(app->p_list_view);
}