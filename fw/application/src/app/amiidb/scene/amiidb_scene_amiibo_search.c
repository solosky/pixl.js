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

static void amiidb_scene_amiibo_search_reload(app_amiidb_t *app);
extern char *strcasestr(const char *, const char *);

static void amiidb_scene_amiibo_search_text_input_event_cb(mui_text_input_event_t event,
                                                           mui_text_input_t *p_text_input) {
    app_amiidb_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        amiidb_scene_amiibo_search_reload(app);
    }
}

static void amiidb_scene_amiibo_search_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                             mui_list_item_t *p_item) {
    uint16_t icon = p_item->icon;
    app_amiidb_t *app = mui_list_view_get_user_data(p_list_view);
    switch (icon) {
    case ICON_EXIT:
        if (app->game_id_index <= 0) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_MAIN);
        } else {
            app->game_id_index--;
            amiidb_scene_amiibo_search_reload(app);
        }
        break;

    case ICON_FILE: {
        const db_amiibo_t *p_amiibo = p_item->user_data;
        app->cur_amiibo = p_amiibo;
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL);
    } break;

    case ICON_SEARCH: {
        mui_text_input_set_header(app->p_text_input, "搜索:");
        mui_text_input_set_event_cb(app->p_text_input, amiidb_scene_amiibo_search_text_input_event_cb);
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_INPUT);
    } break;
    }
}

static int amiidb_scene_amiibo_search_list_view_sort_cb(const mui_list_item_t *p_item_a,
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

static void amiidb_scene_amiibo_search_reload(app_amiidb_t *app) {
    settings_data_t *p_settings_data = settings_get_data();
    char txt[64];

    // clear list view
    mui_list_view_clear_items(app->p_list_view);

    const char *text = mui_text_input_get_input_text(app->p_text_input);
    sprintf(txt, "搜索: %s", text);
    mui_list_view_add_item(app->p_list_view, ICON_SEARCH, txt, (void *)0);

    // add link list by page
    const db_amiibo_t *p_amiibo = amiibo_list;
    uint16_t add_cnt = 0;
    uint16_t hit_cnt = 0;
    while (p_amiibo->name_en != 0) {
        if (strlen(text) > 0 && strcasestr(p_amiibo->name_en, text) != NULL) {
            hit_cnt++;
            if (add_cnt < LINK_MAX_DISPLAY_CNT) {
                const char *name =
                    p_settings_data->language == LANGUAGE_ZH_HANS ? p_amiibo->name_cn : p_amiibo->name_en;
                mui_list_view_add_item(app->p_list_view, ICON_FILE, name, (void *)p_amiibo);
            }
            add_cnt++;
        }
        p_amiibo++;
    }

    if (add_cnt > LINK_MAX_DISPLAY_CNT) {
        mui_list_view_add_item(app->p_list_view, ICON_ERROR, "[更多..]", (void *)0);
    }

    mui_list_view_add_item(app->p_list_view, ICON_EXIT, "[返回]", (void *)0);
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_amiibo_search_list_view_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_amiibo_search_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    amiidb_scene_amiibo_search_reload(app);
}

void amiidb_scene_amiibo_search_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    mui_list_view_clear_items(app->p_list_view);
}