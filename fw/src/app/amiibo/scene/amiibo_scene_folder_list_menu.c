#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vos.h"

enum folder_list_menu_t {
    FOLDER_LIST_MENU_CREATE_FOLDER,
    FOLDER_LIST_MENU_RENAME_FOLDER,
    FOLDER_LIST_MENU_REMOVE_FOLDER,
    FOLDER_LIST_MENU_EMPTY_FOLDER,
    FOLDER_LIST_MENU_BACK,
};

static void amiibo_scene_folder_list_text_input_event_cb(mui_text_input_event_t event, mui_text_input_t *p_text_input) {
    app_amiibo_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *input_text = mui_text_input_get_input_text(p_text_input);
        if (strlen(input_text) > 0) {
            vos_driver_t *p_driver = vos_get_driver(app->current_drive);
            int32_t res = p_driver->create_folder(VOS_BUCKET_AMIIBO, input_text);
            if (res == VOS_OK) {
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST);
            }
        }
    }
}

static void amiibo_scene_folder_list_menu_text_input_rename_folder_event_cb(mui_text_input_event_t event,
                                                                            mui_text_input_t *p_text_input) {
    app_amiibo_t *app = p_text_input->user_data;
    bool renamed = false;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *input_text = mui_text_input_get_input_text(p_text_input);
        if (strlen(input_text) > 0) {
            vos_driver_t *p_driver = vos_get_driver(app->current_drive);
            int32_t res = p_driver->rename_folder(VOS_BUCKET_AMIIBO, string_get_cstr(app->current_folder), input_text);
            if (res == VOS_OK) {
                renamed = true;
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST);
            }
        }
    }

    if (!renamed) {
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
    }
}

static void amiibo_scence_folder_list_menu_msg_box_on_event(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiibo_t *app = p_msg_box->user_data;
    vos_driver_t *p_driver = vos_get_driver(app->current_drive);
    if (event == MUI_MSG_BOX_EVENT_SELECT_RIGHT) {
        int32_t res = p_driver->remove_folder(VOS_BUCKET_AMIIBO, string_get_cstr(app->current_folder));
        if (res == VOS_OK) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST);
        }
    } else {
        // cancel, return to menu
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
    }
}

static void amiibo_scene_folder_list_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                      mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    vos_driver_t *p_driver = vos_get_driver(app->current_drive);
    switch (index) {
    case FOLDER_LIST_MENU_CREATE_FOLDER: {
        mui_text_input_set_header(app->p_text_input, "Input Folder Name:");
        mui_text_input_set_input_text(app->p_text_input, "");
        mui_text_input_set_event_cb(app->p_text_input, amiibo_scene_folder_list_text_input_event_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT);
        break;
    }

    case FOLDER_LIST_MENU_REMOVE_FOLDER: {
        char msg[64];
        snprintf(msg, sizeof(msg), "删除 %s 吗?", string_get_cstr(app->current_folder));
        mui_msg_box_set_header(app->p_msg_box, "删除文件夹");
        mui_msg_box_set_message(app->p_msg_box, msg);
        mui_msg_box_set_btn_text(app->p_msg_box, "取消", NULL, "删除");
        mui_msg_box_set_btn_focus(app->p_msg_box, 2);
        mui_msg_box_set_event_cb(app->p_msg_box, amiibo_scence_folder_list_menu_msg_box_on_event);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);

    } break;

    case FOLDER_LIST_MENU_RENAME_FOLDER: {
        mui_text_input_set_header(app->p_text_input, "Input Folder Name:");
        mui_text_input_set_input_text(app->p_text_input, string_get_cstr(app->current_folder));
        mui_text_input_set_event_cb(app->p_text_input, amiibo_scene_folder_list_menu_text_input_rename_folder_event_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT);
    } break;

    case FOLDER_LIST_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void amiibo_scene_folder_list_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe1c8, "新建..", (void *)FOLDER_LIST_MENU_CREATE_FOLDER);
    mui_list_view_add_item(app->p_list_view, 0xe1c9, "重命名..", (void *)FOLDER_LIST_MENU_RENAME_FOLDER);
    mui_list_view_add_item(app->p_list_view, 0xe1ca, "删除..", (void *)FOLDER_LIST_MENU_REMOVE_FOLDER);
    mui_list_view_add_item(app->p_list_view, 0xe1cb, "清空..", (void *)FOLDER_LIST_MENU_EMPTY_FOLDER);
    mui_list_view_add_item(app->p_list_view, 0xe069, "返回列表", (void *)FOLDER_LIST_MENU_BACK);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_folder_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_folder_list_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}