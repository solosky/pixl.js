#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "cwalk2.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "ntag_def.h"
#include "vfs.h"
#include "ntag_store.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

enum file_browser_menu_t {
    FILE_BROWSER_MENU_FILE_NAME,
    FILE_BROWSER_MENU_CREATE_FOLDER,
    FILE_BROWSER_MENU_CREATE_AMIIBO,
    FILE_BROWSER_MENU_RENAME_FOLDER,
    FILE_BROWSER_MENU_REMOVE_FOLDER,
    FILE_BROWSER_MENU_BACK_FILE_BROWSER,
    FILE_BROWSER_MENU_BACK_MAIN_MENU
};

static void amiibo_scene_file_browser_text_input_create_folder_event_cb(mui_text_input_event_t event,
                                                                        mui_text_input_t *p_text_input) {
    app_amiibo_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *input_text = mui_text_input_get_input_text(p_text_input);
        if (strlen(input_text) > 0) {
            vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
            char path[VFS_MAX_PATH_LEN];
            cwalk_append_segment(path, string_get_cstr(app->current_folder), input_text);
            int32_t res = p_driver->create_dir(path);
            if (res == VFS_OK) {
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
            }
        } else {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    }
}

static void amiibo_scene_file_browser_text_input_create_amiibo_event_cb(mui_text_input_event_t event,
                                                                        mui_text_input_t *p_text_input) {
    app_amiibo_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *input_text = mui_text_input_get_input_text(p_text_input);
        if (strlen(input_text) > 0) {
            vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
            char path[VFS_MAX_PATH_LEN];
            cwalk_append_segment(path, string_get_cstr(app->current_folder), input_text);

            ntag_t ntag;
            ntag_store_new_rand(&ntag);
            int32_t res = p_driver->write_file_data(path, ntag.data, sizeof(ntag.data));
            if (res > 0) {
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
            }
        } else {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    }
}

static void amiibo_scene_file_browser_menu_text_input_rename_folder_event_cb(mui_text_input_event_t event,
                                                                             mui_text_input_t *p_text_input) {
    app_amiibo_t *app = p_text_input->user_data;
    bool renamed = false;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *input_text = mui_text_input_get_input_text(p_text_input);
        if (strlen(input_text) > 0 && strcmp(input_text, string_get_cstr(app->current_file)) != 0) {
            vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
            char path[VFS_MAX_PATH_LEN];
            char path2[VFS_MAX_PATH_LEN];
            vfs_obj_t obj;
            cwalk_append_segment(path, string_get_cstr(app->current_folder), string_get_cstr(app->current_file));
            cwalk_append_segment(path2, string_get_cstr(app->current_folder), input_text);

            int32_t res = p_driver->stat_file(path, &obj);
            if (res == VFS_OK) {
                if (obj.type == VFS_TYPE_DIR) {
                    res = p_driver->rename_dir(path, path2);
                } else {
                    res = p_driver->rename_file(path, path2);
                }
            }

            if (res == VFS_OK) {
                renamed = true;
            }
        }

        if (renamed) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
        }
    }
}

static void amiibo_scene_file_browser_menu_msg_box_remove_folder_event_cb(mui_msg_box_event_t event,
                                                                          mui_msg_box_t *p_msg_box) {
    app_amiibo_t *app = p_msg_box->user_data;
    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);

    char path[VFS_MAX_PATH_LEN];
    vfs_obj_t obj;
    cwalk_append_segment(path, string_get_cstr(app->current_folder), string_get_cstr(app->current_file));

    if (event == MUI_MSG_BOX_EVENT_SELECT_RIGHT) {

        int32_t res = p_driver->stat_file(path, &obj);
        if (res == VFS_OK) {
            if (obj.type == VFS_TYPE_DIR) {
                res = p_driver->remove_dir(path);
            } else {
                res = p_driver->remove_file(path);
            }
        }

        if (res == VFS_OK) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
        }

    } else {
        // cancel, return to menu
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER_MENU);
    }
}

static void amiibo_scene_file_browser_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                       mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
    switch (index) {
    case FILE_BROWSER_MENU_CREATE_FOLDER: {
        mui_text_input_set_header(app->p_text_input, "输入文件夹名:");
        mui_text_input_set_input_text(app->p_text_input, "");
        mui_text_input_set_event_cb(app->p_text_input, amiibo_scene_file_browser_text_input_create_folder_event_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT);
        break;
    }

    case FILE_BROWSER_MENU_CREATE_AMIIBO: {
        mui_text_input_set_header(app->p_text_input, "输入amiibo名:");
        mui_text_input_set_input_text(app->p_text_input, "new.bin");
        mui_text_input_set_event_cb(app->p_text_input, amiibo_scene_file_browser_text_input_create_amiibo_event_cb);
        mui_text_input_set_focus_key(app->p_text_input, ENTER_KEY);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT);
        break;
    }

    case FILE_BROWSER_MENU_REMOVE_FOLDER: {
        char msg[64];
        snprintf(msg, sizeof(msg), "删除 %s 吗?", string_get_cstr(app->current_file));
        mui_msg_box_set_header(app->p_msg_box, "删除");
        mui_msg_box_set_message(app->p_msg_box, msg);
        mui_msg_box_set_btn_text(app->p_msg_box, "取消", NULL, "删除");
        mui_msg_box_set_btn_focus(app->p_msg_box, 2);
        mui_msg_box_set_event_cb(app->p_msg_box, amiibo_scene_file_browser_menu_msg_box_remove_folder_event_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);

    } break;

    case FILE_BROWSER_MENU_RENAME_FOLDER: {
        mui_text_input_set_header(app->p_text_input, "输入新名:");
        mui_text_input_set_input_text(app->p_text_input, string_get_cstr(app->current_file));
        mui_text_input_set_event_cb(app->p_text_input,
                                    amiibo_scene_file_browser_menu_text_input_rename_folder_event_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT);
    } break;

    case FILE_BROWSER_MENU_BACK_FILE_BROWSER:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;

    case FILE_BROWSER_MENU_BACK_MAIN_MENU:
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIBO);
        break;
    }
}

void amiibo_scene_file_browser_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe1c7, string_get_cstr(app->current_file), FILE_BROWSER_MENU_FILE_NAME);

    mui_list_view_add_item(app->p_list_view, 0xe1c8, "新建文件夹..", (void *)FILE_BROWSER_MENU_CREATE_FOLDER);
    mui_list_view_add_item(app->p_list_view, 0xe1c8, "新建标签..", (void *)FILE_BROWSER_MENU_CREATE_AMIIBO);
    if (string_cmp_str(app->current_file, "..") != 0) {
        mui_list_view_add_item(app->p_list_view, 0xe1c9, "重命名..", (void *)FILE_BROWSER_MENU_RENAME_FOLDER);
        mui_list_view_add_item(app->p_list_view, 0xe1ca, "删除..", (void *)FILE_BROWSER_MENU_REMOVE_FOLDER);
    }
    mui_list_view_add_item(app->p_list_view, 0xe069, "返回文件列表", (void *)FILE_BROWSER_MENU_BACK_FILE_BROWSER);
    mui_list_view_add_item(app->p_list_view, 0xe069, "返回主菜单", (void *)FILE_BROWSER_MENU_BACK_MAIN_MENU);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_file_browser_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_file_browser_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}