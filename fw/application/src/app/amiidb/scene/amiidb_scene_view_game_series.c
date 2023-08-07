#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"

#include "mui_icons.h"

typedef enum {
    AMIIDB_VIEW_GAME_SERIES_MENU_BACK,
}amiidb_view_game_series_menu_t;

static void amiidb_scene_view_game_series_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                    mui_list_item_t *p_item){
    amiidb_view_game_series_menu_t menu = (amiidb_view_game_series_menu_t) p_item->user_data;
    app_amiidb_t * app = mui_list_view_get_user_data(p_list_view);
    switch(menu){
    case AMIIDB_VIEW_GAME_SERIES_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}


void amiidb_scene_view_game_series_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *) user_data;
    mui_list_view_add_item(app->p_list_view, ICON_EXIT, "[返回]", (void*) AMIIDB_VIEW_GAME_SERIES_MENU_BACK);
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_view_game_series_list_view_on_selected);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_view_game_series_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *) user_data;
    mui_list_view_clear_items(app->p_list_view);
}