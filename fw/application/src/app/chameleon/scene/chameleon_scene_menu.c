#include "app_chameleon.h"
#include "chameleon_scene.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "amiibo_helper.h"
#include "nrf_log.h"

#include "i18n/language.h"
#include "settings.h"

#include "mui_icons.h"

#include "tag_helper.h"

typedef enum {
    CHAMELEON_MENU_HOME,
    CHAMELEON_MENU_BACK,
    CHAMELEON_MENU_SLOT,
    CHAMELEON_MENU_CARD_DATA,
    CHAMELEON_MENU_CARD_TYPE,
    CHAMELEON_MENU_CARD_ADVANCED,
} chameleon_menu_item_t;

void chameleon_scene_menu_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view, mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    chameleon_menu_item_t item = (chameleon_menu_item_t)p_item->user_data;
    switch (item) {
    case CHAMELEON_MENU_HOME:
        mini_app_launcher_exit(mini_app_launcher());
        break;

    case CHAMELEON_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;

    case CHAMELEON_MENU_SLOT:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_SLOT);
        break;

    case CHAMELEON_MENU_CARD_DATA:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_DATA);
        break;

    case CHAMELEON_MENU_CARD_ADVANCED:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_ADVANCED);
        break;

    case CHAMELEON_MENU_CARD_TYPE:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, CHAMELEON_SCENE_MENU_CARD_TYPE);
        break;
    }
}

void chameleon_scene_menu_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    char buff[64];
    uint8_t slot = tag_emulation_get_slot();
    tag_specific_type_t tag_type = tag_helper_get_active_tag_type();
    const tag_specific_type_name_t *tag_name = tag_helper_get_tag_type_name(tag_type);
    const nfc_tag_14a_coll_res_reference_t *coll_res = tag_helper_get_active_coll_res_ref();

    sprintf(buff, "[%02d]", slot + 1);
    mui_list_view_add_item_ext(app->p_list_view, ICON_VIEW, "当前卡槽", buff, (void *)-1);
    mui_list_view_add_item_ext(app->p_list_view, ICON_KEY, "卡名", "我的家1", (void *)-1);

    tag_helper_format_uid(buff, coll_res->uid, *(coll_res->size));
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "ID", buff, (void *)-1);

    sprintf(buff, "[%s]", tag_name->long_name);
    mui_list_view_add_item_ext(app->p_list_view, ICON_FAVORITE, "卡类型", buff, (void *)CHAMELEON_MENU_CARD_TYPE);

    mui_list_view_add_item(app->p_list_view, ICON_FILE, "卡数据..", (void *)CHAMELEON_MENU_CARD_DATA);
    mui_list_view_add_item(app->p_list_view, ICON_PAGE, "卡高级设置..", (void *)CHAMELEON_MENU_CARD_ADVANCED);
    mui_list_view_add_item(app->p_list_view, ICON_SLOT, "卡槽管理..", (void *)CHAMELEON_MENU_SLOT);
    mui_list_view_add_item(app->p_list_view, ICON_SETTINGS, "全局设置..", (void *)-1);

    mui_list_view_add_item(app->p_list_view, ICON_BACK, getLangString(_L_TAG_DETAILS), (void *)CHAMELEON_MENU_BACK);
    mui_list_view_add_item(app->p_list_view, ICON_HOME, getLangString(_L_MAIN_MENU), (void *)CHAMELEON_MENU_HOME);

    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}