#include "amiibo_helper.h"
#include "amiidb_api_fav.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"

#include "db_header.h"
#include "mui_icons.h"
#include "settings.h"
#include "vfs.h"
#include <math.h>

static void amiidb_scene_fav_list_reload(app_amiidb_t *app);

static void amiidb_scene_fav_list_text_input_event_cb(mui_text_input_event_t event, mui_text_input_t *p_text_input) {
    app_amiidb_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *text = mui_text_input_get_input_text(p_text_input);
        if (strlen(text) > 0) {
            int32_t res = amiidb_api_fav_create_dir(text);
            if (res == VFS_OK) {
                string_reset(app->cur_fav_dir);
                amiidb_scene_fav_list_reload(app);
            }
        }
    }
}

static void amiidb_scene_fav_list_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                        mui_list_item_t *p_item) {
    uint16_t icon = p_item->icon;
    app_amiidb_t *app = mui_list_view_get_user_data(p_list_view);
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        switch (icon) {
        case ICON_EXIT:
            if (string_size(app->cur_fav_dir) == 0) {
                app->in_fav_folders = true;
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_MAIN);
            } else {
                string_reset(app->cur_fav_dir);
                app->in_fav_folders = false;
                amiidb_scene_fav_list_reload(app);
            }
            break;

        case ICON_FILE: {
            amiidb_fav_t *p_fav_item = p_item->user_data;
            memcpy(&app->cur_fav, p_fav_item, sizeof(amiidb_fav_t));
            app->prev_scene_id = AMIIDB_SCENE_FAV_LIST;
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL);
        } break;

        case ICON_FOLDER: {
            string_set(app->cur_fav_dir, p_item->text);
            app->in_fav_folders = false;
            amiidb_scene_fav_list_reload(app);
        } break;

        case ICON_NEW: {
            mui_text_input_set_header(app->p_text_input, getLangString(_L_APP_AMIIDB_FAV_NEW_HEAD));
            mui_text_input_set_event_cb(app->p_text_input, amiidb_scene_fav_list_text_input_event_cb);
            mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_INPUT);
        } break;
        }
    } else if (event == MUI_LIST_VIEW_EVENT_LONG_SELECTED) {
        if (icon == ICON_FILE) {
            amiidb_fav_t *p_fav_item = p_item->user_data;
            memcpy(&app->cur_fav, p_fav_item, sizeof(amiidb_fav_t));
            app->in_fav_folders = false;
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_FAV_LIST_MENU);
        } else if (icon == ICON_FOLDER) {
            memset(&app->cur_fav, 0, sizeof(amiidb_fav_t));
            string_set(app->cur_fav_dir, p_item->text);
            app->in_fav_folders = true;
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_FAV_LIST_MENU);
        }
    }
}

static int amiidb_scene_fav_list_list_view_sort_cb(const mui_list_item_t *p_item_a, const mui_list_item_t *p_item_b) {
    if (p_item_a->icon == ICON_FOLDER && p_item_b->icon == ICON_FOLDER) {
        db_game_t *p_game_a = (db_game_t *)p_item_a->user_data;
        db_game_t *p_game_b = (db_game_t *)p_item_b->user_data;
        return p_game_b->order - p_game_a->order;
    } else if (p_item_a->icon == ICON_FOLDER && p_item_b->icon == ICON_FILE) {
        return -1;
    } else {
        amiidb_fav_t *p_amiibo_a = (amiidb_fav_t *)p_item_a->user_data;
        amiidb_fav_t *p_amiibo_b = (amiidb_fav_t *)p_item_b->user_data;
        return p_amiibo_a->amiibo_head < p_amiibo_b->amiibo_head;
    }
}

static void amiidb_scene_fav_list_read_cb(amiidb_fav_info_t *p_info, void *ctx) {

    app_amiidb_t *app = ctx;
    settings_data_t *p_settings_data = settings_get_data();
    char txt[64];

    if (p_info->fav_type == FAV_TYPE_FOLDER) {
        mui_list_view_add_item(app->p_list_view, ICON_FOLDER, p_info->fav_data.folder_name, 0);
    } else {

        amiidb_fav_t *p_fav = mui_mem_malloc(sizeof(amiidb_fav_t));
        memcpy(p_fav, &p_info->fav_data.fav, sizeof(amiidb_fav_t));

        const db_amiibo_t *p_amiibo = get_amiibo_by_id(p_fav->amiibo_head, p_fav->amiibo_tail);
        if (p_amiibo) {
            const char *name = p_settings_data->language == LANGUAGE_ZH_HANS ? p_amiibo->name_cn : p_amiibo->name_en;
            mui_list_view_add_item(app->p_list_view, ICON_FILE, name, p_fav);
        } else {
            sprintf(txt, "Amiibo[%08x:%08x]", p_fav->amiibo_head, p_fav->amiibo_tail);
            mui_list_view_add_item(app->p_list_view, ICON_FILE, txt, p_fav);
        }
    }
}

static void amiidb_scene_fav_list_reload(app_amiidb_t *app) {
    // clear list view
    mui_list_view_clear_items(app->p_list_view);

    amiidb_api_fav_list_dir(string_get_cstr(app->cur_fav_dir), amiidb_scene_fav_list_read_cb, app);
    mui_list_view_sort(app->p_list_view, amiidb_scene_fav_list_list_view_sort_cb);

    if (mui_list_view_item_size(app->p_list_view) == 0 && app->in_fav_folders) {
        mui_list_view_add_item(app->p_list_view, ICON_NEW, getLangString(_L_APP_AMIIDB_NEW), (void *)0);
    }

    mui_list_view_add_item(app->p_list_view, ICON_EXIT, getLangString(_L_APP_AMIIDB_BACK), (void *)0);
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_fav_list_list_view_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_fav_list_item_clear_cb(mui_list_item_t *p_item) {
    if (p_item->icon == ICON_FILE) {
        amiidb_fav_t *p_fav_item = (amiidb_fav_t *)p_item->user_data;
        mui_mem_free(p_fav_item);
    }
}

void amiidb_scene_fav_list_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    amiidb_scene_fav_list_reload(app);
}

void amiidb_scene_fav_list_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    mui_list_view_clear_items_with_cb(app->p_list_view, amiidb_scene_fav_list_item_clear_cb);
}