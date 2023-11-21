#include "app_chameleon.h"
#include "chameleon_scene.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "amiibo_helper.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "i18n/language.h"
#include "settings.h"

#include "mui_icons.h"
#include "tag_helper.h"

#include "fds_utils.h"

typedef enum {
    CHAMELEON_MENU_BACK,
    CHAMELEON_MENU_FILE,
} chameleon_menu_item_t;

static const char *get_id_edit_title(app_chameleon_id_edit_type_t id_edit_type) {
    switch (id_edit_type) {
    case APP_CHAMELEON_ID_EDIT_TYPE_UID:
        return "UID:";

    case APP_CHAMELEON_ID_EDIT_TYPE_SAK:
        return "SAK:";

    case APP_CHAMELEON_ID_EDIT_TYPE_ATQA:
        return "ATQA:";

    default:
        return "";
    }
}

static void format_id_edit_text(app_chameleon_id_edit_type_t id_edit_type, char *text) {
    const nfc_tag_14a_coll_res_reference_t *coll_res = tag_helper_get_active_coll_res_ref();
    switch (id_edit_type) {
    case APP_CHAMELEON_ID_EDIT_TYPE_UID:
        sprintf(text, "%02x_%02x_%02x_%02x", coll_res->uid[0], coll_res->uid[1], coll_res->uid[2], coll_res->uid[3]);
        return;

    case APP_CHAMELEON_ID_EDIT_TYPE_SAK:
        sprintf(text, "%02x", coll_res->sak[0]);
        return;

    case APP_CHAMELEON_ID_EDIT_TYPE_ATQA:
        sprintf(text, "%02x_%02x", coll_res->atqa[1], coll_res->atqa[0]);
        return;
    }
}

static bool handle_id_edit_cb(app_chameleon_t *app, const char *text) {
    app_chameleon_id_edit_type_t id_edit_type = app->id_edit_type;
    const nfc_tag_14a_coll_res_reference_t *coll_res = tag_helper_get_active_coll_res_ref();
    int8_t buff[4];
    switch (id_edit_type) {
    case APP_CHAMELEON_ID_EDIT_TYPE_UID:
        if (sscanf(text, "%02x_%02x_%02x_%02x", buff, buff + 1, buff + 2, buff + 3) == 4) {
            coll_res->uid[0] = buff[0];
            coll_res->uid[1] = buff[1];
            coll_res->uid[2] = buff[2];
            coll_res->uid[3] = buff[3];
            return true;
        }
        break;

    case APP_CHAMELEON_ID_EDIT_TYPE_SAK:
        if (sscanf(text, "%02x", buff) == 1) {
            coll_res->sak[0] = buff[0];
            return true;
        }
        break;

    case APP_CHAMELEON_ID_EDIT_TYPE_ATQA:
        if (sscanf(text, "%02x_%02x", buff, buff + 1) == 2) {
            coll_res->atqa[1] = buff[0];
            coll_res->atqa[0] = buff[1];
            return true;
        }
        break;
    }

    return false;
}

static void text_input_event_cb(mui_text_input_event_t event, mui_text_input_t *p_text_input) {
    app_chameleon_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *text = mui_text_input_get_input_text(p_text_input);
        if (strlen(text) > 0) {
            if (handle_id_edit_cb(app, text)) {
                mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
            } else {
                mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_ADV_ID_EDIT_INVALID_INPUT));
            }
        } else {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    }
}

void chameleon_scene_menu_card_advanced_id_edit_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;
    char text[32];

    format_id_edit_text(app->id_edit_type, text);

    mui_text_input_set_header(app->p_text_input, get_id_edit_title(app->id_edit_type));
    mui_text_input_set_input_text(app->p_text_input, text);
    mui_text_input_set_event_cb(app->p_text_input, text_input_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_TEXT_INPUT);
}

void chameleon_scene_menu_card_advanced_id_edit_on_exit(void *user_data) { app_chameleon_t *app = user_data; }