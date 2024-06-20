#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "cwalk2.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "ntag_def.h"
#include "ntag_store.h"
#include "vfs.h"
#include "mui_icons.h"


#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"


#define AMIIBO_MAX_AMIIBO_IN_BATCH 50

enum file_browser_menu_t {
    FILE_BROWSER_MENU_FILE_NAME,
    FILE_BROWSER_MENU_CREATE_FOLDER,
    FILE_BROWSER_MENU_CREATE_AMIIBO,
    FILE_BROWSER_MENU_CREATE_AMIIBO_BATCH,
    FILE_BROWSER_MENU_RENAME_FOLDER,
    FILE_BROWSER_MENU_REMOVE_FOLDER,
    FILE_BROWSER_MENU_BACK_FILE_BROWSER,
    FILE_BROWSER_MENU_BACK_MAIN_MENU
};

static void amiibo_scene_scene_file_browser_menu_msg_box_error_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiibo_t *app = p_msg_box->user_data;
    mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
}

static void amiibo_scene_file_browser_show_messege(app_amiibo_t *app, const char *msg) {
    mui_msg_box_set_header(app->p_msg_box, getLangString(_L_ERR));
    mui_msg_box_set_message(app->p_msg_box, msg);
    mui_msg_box_set_btn_text(app->p_msg_box, NULL, getLangString(_L_BACK), NULL);
    mui_msg_box_set_btn_focus(app->p_msg_box, 1);
    mui_msg_box_set_event_cb(app->p_msg_box, amiibo_scene_scene_file_browser_menu_msg_box_error_cb);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);
}

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

static void amiibo_scene_file_browser_text_input_create_amiibo_batch_event_cb(mui_text_input_event_t event,
                                                                              mui_text_input_t *p_text_input) {
    app_amiibo_t *app = p_text_input->user_data;
    char msg[32];
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *input_text = mui_text_input_get_input_text(p_text_input);
        uint32_t num_amiibo = 0;

        if (strlen(input_text) == 0) {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
            return;
        }

        if (sscanf(input_text, "%u", &num_amiibo) != 1) {
            amiibo_scene_file_browser_show_messege(app, _T(INVALID_INPUT));
            return;
        }

        if (num_amiibo > AMIIBO_MAX_AMIIBO_IN_BATCH) {
            sprintf(msg, _T(CREATE_TOO_MANY_NUM), AMIIBO_MAX_AMIIBO_IN_BATCH);
            amiibo_scene_file_browser_show_messege(app, msg);
            return;
        }

        vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
        char path[VFS_MAX_PATH_LEN];
        char file_name[VFS_MAX_NAME_LEN];
        ntag_t ntag;

        mui_msg_box_set_header(app->p_msg_box, _T(CREATING_TAG_BATCH));
        mui_msg_box_set_btn_text(app->p_msg_box, NULL, NULL, NULL);
        mui_msg_box_set_btn_focus(app->p_msg_box, 1);
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);

        for (uint8_t i = 0; i < num_amiibo; i++) {
            sprintf(file_name, "new_%02d.bin", i + 1);

            sprintf(msg, "%s %s ..", _T(CREATING_TAG_BATCH), file_name);
            mui_msg_box_set_message(app->p_msg_box, msg);
            mui_update_now(mui());

            cwalk_append_segment(path, string_get_cstr(app->current_folder), file_name);
            ntag_store_new_rand(&ntag);
            int32_t res = p_driver->write_file_data(path, ntag.data, sizeof(ntag.data));
            if (res < 0) {
                sprintf(msg, _T(CREATING_TAG_FAILED), file_name);
                amiibo_scene_file_browser_show_messege(app, msg);
                return;
            }
        }

        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
    }
}

static void amiibo_scene_file_browser_menu_text_input_rename_folder_event_cb(mui_text_input_event_t event,
                                                                             mui_text_input_t *p_text_input) {
    app_amiibo_t *app = p_text_input->user_data;
    bool renamed = false;
    int32_t res;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *input_text = mui_text_input_get_input_text(p_text_input);
        if (strlen(input_text) > 0 && strcmp(input_text, string_get_cstr(app->current_file)) != 0) {
            vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
            char path[VFS_MAX_PATH_LEN];
            char path2[VFS_MAX_PATH_LEN];
            vfs_obj_t obj;
            cwalk_append_segment(path, string_get_cstr(app->current_folder), string_get_cstr(app->current_file));
            cwalk_append_segment(path2, string_get_cstr(app->current_folder), input_text);

            res = p_driver->stat_file(path, &obj);
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
        } else {
            // ignore error
            renamed = true;
        }

        if (renamed) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
        } else {
            char msg[32];
            sprintf(msg, "%s\n%s:%d", getLangString(_L_RENAME_FAILED), getLangString(_L_ERR_CODE), res);
            mui_msg_box_set_header(app->p_msg_box, getLangString(_L_ERR));
            mui_msg_box_set_message(app->p_msg_box, msg);
            mui_msg_box_set_btn_text(app->p_msg_box, NULL, getLangString(_L_BACK), NULL);
            mui_msg_box_set_btn_focus(app->p_msg_box, 1);
            mui_msg_box_set_event_cb(app->p_msg_box, amiibo_scene_scene_file_browser_menu_msg_box_error_cb);

            mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);
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

    if (event == MUI_MSG_BOX_EVENT_SELECT_LEFT) {

        int32_t res = p_driver->stat_file(path, &obj);
        if (res == VFS_OK) {
            if (obj.type == VFS_TYPE_DIR) {
                res = p_driver->remove_dir(path);
            } else {
                res = p_driver->remove_file(path);
            }
        }

        if (res == VFS_OK) {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }

    } else {
        // cancel, return to menu
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
    }
}

