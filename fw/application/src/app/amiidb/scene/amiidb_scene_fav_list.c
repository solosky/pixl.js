#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"

#include "db_header.h"
#include "mui_icons.h"
#include "settings.h"
#include <math.h>

#define LINK_MAX_DISPLAY_CNT 100

static void amiidb_scene_fav_list_reload(app_amiidb_t *app);

static void amiidb_scene_fav_list_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                             mui_list_item_t *p_item) {
    uint16_t icon = p_item->icon;
    app_amiidb_t *app = mui_list_view_get_user_data(p_list_view);
    switch (icon) {
    case ICON_EXIT:
        if (app->game_id_index <= 0) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_MAIN);
        } else {
            app->game_id_index--;
            amiidb_scene_fav_list_reload(app);
        }
        break;

    case ICON_FILE: {
        const db_amiibo_t *p_amiibo = p_item->user_data;
        app->cur_amiibo = p_amiibo;
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL);
    } break;

    }
}

static int amiidb_scene_fav_list_list_view_sort_cb(const mui_list_item_t *p_item_a,
                                                        const mui_list_item_t *p_item_b) {
    if (p_item_a->icon == ICON_FOLDER && p_item_b->icon == ICON_FOLDER) {
        db_game_t *p_game_a = (db_game_t *)p_item_a->user_data;
        db_game_t *p_game_b = (db_game_t *)p_item_b->user_data;
        return p_game_b->order - p_game_a->order;
    } else if (p_item_a->icon == ICON_FOLDER && p_item_b->icon == ICON_FILE) {
        return -1;
    } else {
        db_amiibo_t *p_amiibo_a = (db_amiibo_t *)p_item_a->user_data;
        db_amiibo_t *p_amiibo_b = (db_amiibo_t *)p_item_b->user_data;
        return strcmp(p_amiibo_a->name_en, p_amiibo_b->name_en);
    }
}

static void amiidb_scene_fav_list_reload(app_amiidb_t *app) {
    settings_data_t *p_settings_data = settings_get_data();
    char txt[64];

    // clear list view
    mui_list_view_clear_items(app->p_list_view);

    //TODO scan fav folder

    mui_list_view_add_item(app->p_list_view, ICON_EXIT, "[返回]", (void *)0);
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_fav_list_list_view_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_fav_list_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    amiidb_scene_fav_list_reload(app);
}

void amiidb_scene_fav_list_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    mui_list_view_clear_items(app->p_list_view);
}