#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"

#include "amiidb_api_db.h"

#include "db_header.h"
#include "mui_icons.h"
#include "settings.h"
#include "i18n/language.h"
#include <math.h>


static void amiidb_scene_amiibo_search_reload(app_amiidb_t *app);

static void amiidb_scene_amiibo_search_text_input_event_cb(mui_text_input_event_t event,
                                                           mui_text_input_t *p_text_input) {
    app_amiidb_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        amiidb_scene_amiibo_search_reload(app);
    }
}

static void amiidb_msg_box_error_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiidb_t *app = p_msg_box->user_data;
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

static void amiidb_show_message(app_amiidb_t *app, const char *msg) {
    mui_msg_box_set_header(app->p_msg_box, getLangString(_L_ERR));
    mui_msg_box_set_message(app->p_msg_box, msg);
    mui_msg_box_set_btn_text(app->p_msg_box, NULL, getLangString(_L_KNOW), NULL);
    mui_msg_box_set_btn_focus(app->p_msg_box, 1);
    mui_msg_box_set_event_cb(app->p_msg_box, amiidb_msg_box_error_cb);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_MSG_BOX);
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
        app->prev_scene_id = AMIIDB_SCENE_AMIIBO_SEARCH;
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL);
    } break;

    case ICON_SEARCH: {
        mui_text_input_set_header(app->p_text_input, getLangString(_L_APP_AMIIDB_SEARCH_HEAD));
        mui_text_input_set_event_cb(app->p_text_input, amiidb_scene_amiibo_search_text_input_event_cb);
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_INPUT);
    } break;

    case ICON_ERROR: {
        amiidb_show_message(app, _T(APP_AMIIDB_MORE_MESSAGE));
    } break;
    }
}

static uint16_t amiibo_hit_total = 0;
static void amiidb_scene_amiibo_search_cb(const db_amiibo_t *p_amiibo, void *ctx) {
    settings_data_t *p_settings_data = settings_get_data();
    app_amiidb_t *app = ctx;
    if (amiibo_hit_total < LIST_VIEW_ITEM_MAX_COUNT) {
        const char *name = get_amiibo_display_name(p_amiibo);
        mui_list_view_add_item(app->p_list_view, ICON_FILE, name, (void *)p_amiibo);
    }
    amiibo_hit_total++;
}

static void amiidb_scene_amiibo_search_reload(app_amiidb_t *app) {
    char txt[64];
    // clear list view
    mui_list_view_clear_items(app->p_list_view);

    const char *text = mui_text_input_get_input_text(app->p_text_input);

    sprintf(txt, "%s %s", getLangString(_L_APP_AMIIDB_SEARCH_HEAD), text);
    mui_list_view_add_item(app->p_list_view, ICON_SEARCH, txt, (void *)0);
    string_set_str(app->search_str, text);

    // do search now!
    amiibo_hit_total = 0;
    amiidb_api_db_search(text, amiidb_scene_amiibo_search_cb, app);

    // update stats
    if(strlen(text) > 0 ) {
        sprintf(txt, "%s %s (%d)",getLangString(_L_APP_AMIIDB_SEARCH_HEAD), text, amiibo_hit_total);
        mui_list_view_set_item(app->p_list_view, 0, ICON_SEARCH, txt, (void *)0);
    }

    if (amiibo_hit_total > LIST_VIEW_ITEM_MAX_COUNT) {
        mui_list_view_add_item(app->p_list_view, ICON_ERROR, getLangString(_L_APP_AMIIDB_MORE), (void *)0);
    }

    mui_list_view_add_item(app->p_list_view, ICON_EXIT, getLangString(_L_APP_AMIIDB_BACK), (void *)0);
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