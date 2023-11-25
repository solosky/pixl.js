#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "ntag_emu.h"
#include "ntag_store.h"
#include "settings.h"

#include "mui_icons.h"

#include "amiidb_api_db.h"
#include "amiidb_api_fav.h"
#include "amiidb_api_slot.h"

APP_TIMER_DEF(m_amiibo_gen_delay_timer);

static void ntag_update_cb(ntag_event_type_t type, void *context, ntag_t *p_ntag) {

    app_amiidb_t *app = context;

    if (type == NTAG_EVENT_TYPE_WRITTEN) {
        memcpy(&app->ntag, p_ntag->data, sizeof(ntag_t));
        if (app->prev_scene_id == AMIIDB_SCENE_DATA_LIST) {
            uint8_t focus = amiibo_view_get_focus(app->p_amiibo_view);
            amiidb_api_slot_write(focus, &app->ntag);
        }
        mui_update(mui());
    } else if (type == NTAG_EVENT_TYPE_READ) {
        settings_data_t *p_settings = settings_get_data();
        if (p_settings->auto_gen_amiibo && amiibo_helper_is_key_loaded()) {
            app_timer_stop(m_amiibo_gen_delay_timer);
            app_timer_start(m_amiibo_gen_delay_timer, APP_TIMER_TICKS(1000), app);
        }
    }
}

static void amiidb_scene_amiibo_game_list_generate(app_amiidb_t *app) {
    const db_amiibo_t *p_amiibo = app->cur_amiibo;
    amiibo_helper_generate_amiibo(p_amiibo->head, p_amiibo->tail, &app->ntag);
    ntag_emu_set_tag(&app->ntag);
}

static void amiidb_scene_amiibo_game_list_init(app_amiidb_t *app) {
    // total count and focus
    uint8_t total = 0;
    uint8_t focus = -1;
    const db_link_t *p_link = link_list;
    uint8_t cur_game_id = app->game_id_path[app->game_id_index];
    while (p_link->game_id > 0) {
        if (p_link->game_id == cur_game_id) {
            total++;
            if (p_link->head == app->cur_amiibo->head && p_link->tail == app->cur_amiibo->tail) {
                focus = total - 1;
            }
        }
        p_link++;
    }

    amiidb_scene_amiibo_game_list_generate(app);

    amiibo_view_set_max_ntags(app->p_amiibo_view, total);
    amiibo_view_set_focus(app->p_amiibo_view, focus >= 0 ? focus : 0);
    amiibo_view_set_game_id(app->p_amiibo_view, cur_game_id);
}

static void amiidb_scene_amiibo_game_list_update(app_amiidb_t *app) {
    uint8_t focus = amiibo_view_get_focus(app->p_amiibo_view);

    const db_link_t *p_link = link_list;
    uint8_t index = -1;
    uint8_t cur_game_id = app->game_id_path[app->game_id_index];
    while (p_link->game_id > 0) {
        if (p_link->game_id == cur_game_id) {
            index++;
            if (index == focus) {
                app->cur_amiibo = get_amiibo_by_id(p_link->head, p_link->tail);
                break;
            }
        }
        p_link++;
    }
    amiidb_scene_amiibo_game_list_generate(app);
}

static void amiidb_scene_amiibo_data_list_init(app_amiidb_t *app) {
    if (amiidb_api_slot_read(app->cur_slot_index, &app->ntag) < 0) {
        ntag_store_new_rand(&app->ntag);
    }
    ntag_emu_set_tag(&app->ntag);

    settings_data_t *p_settings_data = settings_get_data();
    amiibo_view_set_max_ntags(app->p_amiibo_view, p_settings_data->amiidb_data_slot_num);
    amiibo_view_set_focus(app->p_amiibo_view, app->cur_slot_index);
    amiibo_view_set_game_id(app->p_amiibo_view, -1); // no game id
}

static void amiidb_scene_amiibo_data_list_update(app_amiidb_t *app) {
    uint8_t focus = amiibo_view_get_focus(app->p_amiibo_view);
    if (amiidb_api_slot_read(focus, &app->ntag) < 0) {
        ntag_store_new_rand(&app->ntag);
    }
    ntag_emu_set_tag(&app->ntag);
}

