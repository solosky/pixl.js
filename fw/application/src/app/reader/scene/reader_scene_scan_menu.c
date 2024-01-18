#include "reader_scene.h"

#include "app_reader.h"
#include "nfc_scan_view.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "mui_icons.h"
#include "i18n/language.h"

void reader_scene_scan_menu_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                     mui_list_item_t *p_item) {
    app_reader_t *app = p_list_view->user_data;
    int32_t icon = p_item->icon;
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        switch(icon){
            case ICON_HOME:
            mini_app_launcher_exit(mini_app_launcher());
            break;
        }
    }
}

void reader_scene_scan_menu_on_enter(void *user_data) {
    app_reader_t *app = user_data;


    mui_list_view_add_item(app->p_list_view, ICON_HOME, getLangString(_L_MAIN_MENU), NULL_USER_DATA);

    mui_list_view_set_selected_cb(app->p_list_view, reader_scene_scan_menu_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, READER_VIEW_ID_LIST);
}

void reader_scene_scan_menu_on_exit(void *user_data) {
    app_reader_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}