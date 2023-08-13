#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "amiibo_detail_view.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "settings.h"
#include "ntag_emu.h"

#include "mui_icons.h"

APP_TIMER_DEF(m_amiibo_gen_delay_timer);

static void ntag_random(void *p_context) {
    app_amiidb_t *app = p_context;
    ret_code_t err_code;
    ntag_t *ntag_current = &app->ntag;

    err_code = amiibo_helper_rand_amiibo_uuid(ntag_current);
    if (err_code == NRF_SUCCESS) {
//        ntag_emu_set_tag(&app->ntag);
        mui_update(mui());
    }
}

static void ntag_reload(app_amiidb_t* app){
    db_amiibo_t *p_amiibo = app->cur_amiibo;
    amiibo_helper_generate_amiibo(p_amiibo->head, p_amiibo->tail, &app->ntag);
    ntag_emu_set_tag(&app->ntag);
}


static void amiidb_scene_amiibo_detail_view_on_event(amiibo_detail_view_event_t event, amiibo_detail_view_t *p_view) {
    app_amiidb_t *app = p_view->user_data;
    if (event == AMIIBO_DETAIL_VIEW_EVENT_MENU) {
        //mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_DETAIL_MENU);
    } else if (event == AMIIBO_DETAIL_VIEW_EVENT_UPDATE) {
        uint8_t focus = amiibo_detail_view_get_focus(app->p_detail_view);

        const db_link_t *p_link = link_list;
        uint8_t index = 0;
        while (p_link->game_id > 0) {
            if(p_link->game_id == app->cur_game_id){
                index++;
                if(index == focus){
                    app->cur_amiibo = get_amiibo_by_id(p_link->head, p_link->tail);
                    break;
                }
            }
            p_link++;
        }

        ntag_reload(app);
    }
}


void amiidb_scene_amiibo_detail_on_enter(void *user_data) {
    app_amiidb_t *app = user_data;
    amiibo_detail_view_set_ntag(app->p_detail_view, &app->ntag);
    amiibo_detail_view_set_event_cb(app->p_detail_view, amiidb_scene_amiibo_detail_view_on_event);

    //total count and focus
    uint8_t total = 0;
    uint8_t focus = 0;
    const db_link_t *p_link = link_list;
    while (p_link->game_id > 0) {
        if(p_link->game_id == app->cur_game_id){
            total++;
            if(p_link->head == app->cur_amiibo->head && p_link->tail == app->cur_amiibo->tail){
                focus = total;
            }
        }
        p_link++;
    }

    ntag_reload(app);

    amiibo_detail_view_set_max_ntags(app->p_detail_view, total);
    amiibo_detail_view_set_focus(app->p_detail_view, focus);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_DETAIL);
    int32_t err_code = app_timer_create(&m_amiibo_gen_delay_timer, APP_TIMER_MODE_SINGLE_SHOT, ntag_random);
    APP_ERROR_CHECK(err_code);
}

void amiidb_scene_amiibo_detail_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
}