static void amiidb_scene_amiibo_detail_fav_list_read_cb(amiidb_fav_info_t *p_info, void *ctx) {
    app_amiidb_t *app = ctx;
    if (p_info->fav_type == FAV_TYPE_FAV) {
        amiidb_fav_t *p_fav = amiidb_fav_array_push_new(app->fav_array);
        memcpy(p_fav, &p_info->fav_data.fav, sizeof(amiidb_fav_t));
    }
}

static void amiidb_scene_amiibo_fav_list_init(app_amiidb_t *app) {
    amiidb_api_fav_list_dir(string_get_cstr(app->cur_fav_dir), amiidb_scene_amiibo_detail_fav_list_read_cb, app);
    amiibo_view_set_max_ntags(app->p_amiibo_view, amiidb_fav_array_size(app->fav_array));
    amiibo_view_set_focus(app->p_amiibo_view, 0);

    // init data
    for (uint8_t i = 0; i < amiidb_fav_array_size(app->fav_array); i++) {
        amiidb_fav_t *p_fav = amiidb_fav_array_get(app->fav_array, i);
        if (p_fav->amiibo_head == app->cur_fav.amiibo_head && p_fav->amiibo_tail == app->cur_fav.amiibo_tail &&
            p_fav->game_id == app->cur_fav.game_id) {
            app->cur_amiibo = get_amiibo_by_id(p_fav->amiibo_head, p_fav->amiibo_tail);
            amiibo_view_set_game_id(app->p_amiibo_view, p_fav->game_id); // no game id
            amiidb_scene_amiibo_game_list_generate(app);
            amiibo_view_set_focus(app->p_amiibo_view, i);
            break;
        }
    }
}

static void amiidb_scene_amiibo_fav_list_update(app_amiidb_t *app) {
    uint8_t focus = amiibo_view_get_focus(app->p_amiibo_view);
    amiidb_fav_t *p_fav = amiidb_fav_array_get(app->fav_array, focus);
    app->cur_amiibo = get_amiibo_by_id(p_fav->amiibo_head, p_fav->amiibo_tail);
    amiibo_view_set_game_id(app->p_amiibo_view, p_fav->game_id); // no game id
    amiidb_scene_amiibo_game_list_generate(app);
}

static void amiidb_scene_amiibo_search_cb(const db_amiibo_t *p_amiibo, void *ctx) {
    app_amiidb_t *app = ctx;
    if (amiidb_fav_array_size(app->fav_array) < LIST_VIEW_ITEM_MAX_COUNT) {
        amiidb_fav_t *p_fav = amiidb_fav_array_push_new(app->fav_array);
        p_fav->amiibo_head = p_amiibo->head;
        p_fav->amiibo_tail = p_amiibo->tail;
        p_fav->game_id = -1; // no game id
    }
}

static void amiidb_scene_amiibo_search_list_init(app_amiidb_t *app) {
    amiidb_api_db_search(string_get_cstr(app->search_str), amiidb_scene_amiibo_search_cb, app);
    amiibo_view_set_max_ntags(app->p_amiibo_view, amiidb_fav_array_size(app->fav_array));
    amiibo_view_set_focus(app->p_amiibo_view, 0);

    // init data
    for (uint8_t i = 0; i < amiidb_fav_array_size(app->fav_array); i++) {
        amiidb_fav_t *p_fav = amiidb_fav_array_get(app->fav_array, i);
        if (p_fav->amiibo_head == app->cur_amiibo->head && p_fav->amiibo_tail == app->cur_amiibo->tail) {
            app->cur_amiibo = get_amiibo_by_id(p_fav->amiibo_head, p_fav->amiibo_tail);
            amiibo_view_set_game_id(app->p_amiibo_view, p_fav->game_id); // no game id
            amiidb_scene_amiibo_game_list_generate(app);
            amiibo_view_set_focus(app->p_amiibo_view, i);
            break;
        }
    }
}

