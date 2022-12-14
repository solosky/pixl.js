#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "ntag_def.h"
#include "vos.h"

enum amiibo_list_menu_t {
    AMIIBO_LIST_MENU_CREATE_AMIIBO,
    AMIIBO_LIST_MENU_RENAME_AMIIBO,
    AMIIBO_LIST_MENU_REMOVE_AMIIBO,
    AMIIBO_LIST_MENU_BACK,
};

static void amiibo_scence_amiibo_list_menu_msg_box_remove_on_event(mui_msg_box_event_t event,
                                                                   mui_msg_box_t *p_msg_box) {
    app_amiibo_t *app = p_msg_box->user_data;
    vos_driver_t *p_driver = vos_get_driver(app->current_drive);
    if (event == MUI_MSG_BOX_EVENT_SELECT_RIGHT) {
        int32_t res = p_driver->remove_object(VOS_BUCKET_AMIIBO, string_get_cstr(app->current_folder),
                                              string_get_cstr(app->current_file));
        if (res == VOS_OK) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_LIST);
        }
    } else {
        // cancel, return to menu
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
    }
}

static void amiibo_scene_amiibo_list_menu_text_input_rename_event_cb(mui_text_input_event_t event,
                                                                     mui_text_input_t *p_text_input) {
    app_amiibo_t *app = p_text_input->user_data;
    bool renamed = false;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *input_text = mui_text_input_get_input_text(p_text_input);
        if (strlen(input_text) > 0) {
            vos_driver_t *p_driver = vos_get_driver(app->current_drive);
            int32_t res = p_driver->rename_object(VOS_BUCKET_AMIIBO, string_get_cstr(app->current_folder),
                                                  string_get_cstr(app->current_file), input_text);
            if (res == VOS_OK) {
                renamed = true;
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_LIST);
            }
        }
    }

    if (!renamed) {
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
    }
}

static void amiibo_scene_amiibo_list_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                      mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    vos_driver_t *p_driver = vos_get_driver(app->current_drive);
    const char *file = string_get_cstr(app->current_file);
    const char *folder = string_get_cstr(app->current_folder);
    switch (index) {
    case AMIIBO_LIST_MENU_CREATE_AMIIBO: {
        extern const ntag_t default_ntag215;
        ntag_t tmp_ntag;
        memcpy(&tmp_ntag, &default_ntag215, sizeof(ntag_t));

        int32_t res = p_driver->write_object(VOS_BUCKET_AMIIBO, folder, "<empty>.bin", &tmp_ntag, sizeof(tmp_ntag));
        if (res > 0) {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }

        break;
    }

    case AMIIBO_LIST_MENU_RENAME_AMIIBO: {
        mui_text_input_set_header(app->p_text_input, "Input Amiibo Name:");
        mui_text_input_set_input_text(app->p_text_input, string_get_cstr(app->current_file));
        mui_text_input_set_event_cb(app->p_text_input, amiibo_scene_amiibo_list_menu_text_input_rename_event_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT);

        break;
    }

    case AMIIBO_LIST_MENU_REMOVE_AMIIBO: {
        if (strcmp(file, "..") != 0) {

            char msg[64];
            snprintf(msg, sizeof(msg), "删除 %s 吗?", string_get_cstr(app->current_file));
            mui_msg_box_set_header(app->p_msg_box, "删除标签");
            mui_msg_box_set_message(app->p_msg_box, msg);
            mui_msg_box_set_btn_text(app->p_msg_box, "取消", NULL, "删除");
            mui_msg_box_set_btn_focus(app->p_msg_box, 2);
            mui_msg_box_set_event_cb(app->p_msg_box, amiibo_scence_amiibo_list_menu_msg_box_remove_on_event);

            mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);
        }
        break;
    }
    case AMIIBO_LIST_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void amiibo_scene_amiibo_list_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe1c8, "新建标签..", (void *)AMIIBO_LIST_MENU_CREATE_AMIIBO);
    mui_list_view_add_item(app->p_list_view, 0xe1c9, "重命名..", (void *)AMIIBO_LIST_MENU_RENAME_AMIIBO);
    mui_list_view_add_item(app->p_list_view, 0xe1ca, "删除..", (void *)AMIIBO_LIST_MENU_REMOVE_AMIIBO);
    mui_list_view_add_item(app->p_list_view, 0xe069, "返回列表", (void *)AMIIBO_LIST_MENU_BACK);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_amiibo_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_amiibo_list_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}