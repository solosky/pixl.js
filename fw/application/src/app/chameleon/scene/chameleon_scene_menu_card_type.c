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

typedef enum {
    CHAMELEON_MENU_BACK,
    CHAMELEON_MENU_CARD_TYPE,
} chameleon_menu_item_t;

void chameleon_scene_menu_card_type_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                             mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    chameleon_menu_item_t item = (chameleon_menu_item_t)p_item->user_data;
    switch (item) {
    case CHAMELEON_MENU_CARD_TYPE: {
        uint8_t slot = tag_emulation_get_slot();
        uint16_t idx = mui_list_view_get_focus(p_list_view);
        tag_emulation_change_type(slot, hf_tag_specific_types[idx]);
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;

    case CHAMELEON_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void chameleon_scene_menu_card_type_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    //0 is unkown...
    for (uint32_t i = 0; i < TAG_TYPE_HF_MAX; i++) {
        const tag_specific_type_name_t *tag_name = tag_helper_get_tag_type_name(hf_tag_specific_types[i]);
        mui_list_view_add_item(app->p_list_view, ICON_FILE, tag_name->long_name, (void *)CHAMELEON_MENU_CARD_TYPE);
    }

    mui_list_view_add_item(app->p_list_view, ICON_BACK, getLangString(_L_MAIN_RETURN), (void *)CHAMELEON_MENU_BACK);

    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_card_type_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_card_type_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}