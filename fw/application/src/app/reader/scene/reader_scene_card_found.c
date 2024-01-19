#include "reader_scene.h"

#include "app_reader.h"
#include "nfc_scan_view.h"

#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "mui_icons.h"

void reader_scene_card_found_menu_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                           mui_list_item_t *p_item) {
    app_reader_t *app = p_list_view->user_data;
    int32_t icon = p_item->icon;
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        switch (icon) {
        case ICON_BACK:
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
            break;
        }
    }
}
void reader_scene_card_found_on_enter(void *user_data) {
    app_reader_t *app = user_data;
    uint8_t buff[64];

    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "发现卡片", buff, NULL_USER_DATA);

    strcpy(buff, "[");
    tag_helper_format_uid(buff + 1, app->tag_info.uid, app->tag_info.uid_len);
    strcat(buff, "]");
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "UID", buff, NULL_USER_DATA);

    sprintf(buff, "[%02X]", app->tag_info.sak);
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "SAK", buff, NULL_USER_DATA);

    sprintf(buff, "[%02X %02X]", app->tag_info.atqa[1], app->tag_info.atqa[0]);
    mui_list_view_add_item_ext(app->p_list_view, ICON_DATA, "ATQA", buff, NULL_USER_DATA);

    mui_list_view_add_item(app->p_list_view, ICON_BACK, _T(MAIN_RETURN), NULL_USER_DATA);

    mui_list_view_set_selected_cb(app->p_list_view, reader_scene_card_found_menu_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, READER_VIEW_ID_LIST);
}

void reader_scene_card_found_on_exit(void *user_data) {
    app_reader_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
}