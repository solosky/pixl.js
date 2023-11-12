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
#include "tag_emulation.h"

typedef enum {
    CHAMELEON_MENU_BACK,
    CHAMELEON_MENU_LOAD_DATA,
    CHAMELEON_MENU_FACTORY,
} chameleon_menu_item_t;

void chameleon_scene_menu_card_data_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                             mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
     chameleon_menu_item_t item = (chameleon_menu_item_t)p_item->user_data;
    switch (item) {
    case CHAMELEON_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    case CHAMELEON_MENU_FACTORY: {
        uint8_t slot = tag_emulation_get_slot();
        tag_emulation_factory_data(slot, tag_helper_get_active_tag_type());
        tag_emulation_save_data();
        mui_toast_view_show(app->p_toast_view, "卡片初始化成功");
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;
    case CHAMELEON_MENU_LOAD_DATA:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void chameleon_scene_menu_card_data_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, ICON_VIEW, "从文件加载..", (void *)CHAMELEON_MENU_LOAD_DATA);
    mui_list_view_add_item(app->p_list_view, ICON_DATA, "重置默认数据..", (void *)CHAMELEON_MENU_FACTORY);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, getLangString(_L_MAIN_RETURN), (void *)CHAMELEON_MENU_BACK);

    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_card_data_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_card_data_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}