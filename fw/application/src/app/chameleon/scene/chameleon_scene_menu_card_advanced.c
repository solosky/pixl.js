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

#define CHAMELEON_MENU_BACK_EXIT 0
#define CHAMELEON_MENU_BACK_MAIN 1
#define CHAMELEON_MENU_MODE 2
#define CHAMELEON_MENU_VER 3
#define CHAMELEON_MENU_AUTO_GENERATE 4


void chameleon_scene_menu_card_advanced_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view, mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    switch (p_item->icon) {
    case ICON_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void chameleon_scene_menu_card_advanced_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "ID", "[00:11:22:AA:BB:CC]", (void *)CHAMELEON_MENU_BACK_EXIT);
    mui_list_view_add_item_ext(app->p_list_view, ICON_FAVORITE, "SAK", "[00]", (void *)CHAMELEON_MENU_BACK_EXIT);
    mui_list_view_add_item_ext(app->p_list_view, ICON_FILE, "ATQA", "[44 00]", (void *)CHAMELEON_MENU_BACK_EXIT);
    mui_list_view_add_item_ext(app->p_list_view, ICON_PAGE, "Gen1A模式", "[开]", (void *)CHAMELEON_MENU_BACK_EXIT);
    mui_list_view_add_item_ext(app->p_list_view, ICON_PAGE, "Gen2模式", "[开]", (void *)CHAMELEON_MENU_BACK_EXIT);
    mui_list_view_add_item_ext(app->p_list_view, ICON_PAGE, "写入模式", "[写入]", (void *)CHAMELEON_MENU_BACK_EXIT);
    mui_list_view_add_item(app->p_list_view, ICON_VIEW, "加载0扇区..", (void *)CHAMELEON_MENU_BACK_EXIT);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, getLangString(_L_MAIN_RETURN), (void *)CHAMELEON_MENU_BACK_MAIN);
    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_card_advanced_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_card_advanced_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}