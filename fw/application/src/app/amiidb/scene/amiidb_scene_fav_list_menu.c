#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "app_error.h"
#include "cwalk2.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "settings.h"
#include "vfs.h"

#include "mui_icons.h"
#include "ntag_emu.h"
#include "ntag_store.h"

#include "db_header.h"
#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"

void amiidb_scene_fav_list_menu_on_enter(void *user_data);

static void amiidb_scene_fav_list_menu_msg_box_empty_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiidb_t *app = p_msg_box->user_data;
    if (event == MUI_MSG_BOX_EVENT_SELECT_LEFT) {
        // do empty
        // do delete
        char path[VFS_MAX_PATH_LEN];
        char sub_path[VFS_MAX_PATH_LEN];
        int32_t res;
        vfs_dir_t dir;
        vfs_obj_t obj;

        vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
        if (string_size(app->cur_fav_dir) > 0) {
            sprintf(path, "/amiibo/fav/%s", string_get_cstr(app->cur_fav_dir));
        } else {
            sprintf(path, "/amiibo/fav/%s", string_get_cstr(app->cur_fav_file));
        }

        res = p_driver->open_dir(path, &dir);
        if (res == VFS_OK) {
            while (p_driver->read_dir(&dir, &obj) == VFS_OK) {
                sprintf(sub_path, "/%s/%s", path, obj.name);
                if (obj.type == VFS_TYPE_DIR) {
                    p_driver->remove_dir(sub_path);
                } else {
                    p_driver->remove_file(sub_path);
                }
            }
            p_driver->close_dir(&dir);
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    } else {
        // reload menu
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    }
}

static void amiidb_scene_fav_list_menu_msg_box_delete_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiidb_t *app = p_msg_box->user_data;
    if (event == MUI_MSG_BOX_EVENT_SELECT_LEFT) {
        // do delete
        char path[VFS_MAX_PATH_LEN];
        int32_t res;
        vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
        if (string_size(app->cur_fav_dir) > 0) {
            sprintf(path, "/amiibo/fav/%s/%s", string_get_cstr(app->cur_fav_dir), string_get_cstr(app->cur_fav_file));
            res = p_driver->remove_file(path);
        } else {
            amiidb_api_fav_remove_dir(app->cur_fav_file);
        }
        NRF_LOG_INFO("remove fav: %d", res);
        if (res == VFS_OK) {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    } else {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    }
}

static void amiidb_scene_fav_list_menu_msg_box(app_amiidb_t *app, const char *header, const char *msg,
                                               mui_msg_box_event_cb_t cb) {
    mui_msg_box_set_header(app->p_msg_box, header);
    mui_msg_box_set_message(app->p_msg_box, msg);
    mui_msg_box_set_btn_text(app->p_msg_box, "确认", NULL, "取消");
    mui_msg_box_set_btn_focus(app->p_msg_box, 0);
    mui_msg_box_set_event_cb(app->p_msg_box, cb);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_MSG_BOX);
}

static void amiidb_scene_fav_list_menu_text_input_event_cb(mui_text_input_event_t event,
                                                           mui_text_input_t *p_text_input) {
    app_amiidb_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *text = mui_text_input_get_input_text(p_text_input);
        if (strlen(text) > 0) {
            char path[VFS_MAX_PATH_LEN];
            sprintf(path, "/amiibo/fav/%s", text);
            vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
            int32_t res = p_driver->create_dir(path);
            if (res == VFS_OK) {
                mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
            }
        }
    }
}

static void amiidb_scene_fav_list_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                   mui_list_item_t *p_item) {
    app_amiidb_t *app = p_list_view->user_data;

    switch (p_item->icon) {
    case ICON_EXIT:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;

    case ICON_NEW: {
        mui_text_input_set_header(app->p_text_input, "新建收藏夹:");
        mui_text_input_set_event_cb(app->p_text_input, amiidb_scene_fav_list_menu_text_input_event_cb);
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_INPUT);
    } break;

    case ICON_EMPTY: {
        amiidb_scene_fav_list_menu_msg_box(app, "提示", "确认清空收藏夹？",
                                           amiidb_scene_fav_list_menu_msg_box_empty_cb);
    } break;

    case ICON_DELETE: {
        amiidb_scene_fav_list_menu_msg_box(app, "提示", "确认删除？", amiidb_scene_fav_list_menu_msg_box_delete_cb);
    } break;
    }
}

void amiidb_scene_fav_list_menu_on_enter(void *user_data) {
    app_amiidb_t *app = user_data;

    char txt[32];
    settings_data_t *p_settings = settings_get_data();
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_add_item(app->p_list_view, ICON_NEW, "新建..", (void *)0);
    mui_list_view_add_item(app->p_list_view, ICON_EMPTY, "清空..", (void *)0);
    mui_list_view_add_item(app->p_list_view, ICON_DELETE, "删除..", (void *)0);
    mui_list_view_add_item(app->p_list_view, ICON_EXIT, "[返回]", (void *)0);

    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_fav_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_fav_list_menu_on_exit(void *user_data) {
    app_amiidb_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}
