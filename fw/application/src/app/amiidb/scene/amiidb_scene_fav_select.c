#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"

#include "amiidb_api_fav.h"
#include "db_header.h"
#include "i18n/language.h"
#include "mui_icons.h"
#include "settings.h"
#include "vfs.h"
#include <math.h>

static void amiidb_scene_fav_select_reload(app_amiidb_t *app);

static void amiidb_scene_fav_select_msg_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiidb_t *app = p_msg_box->user_data;
    mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL);
}

static void amiidb_scene_fav_select_msg_show(app_amiidb_t *app, const char *message) {
    mui_msg_box_set_header(app->p_msg_box, getLangString(_L_APP_AMIIDB_TIPS));
    mui_msg_box_set_message(app->p_msg_box, message);
    mui_msg_box_set_btn_text(app->p_msg_box, NULL, getLangString(_L_APP_AMIIDB_CONFIRM), NULL);
    mui_msg_box_set_btn_focus(app->p_msg_box, 1);
    mui_msg_box_set_event_cb(app->p_msg_box, amiidb_scene_fav_select_msg_cb);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_MSG_BOX);
}

static void amiidb_scene_fav_select_text_input_event_cb(mui_text_input_event_t event, mui_text_input_t *p_text_input) {
    app_amiidb_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *text = mui_text_input_get_input_text(p_text_input);
        if (strlen(text) > 0) {
            int32_t res = amiidb_api_fav_create_dir(text);
            if (res == VFS_OK) {
                amiidb_scene_fav_select_reload(app);
            }
        }
    }
}

static void amiidb_scene_fav_select_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                          mui_list_item_t *p_item) {
    uint16_t icon = p_item->icon;
    app_amiidb_t *app = mui_list_view_get_user_data(p_list_view);

    switch (icon) {
    case ICON_EXIT:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;

    case ICON_NEW: {
        mui_text_input_set_header(app->p_text_input, getLangString(_L_APP_AMIIDB_FAV_NEW_HEAD));
        mui_text_input_set_event_cb(app->p_text_input, amiidb_scene_fav_select_text_input_event_cb);
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_INPUT);
    } break;

    case ICON_FOLDER: {
        uint8_t cur_game_id = app->game_id_path[app->game_id_index];
        const db_amiibo_t *p_amiibo = app->cur_amiibo;
        amiidb_fav_t fav = {.game_id = cur_game_id, .amiibo_head = p_amiibo->head, .amiibo_tail = p_amiibo->tail};
        int res = amiidb_api_fav_add(string_get_cstr(p_item->text), &fav);
        if (res == VFS_OK) {
            amiidb_scene_fav_select_msg_show(app, getLangString(_L_APP_AMIIDB_FAV_SUCCESS));
        } else {
            amiidb_scene_fav_select_msg_show(app, getLangString(_L_APP_AMIIDB_FAV_FAILED));
        }
    } break;
    }
}

static void amiidb_scene_fav_select_read_cb(amiidb_fav_info_t *p_info, void *ctx) {
    app_amiidb_t *app = ctx;
    if (p_info->fav_type == FAV_TYPE_FOLDER) {
        mui_list_view_add_item(app->p_list_view, ICON_FOLDER, p_info->fav_data.folder_name, 0);
    }
}

static void amiidb_scene_fav_select_reload(app_amiidb_t *app) {
    // clear list view
    mui_list_view_clear_items(app->p_list_view);

    mui_list_view_add_item(app->p_list_view, ICON_NEW, getLangString(_L_APP_AMIIDB_NEW), (void *)0);

    amiidb_api_fav_list_dir("", amiidb_scene_fav_select_read_cb, app);

    mui_list_view_add_item(app->p_list_view, ICON_EXIT, getLangString(_L_APP_AMIIDB_BACK), (void *)0);
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_fav_select_list_view_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    // no fav folder, first create one
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