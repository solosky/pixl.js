#include "amiibo_helper.h"
#include "amiidb_api_slot.h"
#include "amiidb_scene.h"
#include "app_amiibo.h"
#include "app_amiidb.h"
#include "i18n/language.h"
#include "mui_icons.h"

void amiidb_scene_data_list_reload(app_amiidb_t *app);

static void amiidb_scene_data_select_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                           mui_list_item_t *p_item) {
    uint16_t icon = p_item->icon;
    app_amiidb_t *app = mui_list_view_get_user_data(p_list_view);
    switch (icon) {
    case ICON_EXIT: {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;

    case ICON_AMIIBO:
    case ICON_FILE: {
        uint32_t index = mui_list_view_get_focus(p_list_view);
        int32_t res = amiidb_api_slot_write(index, &app->ntag);
        mui_toast_view_show(app->p_toast_view,
                            getLangString(res < 0 ? _L_APP_AMIIDB_SLOT_SAVE_FAILED : _L_APP_AMIIDB_SLOT_SAVE_SUCCESS));
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL);
    } break;
    }
}

static void amiidb_scene_data_select_setup(app_amiidb_t *app) {
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_data_select_list_view_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_data_select_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    amiidb_scene_data_list_reload(app);
    amiidb_scene_data_select_setup(app);
}

void amiidb_scene_data_select_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    mui_list_view_clear_items(app->p_list_view);
}