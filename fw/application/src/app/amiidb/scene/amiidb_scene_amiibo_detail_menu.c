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

#include "amiidb_api_slot.h"

static enum amiidb_detail_menu_t {
    AMIIDB_DETAIL_MENU_RAND_UID,
    AMIIDB_DETAIL_MENU_AUTO_RAND_UID,
    AMIIDB_DETAIL_MENU_READ_ONLY,
    AMIIDB_DETAIL_MENU_SHOW_QRCODE,
    AMIIDB_DETAIL_MENU_FAVORITE,
    AMIIDB_DETAIL_MENU_SAVE_AS,
    AMIIDB_DETAIL_MENU_BACK_AMIIBO_DETAIL,
    AMIIDB_DETAIL_MENU_BACK_FILE_BROWSER,
    AMIIDB_DETAIL_MENU_BACK_MAIN_MENU,
};

static void amiidb_scene_amiibo_detail_menu_msg_box_no_key_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiidb_t *app = p_msg_box->user_data;
    if (event == MUI_MSG_BOX_EVENT_SELECT_CENTER) {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    }
}

static void amiidb_scene_amiibo_detail_no_key_msg(app_amiidb_t *app) {
    mui_msg_box_set_header(app->p_msg_box, getLangString(_L_AMIIBO_KEY_UNLOADED));
    mui_msg_box_set_message(app->p_msg_box, getLangString(_L_UPLOAD_KEY_RETAIL_BIN));
    mui_msg_box_set_btn_text(app->p_msg_box, NULL, getLangString(_L_KNOW), NULL);
    mui_msg_box_set_btn_focus(app->p_msg_box, 1);
    mui_msg_box_set_event_cb(app->p_msg_box, amiidb_scene_amiibo_detail_menu_msg_box_no_key_cb);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_MSG_BOX);
}

static void amiidb_scene_amiibo_detail_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                        mui_list_item_t *p_item) {
    app_amiidb_t *app = p_list_view->user_data;

    uint32_t selection = (uint32_t)p_item->user_data;

    switch (selection) {
    case AMIIDB_DETAIL_MENU_BACK_FILE_BROWSER:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, app->prev_scene_id);
        break;
    case AMIIDB_DETAIL_MENU_RAND_UID: {
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
            amiidb_scene_amiibo_detail_no_key_msg(app);
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

    case AMIIDB_DETAIL_MENU_READ_ONLY: {
        ret_code_t err_code = amiidb_api_slot_set_readonly(app->cur_slot_index, !app->ntag.read_only);
        if (err_code == NRF_SUCCESS) {
            app->ntag.read_only = !app->ntag.read_only;
            mui_list_view_item_set_sub_text(p_item,
                                            app->ntag.read_only ? getLangString(_L_ON_F) : getLangString(_L_OFF_F));
        }
    } break;

    case AMIIDB_DETAIL_MENU_BACK_AMIIBO_DETAIL: {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }

    case AMIIDB_DETAIL_MENU_AUTO_RAND_UID: {
        if (!amiibo_helper_is_key_loaded()) {
            amiidb_scene_amiibo_detail_no_key_msg(app);
            return;
        }
        char txt[32];
        settings_data_t *p_settings = settings_get_data();
        p_settings->auto_gen_amiibo = !p_settings->auto_gen_amiibo;
        snprintf(txt, sizeof(txt), "%s",
                 p_settings->auto_gen_amiibo ? getLangString(_L_ON_F) : getLangString(_L_OFF_F));
        settings_save();

        string_set_str(p_item->sub_text, txt);

        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;

    case AMIIDB_DETAIL_MENU_SHOW_QRCODE: {
        char txt[32];
        settings_data_t *p_settings = settings_get_data();
        p_settings->qrcode_enabled = !p_settings->qrcode_enabled;
        snprintf(txt, sizeof(txt), "%s", p_settings->qrcode_enabled ? getLangString(_L_ON_F) : getLangString(_L_OFF_F));
        settings_save();

        string_set_str(p_item->sub_text, txt);

        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;

    case AMIIDB_DETAIL_MENU_FAVORITE: {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_FAV_SELECT);
    } break;

    case AMIIDB_DETAIL_MENU_SAVE_AS: {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_DATA_SELECT);
    } break;

    case AMIIDB_DETAIL_MENU_BACK_MAIN_MENU:
        mini_app_launcher_exit(mini_app_launcher());
        break;
    }
}

void amiidb_scene_amiibo_detail_menu_on_enter(void *user_data) {
    app_amiidb_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe1c5, getLangString(_L_RANDOM_GENERATION),
                           (void *)AMIIDB_DETAIL_MENU_RAND_UID);

    char txt[32];
    settings_data_t *p_settings = settings_get_data();

    snprintf(txt, sizeof(txt), "%s", p_settings->auto_gen_amiibo ? getLangString(_L_ON_F) : getLangString(_L_OFF_F));
    mui_list_view_add_item_ext(app->p_list_view, 0xe1c6, getLangString(_L_AUTO_RANDOM_GENERATION), txt,
                               (void *)AMIIDB_DETAIL_MENU_AUTO_RAND_UID);

    snprintf(txt, sizeof(txt), "%s", p_settings->qrcode_enabled ? getLangString(_L_ON_F) : getLangString(_L_OFF_F));
    mui_list_view_add_item_ext(app->p_list_view, 0xe006, getLangString(_L_SHOW_QRCODE), txt,
                               (void *)AMIIDB_DETAIL_MENU_SHOW_QRCODE);

    if (app->prev_scene_id == AMIIDB_SCENE_DATA_LIST){
        mui_list_view_add_item_ext(app->p_list_view, 0xe007, getLangString(_L_READ_ONLY),
                               app->ntag.read_only ? getLangString(_L_ON_F) : getLangString(_L_OFF_F),
                               (void *)AMIIDB_DETAIL_MENU_READ_ONLY);
    }

    mui_list_view_add_item(app->p_list_view, ICON_FAVORITE, getLangString(_L_APP_AMIIDB_DETAIL_FAVORITE),
                           (void *)AMIIDB_DETAIL_MENU_FAVORITE);
    mui_list_view_add_item(app->p_list_view, ICON_DATA, getLangString(_L_APP_AMIIDB_DETAIL_SAVE_AS),
                           (void *)AMIIDB_DETAIL_MENU_SAVE_AS);
    mui_list_view_add_item(app->p_list_view, 0xe068, getLangString(_L_APP_AMIIDB_DETAIL_BACK_DETAIL),
                           (void *)AMIIDB_DETAIL_MENU_BACK_AMIIBO_DETAIL);
    mui_list_view_add_item(app->p_list_view, 0xe069, getLangString(_L_APP_AMIIDB_DETAIL_BACK_LIST),
                           (void *)AMIIDB_DETAIL_MENU_BACK_FILE_BROWSER);
    mui_list_view_add_item(app->p_list_view, 0xe1c8, getLangString(_L_APP_AMIIDB_EXIT),
                           (void *)AMIIDB_DETAIL_MENU_BACK_MAIN_MENU);

    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_amiibo_detail_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_amiibo_detail_menu_on_exit(void *user_data) {
    app_amiidb_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}
