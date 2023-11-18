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

#define CHAMELEON_MENU_BACK_EXIT 0
#define CHAMELEON_MENU_BACK_MAIN 1
#define CHAMELEON_MENU_MODE 2
#define CHAMELEON_MENU_VER 3
#define CHAMELEON_MENU_AUTO_GENERATE 4

void chameleon_scene_menu_card_slot_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                             mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    char buff[16];
    switch (p_item->icon) {
    case ICON_DATA: {
        int32_t slot = mui_list_view_get_focus(p_list_view) - 1;
        tag_emulation_slot_set_enable(slot, TAG_SENSE_HF, !tag_emulation_slot_is_enabled(slot, TAG_SENSE_HF));
        sprintf(buff, "[%s]", tag_emulation_slot_is_enabled(slot, TAG_SENSE_HF) ? "开" : "关");
        mui_list_view_item_set_sub_text(p_item, buff);
    } break;

    case ICON_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void chameleon_scene_menu_card_slot_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    char buff[32];
    char buff2[32];
    sprintf(buff, "[%0d]", TAG_MAX_SLOT_NUM);

    mui_list_view_add_item_ext(app->p_list_view, ICON_VIEW, "卡槽数量..", buff, (void *)CHAMELEON_MENU_BACK_EXIT);

   
    for (uint32_t i = 0; i < TAG_MAX_SLOT_NUM; i++) {
        sprintf(buff, "[%s]", tag_emulation_slot_is_enabled(i, TAG_SENSE_HF) ? "开" : "关");
        sprintf(buff2, "卡槽 %02d", i + 1);
        mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, buff2, buff, (void *)CHAMELEON_MENU_BACK_EXIT);
    }
    mui_list_view_add_item(app->p_list_view, ICON_BACK, getLangString(_L_MAIN_RETURN),
                           (void *)CHAMELEON_MENU_BACK_MAIN);

    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_card_slot_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_card_slot_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}