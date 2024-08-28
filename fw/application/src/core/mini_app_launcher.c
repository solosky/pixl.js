
#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "settings2.h"
#include "nrf_power.h"
#include "tag_helper.h"

mini_app_launcher_t *mini_app_launcher() {
    static mini_app_launcher_t launcher;
    return &launcher;
}

static void mini_app_launcher_inst_kill(mini_app_launcher_t *p_launcher, mini_app_inst_t *p_app_inst) {
    p_app_inst->p_app->kill_cb(p_app_inst);
    p_app_inst->state = APP_STATE_KILLED;
}

static void mini_app_launcher_inst_run(mini_app_launcher_t *p_launcher, uint32_t id, uint8_t *retain_data) {
    mini_app_inst_t **pp_app_inst = mui_app_inst_dict_get(p_launcher->app_inst_dict, id);
    mini_app_inst_t *p_app_inst = NULL;
    if (pp_app_inst) {
        p_app_inst = *pp_app_inst;
    }
    if (p_app_inst) {
        p_app_inst->p_retain_data = NULL;
        p_app_inst->p_app->run_cb(p_app_inst);
    } else {
        p_app_inst = mui_mem_malloc(sizeof(mini_app_inst_t));
        p_app_inst->p_app = mini_app_registry_find_by_id(id);
        p_app_inst->state = APP_STATE_RUNNING;
        p_app_inst->p_retain_data = retain_data;
        mui_app_inst_dict_set_at(p_launcher->app_inst_dict, id, p_app_inst);
        p_app_inst->p_app->run_cb(p_app_inst);
    }
}

void mini_app_launcher_run_with_retain_data(mini_app_launcher_t *p_launcher, uint32_t id, uint8_t *retain_data) {
    mini_app_t *p_app = mini_app_registry_find_by_id(id);
    if (p_app == NULL) {
        return;
    }

    if (!p_app->deamon && p_launcher->p_main_app_inst) {
        mini_app_launcher_inst_kill(p_launcher, p_launcher->p_main_app_inst);
    }
    mini_app_launcher_inst_run(p_launcher, id, retain_data == NULL ? NULL : retain_data);

    if (!p_app->deamon) {
        p_launcher->p_main_app_inst = *mui_app_inst_dict_get(p_launcher->app_inst_dict, id);
    }
}

void mini_app_launcher_run(mini_app_launcher_t *p_launcher, uint32_t id) {
    mini_app_launcher_run_with_retain_data(p_launcher, id, NULL);
}

void mini_app_launcher_kill(mini_app_launcher_t *p_launcher, uint32_t id) {
    mini_app_inst_t *p_app_inst = *mui_app_inst_dict_get(p_launcher->app_inst_dict, id);
    if (p_app_inst) {
        mini_app_launcher_inst_kill(p_launcher, p_app_inst);
    }

    // run desktop if kill current app
    if (p_app_inst == p_launcher->p_main_app_inst) {
        mini_app_launcher_inst_run(p_launcher, MINI_APP_ID_DESKTOP, NULL);
        p_launcher->p_main_app_inst = *mui_app_inst_dict_get(p_launcher->app_inst_dict, MINI_APP_ID_DESKTOP);
    }
}

void mini_app_launcher_exit(mini_app_launcher_t* p_launcher){
    mini_app_launcher_kill(p_launcher, p_launcher->p_main_app_inst->p_app->id);
}

void mini_app_launcher_init(mini_app_launcher_t *p_launcher, uint32_t wakeup_reason) {

    mui_app_inst_dict_init(p_launcher->app_inst_dict);
    p_launcher->p_main_app_inst = NULL;

    mini_app_launcher_run(p_launcher, MINI_APP_ID_STATUS_BAR);

    cache_data_t *p_cache = cache_get_data();
    settings_data_t *p_settings = settings_get_data();
    
    NRF_LOG_INFO("wakeup reason: %d", wakeup_reason);

    if( (wakeup_reason & NRF_POWER_RESETREAS_NFC_MASK) && tag_helper_valid_default_slot()){
        mini_app_launcher_run_with_retain_data(p_launcher, MINI_APP_ID_CHAMELEON, NULL);
    }else if (p_cache->enabled == 1 && p_settings->hibernate_enabled == 1) {
        mini_app_launcher_run_with_retain_data(p_launcher, p_cache->id, p_cache->retain_data);
    } else {
        mini_app_launcher_run_with_retain_data(p_launcher, MINI_APP_ID_DESKTOP, NULL);
    }
}

void mini_app_launcher_sleep(mini_app_launcher_t *p_launcher) {
    mini_app_inst_t *app = p_launcher->p_main_app_inst;
    cache_data_t *p_cache = cache_get_data();
    if (app) {
        settings_data_t *p_settings = settings_get_data();
        NRF_LOG_INFO("running APP: %d %s %d", app->p_app->id, nrf_log_push(app->p_app->name),
                     app->p_app->hibernate_enabled);
        if (app->p_app->hibernate_enabled == 1 && p_settings->hibernate_enabled == 1) {
            p_cache->id = app->p_app->id;
            p_cache->enabled = true;
            app->p_retain_data = p_cache->retain_data;
            app->p_app->kill_cb(app);
        } else {
            p_cache->enabled = false;
            memset(p_cache->retain_data, 0, sizeof(p_cache->retain_data));
        }
    }
}