static void amiidb_scene_amiibo_search_update(app_amiidb_t *app) { amiidb_scene_amiibo_fav_list_update(app); }

static void ntag_generate_timer_handler(void *p_context) {
    app_amiidb_t *app = p_context;
    if (app->prev_scene_id == AMIIDB_SCENE_GAME_LIST || app->prev_scene_id == AMIIDB_SCENE_FAV_LIST) {
        amiidb_scene_amiibo_game_list_generate(app);
    }
    mui_update(mui());
}

static void amiidb_scene_amiibo_view_on_event(amiibo_view_event_t event, amiibo_view_t *p_view) {
    app_amiidb_t *app = p_view->user_data;
    if (event == AMIIBO_VIEW_EVENT_MENU) {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL_MENU);
    } else if (event == AMIIBO_VIEW_EVENT_UPDATE) {
        if (app->prev_scene_id == AMIIDB_SCENE_GAME_LIST) {
            amiidb_scene_amiibo_game_list_update(app);
        } else if (app->prev_scene_id == AMIIDB_SCENE_FAV_LIST) {
            amiidb_scene_amiibo_fav_list_update(app);
        } else if (app->prev_scene_id == AMIIDB_SCENE_DATA_LIST) {
            amiidb_scene_amiibo_data_list_update(app);
        } else if (app->prev_scene_id == AMIIDB_SCENE_AMIIBO_SEARCH) {
            amiidb_scene_amiibo_search_update(app);
        }
    }
}
static void amiidb_scene_amiibo_detail_menu_msg_box_no_key_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiidb_t *app = p_msg_box->user_data;
    if (event == MUI_MSG_BOX_EVENT_SELECT_CENTER) {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    }
}

static void amiidb_scene_amiibo_detail_no_key_msg(app_amiidb_t *app) {
    mui_msg_box_set_header(app->p_msg_box, getLangString(_L_AMIIBO_KEY_UNLOADED));
    mui_msg_box_set_message(app->p_msg_box, getLangString(_L_UPLOAD_KEY_RETAIL_BIN));
    mui_msg_box_set_btn_text(app->p_msg_box, NULL, getLangString(_L_KNOW), NULL);
    mui_msg_box_set_btn_focus(app->p_msg_box, 1);
    mui_msg_box_set_event_cb(app->p_msg_box, amiidb_scene_amiibo_detail_menu_msg_box_no_key_cb);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_MSG_BOX);
}

void amiidb_scene_amiibo_detail_on_enter(void *user_data) {
    app_amiidb_t *app = user_data;
    amiibo_view_set_ntag(app->p_amiibo_view, &app->ntag);
    amiibo_view_set_event_cb(app->p_amiibo_view, amiidb_scene_amiibo_view_on_event);

    if (!amiibo_helper_is_key_loaded() && app->prev_scene_id != AMIIDB_SCENE_DATA_LIST) {
        amiidb_scene_amiibo_detail_no_key_msg(app);
        return;
    }

    if (app->prev_scene_id == AMIIDB_SCENE_GAME_LIST) {
        amiidb_scene_amiibo_game_list_init(app);
    } else if (app->prev_scene_id == AMIIDB_SCENE_FAV_LIST) {
        amiidb_scene_amiibo_fav_list_init(app);
    } else if (app->prev_scene_id == AMIIDB_SCENE_DATA_LIST) {
        amiidb_scene_amiibo_data_list_init(app);
    } else if (app->prev_scene_id == AMIIDB_SCENE_AMIIBO_SEARCH) {
        amiidb_scene_amiibo_search_list_init(app);
    }

    ntag_emu_set_update_cb(ntag_update_cb, app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_DETAIL);
    int32_t err_code =
        app_timer_create(&m_amiibo_gen_delay_timer, APP_TIMER_MODE_SINGLE_SHOT, ntag_generate_timer_handler);
    APP_ERROR_CHECK(err_code);
}

void amiidb_scene_amiibo_detail_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    app_timer_stop(m_amiibo_gen_delay_timer);
    amiidb_fav_array_reset(app->fav_array);
}