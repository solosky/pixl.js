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

void chameleon_scene_menu_card_advanced_id_size_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                    mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    switch (p_item->icon) {
    case ICON_DATA: {
        nfc_tag_14a_uid_size uid_size = (nfc_tag_14a_uid_size)p_item->user_data;
        nfc_tag_14a_coll_res_reference_t *coll_res = tag_helper_get_active_coll_res_ref();
        *(coll_res->size) = uid_size;
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;

    case ICON_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void chameleon_scene_menu_card_advanced_id_size_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;
    char buff[32];
    
    sprintf(buff, "%d %s", NFC_TAG_14A_UID_SINGLE_SIZE, _T(BYTES));
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, buff, NULL, (void*)NFC_TAG_14A_UID_SINGLE_SIZE);
    sprintf(buff, "%d %s", NFC_TAG_14A_UID_DOUBLE_SIZE, _T(BYTES));
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, buff, NULL, (void*)NFC_TAG_14A_UID_DOUBLE_SIZE);
    sprintf(buff, "%d %s", NFC_TAG_14A_UID_TRIPLE_SIZE, _T(BYTES));
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, buff, NULL, (void*)NFC_TAG_14A_UID_TRIPLE_SIZE);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, _T(MAIN_RETURN), NULL_USER_DATA);

    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_card_advanced_id_size_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_card_advanced_id_size_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}