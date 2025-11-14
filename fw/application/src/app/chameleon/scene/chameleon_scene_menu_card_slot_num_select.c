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

void chameleon_scene_menu_card_slot_num_select_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                    mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    switch (p_item->icon) {
    case ICON_DATA: {
        tag_helper_set_slot_num((uint8_t)p_item->user_data);
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;

    case ICON_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void chameleon_scene_menu_card_slot_num_select_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    // 只添加 8、20、50 三个选项
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "8", NULL, (void *)8);
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "20", NULL, (void *)20);
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "50", NULL, (void *)50);

    mui_list_view_add_item(app->p_list_view, ICON_BACK, _T(MAIN_RETURN), NULL_USER_DATA);

    // 设置当前选择的选项
    settings_data_t *settings = settings_get_data();
    // 需要根据设置的值设置当前选中的选项
    if (settings->chameleon_slot_num == 8) {
        mui_list_view_set_focus(app->p_list_view, 0);
    } else if (settings->chameleon_slot_num == 20) {
        mui_list_view_set_focus(app->p_list_view, 1);
    } else if (settings->chameleon_slot_num == 50) {
        mui_list_view_set_focus(app->p_list_view, 2);
    }else{
        mui_list_view_set_focus(app->p_list_view, 0);
    }
    mui_list_view_set_scroll_offset(app->p_list_view, 0);
    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_card_slot_num_select_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_card_slot_num_select_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}