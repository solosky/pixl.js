#include "app_chameleon.h"
#include "chameleon_scene.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "amiibo_helper.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "i18n/language.h"
#include "settings2.h"

#include "mui_icons.h"
#include "tag_helper.h"

typedef enum {
    CHAMELEON_MENU_CUSTOM,
    CHAMELEON_MENU_UID,
    CHAMELEON_MENU_SAK,
    CHAMELEON_MENU_ATQA,
    CHAMELEON_MENU_GEN1A,
    CHAMELEON_MENU_GEN2,
    CHAMELEON_MENU_GEN_UID,
    CHAMELEON_MENU_WRITE_MODE,
    CHAMELEON_MENU_BACK,
} chameleon_menu_item_t;

void chameleon_scene_menu_card_advanced_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                 mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    chameleon_menu_item_t item = (chameleon_menu_item_t)p_item->user_data;
    switch (item) {

    case CHAMELEON_MENU_CUSTOM: {
        nfc_tag_mf1_set_use_mf1_coll_res(!nfc_tag_mf1_is_use_mf1_coll_res());
        chameleon_scene_menu_card_advanced_reload(app);
        break;
    }

    case CHAMELEON_MENU_UID: {
        if (!nfc_tag_mf1_is_use_mf1_coll_res()) {
            app->id_edit_type = APP_CHAMELEON_ID_EDIT_TYPE_UID;
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_ADVANCED_ID_EDIT);
        }
    } break;

    case CHAMELEON_MENU_SAK: {
        if (!nfc_tag_mf1_is_use_mf1_coll_res()) {
            app->id_edit_type = APP_CHAMELEON_ID_EDIT_TYPE_SAK;
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_ADVANCED_ID_EDIT);
        }
    } break;

    case CHAMELEON_MENU_ATQA: {
        if (!nfc_tag_mf1_is_use_mf1_coll_res()) {
            app->id_edit_type = APP_CHAMELEON_ID_EDIT_TYPE_ATQA;
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_ADVANCED_ID_EDIT);
        }
    } break;

    case CHAMELEON_MENU_GEN_UID: {

        tag_helper_generate_uid();
        mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_GENERATE_UID_SUCCESS));

        chameleon_scene_menu_card_advanced_reload(app);

    } break;

    case CHAMELEON_MENU_GEN1A: {
        nfc_tag_mf1_set_gen1a_magic_mode(!nfc_tag_mf1_is_gen1a_magic_mode());
        chameleon_scene_menu_card_advanced_reload(app);
    } break;

    case CHAMELEON_MENU_GEN2: {
        nfc_tag_mf1_set_gen2_magic_mode(!nfc_tag_mf1_is_gen2_magic_mode());
        chameleon_scene_menu_card_advanced_reload(app);
    } break;

    case CHAMELEON_MENU_WRITE_MODE: {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_WRITE_MODE_SELECT);
    } break;

    case CHAMELEON_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void chameleon_scene_menu_card_advanced_reload(app_chameleon_t *app) {
    char buff[64];

    uint16_t focus = mui_list_view_get_focus(app->p_list_view);
    uint16_t scroll_offset = mui_list_view_get_scroll_offset(app->p_list_view);
    mui_list_view_clear_items(app->p_list_view);

    tag_specific_type_t tag_type = tag_helper_get_active_tag_type();
    tag_group_type_t tag_group = tag_helper_get_tag_group_type(tag_type);
    const tag_specific_type_name_t *tag_name = tag_helper_get_tag_type_name(tag_type);
    const nfc_tag_14a_coll_res_reference_t *coll_res = tag_helper_get_active_coll_res_ref();

    if (tag_group == TAG_GROUP_MIFLARE) {
        mui_list_view_add_item_ext(app->p_list_view, ICON_VIEW, _T(APP_CHAMELEON_CARD_ADV_CUSTOM_MODE),
                                   (nfc_tag_mf1_is_use_mf1_coll_res() ? _T(OFF_F) : _T(ON_F)), CHAMELEON_MENU_CUSTOM);
    }

    strcpy(buff, "[");
    tag_helper_format_uid(buff + 1, coll_res->uid, *(coll_res->size));
    strcat(buff, "]");
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "ID", buff, CHAMELEON_MENU_UID);

    sprintf(buff, "[%02X]", coll_res->sak[0]);
    mui_list_view_add_item_ext(app->p_list_view, ICON_FAVORITE, "SAK", buff, CHAMELEON_MENU_SAK);

    sprintf(buff, "[%02X %02X]", coll_res->atqa[1], coll_res->atqa[0]);
    mui_list_view_add_item_ext(app->p_list_view, ICON_FILE, "ATQA", buff, CHAMELEON_MENU_ATQA);

    if (tag_group == TAG_GROUP_MIFLARE) {
        mui_list_view_add_item_ext(app->p_list_view, ICON_PAGE, _T(APP_CHAMELEON_CARD_GEN1A_MODE),
                                   (nfc_tag_mf1_is_gen1a_magic_mode() ? _T(ON_F) : _T(OFF_F)), CHAMELEON_MENU_GEN1A);
        mui_list_view_add_item_ext(app->p_list_view, ICON_PAGE, _T(APP_CHAMELEON_CARD_GEN2_MODE),
                                   (nfc_tag_mf1_is_gen2_magic_mode() ? _T(ON_F) : _T(OFF_F)), CHAMELEON_MENU_GEN2);

        sprintf(buff, "[%s]", tag_helper_get_mf_write_mode_name(nfc_tag_mf1_get_write_mode()));
        mui_list_view_add_item_ext(app->p_list_view, ICON_PAGE, _T(APP_CHAMELEON_CARD_WRITE_MODE), buff,
                                   CHAMELEON_MENU_WRITE_MODE);
    } else if (tag_group == TAG_GROUP_NTAG) {
        mui_list_view_add_item(app->p_list_view, ICON_PAGE, _T(APP_CHAMELEON_CARD_GENERATE_UID),
                               CHAMELEON_MENU_GEN_UID);
    }

    mui_list_view_add_item(app->p_list_view, ICON_BACK, getLangString(_L_MAIN_RETURN), (void *)CHAMELEON_MENU_BACK);

    mui_list_view_set_focus(app->p_list_view, focus);
    mui_list_view_set_scroll_offset(app->p_list_view, scroll_offset);
}

void chameleon_scene_menu_card_advanced_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;
    chameleon_scene_menu_card_advanced_reload(app);
    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_card_advanced_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_card_advanced_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}