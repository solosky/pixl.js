
#include "mini_app_launcher.h"
#include "mini_app_registry.h"

mini_app_launcher_t *mini_app_launcher() {
    static mini_app_launcher_t launcher;
    return &launcher;
}

static void mini_app_launcher_inst_kill(mini_app_launcher_t *p_launcher,
                                        mini_app_inst_t *p_app_inst) {
    p_app_inst->p_app->kill_cb(p_app_inst);
    p_app_inst->state = APP_STATE_KILLED;
    memset(p_app_inst->retain_data, 0, CACHEDATASIZE);
}

mini_app_inst_t *mini_app_get_this_run_inst(mini_app_launcher_t *p_launcher) {
    mini_app_inst_t *pp_app_inst = NULL;
    mui_app_inst_dict_it_t it;
    mui_app_inst_dict_it(it, p_launcher->app_inst_dict);
    while (!mui_app_inst_dict_end_p(it)) {
        mui_app_inst_dict_itref_t *it_ref = mui_app_inst_dict_ref(it);
        pp_app_inst = it_ref->value;
        if (pp_app_inst->state == APP_STATE_RUNNING && pp_app_inst->p_app->id != MINI_APP_ID_DESKTOP && pp_app_inst->p_app->id != MINI_APP_ID_STATUS_BAR) {
            return pp_app_inst;
        }
        mui_app_inst_dict_next(it);
    }
    return NULL;
}

static void mini_app_launcher_inst_run(mini_app_launcher_t *p_launcher, uint32_t id, uint8_t *retain_data) {
    mini_app_inst_t **pp_app_inst = mui_app_inst_dict_get(p_launcher->app_inst_dict, id);
    mini_app_inst_t *p_app_inst = NULL;
    if(pp_app_inst){
        p_app_inst = *pp_app_inst;
    }
    if (p_app_inst) {
        p_app_inst->p_app->run_cb(p_app_inst);
    } else {
        p_app_inst = mui_mem_malloc(sizeof(mini_app_inst_t));
        p_app_inst->p_app = mini_app_registry_find_by_id(id);
        p_app_inst->state = APP_STATE_RUNNING;
        mui_app_inst_dict_set_at(p_launcher->app_inst_dict, id, p_app_inst);
        if (retain_data) {
            memcpy(p_app_inst->retain_data, retain_data, CACHEDATASIZE);
        } else {
            memset(p_app_inst->retain_data, 0, CACHEDATASIZE);
        }
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
        p_launcher->p_main_app_inst =
            *mui_app_inst_dict_get(p_launcher->app_inst_dict, id);
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
        p_launcher->p_main_app_inst =
            *mui_app_inst_dict_get(p_launcher->app_inst_dict, MINI_APP_ID_DESKTOP);
    }
}
void mini_app_launcher_init(mini_app_launcher_t *p_launcher) {

    mui_app_inst_dict_init(p_launcher->app_inst_dict);
    p_launcher->p_main_app_inst = NULL;

    mini_app_launcher_run(p_launcher, MINI_APP_ID_STATUS_BAR);
    mini_app_launcher_run(p_launcher, MINI_APP_ID_DESKTOP);
}
