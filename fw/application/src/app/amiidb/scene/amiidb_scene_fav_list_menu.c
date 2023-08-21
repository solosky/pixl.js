#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "app_error.h"
#include "cwalk2.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "settings.h"
#include "vfs.h"

#include "amiidb_api_fav.h"
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
        int32_t res;
        if (string_size(app->cur_fav_dir) > 0) {
            amiidb_api_fav_empty_dir(string_get_cstr(app->cur_fav_dir));
        } else {
            // TODO ???
        }
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } else {
        // reload menu
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    }
}

static void amiidb_scene_fav_list_menu_msg_box_delete_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiidb_t *app = p_msg_box->user_data;
    if (event == MUI_MSG_BOX_EVENT_SELECT_LEFT) {
        // do delete
        int32_t res;
        if (app->cur_fav.amiibo_tail > 0 || app->cur_fav.amiibo_head > 0) {
            res = amiidb_api_fav_remove(string_get_cstr(app->cur_fav_dir), &app->cur_fav);
        } else {
            res = amiidb_api_fav_remove_dir(string_get_cstr(app->cur_fav_dir));
            string_reset(app->cur_fav_dir);
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
    mui_msg_box_set_btn_text(app->p_msg_box, getLangString(_L_APP_AMIIDB_CONFIRM), NULL,
                             getLangString(_L_APP_AMIIDB_CANCEL));
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
            int32_t res = amiidb_api_fav_create_dir(text);
            if (res == VFS_OK) {
                string_reset(app->cur_fav_dir);
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
        mui_text_input_set_header(app->p_text_input, getLangString(_L_APP_AMIIDB_FAV_NEW_HEAD));
        mui_text_input_set_event_cb(app->p_text_input, amiidb_scene_fav_list_menu_text_input_event_cb);
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_INPUT);
    } break;

    case ICON_EMPTY: {
        amiidb_scene_fav_list_menu_msg_box(app, getLangString(_L_APP_AMIIDB_TIPS),
                                           getLangString(_L_APP_AMIIDB_FAV_EMPTY_MSG),
                                           amiidb_scene_fav_list_menu_msg_box_empty_cb);
    } break;

    case ICON_DELETE: {
        amiidb_scene_fav_list_menu_msg_box(app, getLangString(_L_APP_AMIIDB_TIPS),
                                           getLangString(_L_APP_AMIIDB_FAV_DELETE_MSG),
                                           amiidb_scene_fav_list_menu_msg_box_delete_cb);
    } break;
    }
}

void amiidb_scene_fav_list_menu_on_enter(void *user_data) {
    app_amiidb_t *app = user_data;

    char txt[32];
    settings_data_t *p_settings = settings_get_data();
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_add_item(app->p_list_view, ICON_NEW, getLangString(_L_APP_AMIIDB_NEW), (void *)0);
    mui_list_view_add_item(app->p_list_view, ICON_EMPTY, getLangString(_L_APP_AMIIDB_EMPTY), (void *)0);
    mui_list_view_add_item(app->p_list_view, ICON_DELETE, getLangString(_L_APP_AMIIDB_DELETE), (void *)0);
    mui_list_view_add_item(app->p_list_view, ICON_EXIT, getLangString(_L_APP_AMIIDB_BACK), (void *)0);

    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_fav_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_fav_list_menu_on_exit(void *user_data) {
    app_amiidb_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}
