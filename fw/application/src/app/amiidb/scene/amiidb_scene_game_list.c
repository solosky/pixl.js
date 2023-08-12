#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"

#include "mui_icons.h"
#include "db_header.h"

typedef enum {
    AMIIDB_GAME_LIST_MENU_BACK,
}amiidb_game_list_menu_t;

static void amiidb_scene_game_list_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                    mui_list_item_t *p_item){
    amiidb_game_list_menu_t menu = (amiidb_game_list_menu_t) p_item->user_data;
    app_amiidb_t * app = mui_list_view_get_user_data(p_list_view);
    switch(menu){
    case AMIIDB_GAME_LIST_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

static int amiidb_scene_game_list_list_view_sort_cb(const mui_list_item_t* p_item_a, const mui_list_item_t* p_item_b){
    db_game_t* p_game_a = (db_game_t*) p_item_a->user_data;
    db_game_t* p_game_b = (db_game_t*) p_item_b->user_data;
    return p_game_b->order - p_game_a->order;
}


void amiidb_scene_game_list_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *) user_data;
    const db_game_t* p_game = game_list;
    while(p_game->game_id > 0){
        mui_list_view_add_item(app->p_list_view, ICON_FOLDER, p_game->name_cn, (void*) p_game);
        p_game++;
    }

    mui_list_view_sort(app->p_list_view, amiidb_scene_game_list_list_view_sort_cb);
    mui_list_view_add_item(app->p_list_view, ICON_EXIT, "[返回]", (void*) AMIIDB_GAME_LIST_MENU_BACK);
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_game_list_list_view_on_selected);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_game_list_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *) user_data;
    mui_list_view_clear_items(app->p_list_view);
}