static void amiibo_scene_file_browser_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                       mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
    switch (index) {
    case FILE_BROWSER_MENU_CREATE_FOLDER: {
        mui_text_input_set_header(app->p_text_input, getLangString(_L_INPUT_FOLDER_NAME));
        mui_text_input_set_input_text(app->p_text_input, "");
        mui_text_input_set_event_cb(app->p_text_input, amiibo_scene_file_browser_text_input_create_folder_event_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT);
        break;
    }

    case FILE_BROWSER_MENU_CREATE_AMIIBO: {
        mui_text_input_set_header(app->p_text_input, getLangString(_L_INPUT_AMIIBO_NAME));
        mui_text_input_set_input_text(app->p_text_input, "new.bin");
        mui_text_input_set_event_cb(app->p_text_input, amiibo_scene_file_browser_text_input_create_amiibo_event_cb);
        mui_text_input_set_focus_key(app->p_text_input, ENTER_KEY);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT);
        break;
    }

    case FILE_BROWSER_MENU_CREATE_AMIIBO_BATCH: {
        mui_text_input_set_header(app->p_text_input, _T(INPUT_TAG_NUM));
        mui_text_input_set_input_text(app->p_text_input, "1");
        mui_text_input_set_event_cb(app->p_text_input,
                                    amiibo_scene_file_browser_text_input_create_amiibo_batch_event_cb);
        mui_text_input_set_focus_key(app->p_text_input, ENTER_KEY);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT);

        break;
    }

    case FILE_BROWSER_MENU_REMOVE_FOLDER: {
        char msg[64];
        snprintf(msg, sizeof(msg), _T(DELETE_FILE), string_get_cstr(app->current_file));
        mui_msg_box_set_header(app->p_msg_box, getLangString(_L_TIPS));
        mui_msg_box_set_message(app->p_msg_box, msg);
        mui_msg_box_set_btn_text(app->p_msg_box, getLangString(_L_CONFIRM), NULL, getLangString(_L_CANCEL));
        mui_msg_box_set_btn_focus(app->p_msg_box, 2);
        mui_msg_box_set_event_cb(app->p_msg_box, amiibo_scene_file_browser_menu_msg_box_remove_folder_event_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);

    } break;

    case FILE_BROWSER_MENU_RENAME_FOLDER: {
        mui_text_input_set_header(app->p_text_input, getLangString(_L_INPUT_NEW_NAME));
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

    mui_list_view_add_item(app->p_list_view, ICON_FILE, string_get_cstr(app->current_file), FILE_BROWSER_MENU_FILE_NAME);

    mui_list_view_add_item(app->p_list_view, ICON_NEW, getLangString(_L_CREATE_NEW_FOLDER),
                           (void *)FILE_BROWSER_MENU_CREATE_FOLDER);
    mui_list_view_add_item(app->p_list_view, ICON_NEW, getLangString(_L_CREATE_NEW_TAG),
                           (void *)FILE_BROWSER_MENU_CREATE_AMIIBO);
    mui_list_view_add_item(app->p_list_view, ICON_NEW, getLangString(_L_CREATE_NEW_TAG_BATCH),
                           (void *)FILE_BROWSER_MENU_CREATE_AMIIBO_BATCH);
    if (string_cmp_str(app->current_file, "..") != 0) {
        char txt[32];
        snprintf(txt, sizeof(txt), "%s", getLangString(_L_RENAME));
        mui_list_view_add_item(app->p_list_view, ICON_EMPTY, txt, (void *)FILE_BROWSER_MENU_RENAME_FOLDER);
        snprintf(txt, sizeof(txt), "%s", getLangString(_L_DELETE));
        mui_list_view_add_item(app->p_list_view, ICON_DELETE, txt, (void *)FILE_BROWSER_MENU_REMOVE_FOLDER);
    }
    mui_list_view_add_item(app->p_list_view, ICON_BACK, getLangString(_L_BACK_TO_FILE_LIST),
                           (void *)FILE_BROWSER_MENU_BACK_FILE_BROWSER);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, getLangString(_L_BACK_TO_MAIN_MENU),
                           (void *)FILE_BROWSER_MENU_BACK_MAIN_MENU);

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