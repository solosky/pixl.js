#include "amiibo_helper.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "app_error.h"
#include "cwalk2.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "settings.h"
#include "vfs.h"

#include "ntag_emu.h"
#include "ntag_store.h"

#include "db_header.h"
#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"

enum amiibo_detail_menu_t {
    AMIIBO_DETAIL_MENU_RAND_UID,
    AMIIBO_DETAIL_MENU_AUTO_RAND_UID,
    AMIIBO_DETAIL_MENU_REMOVE_AMIIBO,
    AMIIBO_DETAIL_MENU_BACK_AMIIBO_DETAIL,
    AMIIBO_DETAIL_MENU_BACK_FILE_BROWSER,
    AMIIBO_DETAIL_MENU_BACK_MAIN_MENU,
};

static void amiibo_scene_amiibo_detail_menu_msg_box_no_key_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiibo_t *app = p_msg_box->user_data;
    if (event == MUI_MSG_BOX_EVENT_SELECT_CENTER) {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    }
}

static void amiibo_scene_amiibo_detail_no_key_msg(app_amiibo_t *app) {
    mui_msg_box_set_header(app->p_msg_box, getLangString(_L_AMIIBO_KEY_UNLOADED));
    mui_msg_box_set_message(app->p_msg_box, getLangString(_L_UPLOAD_KEY_RETAIL_BIN));
    mui_msg_box_set_btn_text(app->p_msg_box, NULL, getLangString(_L_KNOW), NULL);
    mui_msg_box_set_btn_focus(app->p_msg_box, 1);
    mui_msg_box_set_event_cb(app->p_msg_box, amiibo_scene_amiibo_detail_menu_msg_box_no_key_cb);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);
}

static void amiibo_scene_amiibo_detail_delete_tag_confirmed(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    char path[VFS_MAX_PATH_LEN];
    app_amiibo_t *app = p_msg_box->user_data;

    if (event == MUI_MSG_BOX_EVENT_SELECT_LEFT) {

        vfs_driver_t *p_vfs_driver = vfs_get_driver(app->current_drive);
        cwalk_append_segment(path, string_get_cstr(app->current_folder), string_get_cstr(app->current_file));
        int res = p_vfs_driver->remove_file(path);

        if (res == VFS_OK) {
            uint8_t focus = amiibo_detail_view_get_focus(app->p_amiibo_detail_view);
            bool reload = false;
            if (focus > 0) {
                string_set(app->current_file, *string_array_get(app->amiibo_files, focus - 1));
                reload = true;
            } else if (string_array_size(app->amiibo_files) > 1) {
                string_set(app->current_file, *string_array_get(app->amiibo_files, focus + 1));
                reload = true;
            } else {
                string_set_str(app->current_file, "");
            }

            if (reload) {
                app->reload_amiibo_files = true;
                mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
            } else {
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
            }
        }
    } else {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    }
}

static void amiibo_scene_amiibo_detail_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                        mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;

    uint32_t selection = (uint32_t)p_item->user_data;
    char msg[32];

    switch (selection) {
    case AMIIBO_DETAIL_MENU_BACK_FILE_BROWSER:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
        break;
    case AMIIBO_DETAIL_MENU_RAND_UID: {
        ret_code_t err_code;
        ntag_t *ntag_current = &app->ntag;
        uint32_t head = to_little_endian_int32(&ntag_current->data[84]);
        uint32_t tail = to_little_endian_int32(&ntag_current->data[88]);

        const db_amiibo_t *amd = get_amiibo_by_id(head, tail);
        if (amd == NULL) {
            NRF_LOG_WARNING("amiibo not found:[%08x:%08x]", head, tail);
            return;
        }

        if (!amiibo_helper_is_key_loaded()) {
            amiibo_scene_amiibo_detail_no_key_msg(app);
            return;
        }

        err_code = amiibo_helper_rand_amiibo_uuid(ntag_current);
        APP_ERROR_CHECK(err_code);
        if (err_code == NRF_SUCCESS) {
            ntag_emu_set_tag(&app->ntag);
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }

        break;
    }

    case AMIIBO_DETAIL_MENU_BACK_AMIIBO_DETAIL: {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }

    case AMIIBO_DETAIL_MENU_AUTO_RAND_UID: {
        if (!amiibo_helper_is_key_loaded()) {
            amiibo_scene_amiibo_detail_no_key_msg(app);
            return;
        }
        settings_data_t *p_settings = settings_get_data();
        p_settings->auto_gen_amiibo = !p_settings->auto_gen_amiibo;
        settings_save();

        mui_list_view_item_set_sub_text(p_item, (p_settings->auto_gen_amiibo ? getLangString(_L_ON_F) : getLangString(_L_OFF_F)));
    } break;

    case AMIIBO_DETAIL_MENU_REMOVE_AMIIBO: {
        mui_msg_box_set_header(app->p_msg_box, _T(DELETE_TAG));
        sprintf(msg, _T(DELETE_TAG_CONFIRM), string_get_cstr(app->current_file));
        mui_msg_box_set_message(app->p_msg_box, msg);
        mui_msg_box_set_btn_text(app->p_msg_box, _T(CONFIRM), NULL, _T(CANCEL));
        mui_msg_box_set_btn_focus(app->p_msg_box, 2);
        mui_msg_box_set_event_cb(app->p_msg_box, amiibo_scene_amiibo_detail_delete_tag_confirmed);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);
        break;
    }

    case AMIIBO_DETAIL_MENU_BACK_MAIN_MENU:
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIBO);
        break;
    }
}

void amiibo_scene_amiibo_detail_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe1c5, getLangString(_L_RANDOM_GENERATION),
                           (void *)AMIIBO_DETAIL_MENU_RAND_UID);
    settings_data_t *p_settings = settings_get_data();

    mui_list_view_add_item_ext(app->p_list_view, 0xe1c6, getLangString(_L_AUTO_RANDOM_GENERATION), 
     (p_settings->auto_gen_amiibo ? getLangString(_L_ON_F) : getLangString(_L_OFF_F)), 
     (void *)AMIIBO_DETAIL_MENU_AUTO_RAND_UID);
    mui_list_view_add_item(app->p_list_view, 0xe1c7, getLangString(_L_DELETE_TAG),
                           (void *)AMIIBO_DETAIL_MENU_REMOVE_AMIIBO);
    mui_list_view_add_item(app->p_list_view, 0xe068, getLangString(_L_BACK_TO_DETAILS),
                           (void *)AMIIBO_DETAIL_MENU_BACK_AMIIBO_DETAIL);
    mui_list_view_add_item(app->p_list_view, 0xe069, getLangString(_L_BACK_TO_FILE_LIST),
                           (void *)AMIIBO_DETAIL_MENU_BACK_FILE_BROWSER);
    mui_list_view_add_item(app->p_list_view, 0xe1c8, getLangString(_L_BACK_TO_MAIN_MENU),
                           (void *)AMIIBO_DETAIL_MENU_BACK_MAIN_MENU);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_amiibo_detail_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_amiibo_detail_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}