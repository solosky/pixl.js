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

void chameleon_scene_menu_card_slot_select_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                    mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    switch (p_item->icon) {
    case ICON_DATA: {
        uint8_t slot = (uint8_t)p_item->user_data;
        tag_emulation_change_slot(slot, false);
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;

    case ICON_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void chameleon_scene_menu_card_slot_select_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    char buff[32];

    for (uint32_t i = 0; i < TAG_MAX_SLOT_NUM; i++) {
        if (tag_emulation_slot_is_enabled(i, TAG_SENSE_HF)) {
            sprintf(buff, "%s %02d", _T(APP_CHAMELEON_CARD_SLOT), i + 1);
            mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, buff, NULL, i);
        }
    }
    mui_list_view_add_item(app->p_list_view, ICON_BACK, _T(MAIN_RETURN), NULL_USER_DATA);

    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_card_slot_select_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_card_slot_select_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}