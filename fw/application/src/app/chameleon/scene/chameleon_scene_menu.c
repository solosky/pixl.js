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

void chameleon_scene_menu_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view, mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    switch (p_item->icon) {
    case ICON_HOME:
        mini_app_launcher_exit(mini_app_launcher());
        break;

    case ICON_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;

    case ICON_SLOT:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_SLOT);
        break;

    case ICON_FILE:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_DATA);
        break;

    case ICON_PAGE:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_ADVANCED);
        break;
    }
}

void chameleon_scene_menu_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    tag_specific_type_t tag_type[2];
    char buff[64];
    uint8_t slot = tag_emulation_get_slot();

    tag_emulation_get_specific_type_by_slot(slot, tag_type);
    const tag_specific_type_name_t *tag_name = tag_helper_get_tag_type_name(tag_type[0]);
    const nfc_tag_14a_coll_res_reference_t *coll_res = tag_helper_get_tag_type_coll_res_entity(tag_type[0]);

    sprintf(buff, "[%02d]", slot + 1);
    mui_list_view_add_item_ext(app->p_list_view, ICON_VIEW, "当前卡槽", buff, (void *)CHAMELEON_MENU_BACK_EXIT);

    if (*(coll_res->size) == NFC_TAG_14A_UID_SINGLE_SIZE) {
        sprintf(buff, "%02x:%02x:%02x:%02x", slot, coll_res->uid[0], coll_res->uid[1], coll_res->uid[2],
                coll_res->uid[3]);
    } else if (*(coll_res->size) == NFC_TAG_14A_UID_DOUBLE_SIZE) {
        sprintf(buff, "%02x:%02x:%02x:%02x:%02x:%02x:%02x", slot, coll_res->uid[0], coll_res->uid[1],
                coll_res->uid[2], coll_res->uid[3], coll_res->uid[4], coll_res->uid[5], coll_res->uid[6]);
    } else {
        sprintf(buff, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", slot, coll_res->uid[0],
                coll_res->uid[1], coll_res->uid[2], coll_res->uid[3], coll_res->uid[4], coll_res->uid[5],
                coll_res->uid[6], coll_res->uid[7], coll_res->uid[8], coll_res->uid[9]);
    }
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "ID", buff, (void *)CHAMELEON_MENU_BACK_EXIT);

    sprintf(buff, "[%s]", tag_name->long_name);
    mui_list_view_add_item_ext(app->p_list_view, ICON_FAVORITE, "卡类型", buff, (void *)CHAMELEON_MENU_BACK_EXIT);

    mui_list_view_add_item(app->p_list_view, ICON_FILE, "卡数据..", (void *)CHAMELEON_MENU_BACK_EXIT);
    mui_list_view_add_item(app->p_list_view, ICON_PAGE, "卡高级设置..", (void *)CHAMELEON_MENU_BACK_EXIT);
    mui_list_view_add_item(app->p_list_view, ICON_SLOT, "卡槽管理..", (void *)CHAMELEON_MENU_BACK_EXIT);
    mui_list_view_add_item(app->p_list_view, ICON_SETTINGS, "全局设置..", (void *)CHAMELEON_MENU_BACK_EXIT);

    mui_list_view_add_item(app->p_list_view, ICON_BACK, getLangString(_L_TAG_DETAILS),
                           (void *)CHAMELEON_MENU_BACK_MAIN);

    mui_list_view_add_item(app->p_list_view, ICON_HOME, getLangString(_L_MAIN_MENU), (void *)CHAMELEON_MENU_BACK_EXIT);

    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}