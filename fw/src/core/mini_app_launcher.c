
#include "mini_app_launcher.h"

mini_app_launcher_t *mini_app_launcher() {
    static mini_app_launcher_t launcher;
    return &launcher;
}

static void mini_app_launcher_inst_kill(mini_app_launcher_t *p_launcher,
                                      mini_app_inst_t *p_app_inst) {
    p_app_inst->kill_cb(p_app_inst);
    p_app_inst->state = APP_STATE_KILLED;
}

static void mini_app_launcher_inst_run(mini_app_launcher_t *p_launcher,
                                      uint32_t id) {
    mini_app_inst_t *p_app_inst = mui_app_inst_dict_get(p_launcher->app_inst_dict, id);
    if (p_app_inst) {
        p_app->run_cb(p_app_inst);
    } else {
        p_app_inst = malloc(sizeof(mini_app_inst_t));
        p_app_inst->p_app = p_app;
        p_app_inst->state = APP_STATE_RUNNING;
        mui_app_inst_dict_set_at(p_launcher->app_inst_dict, id, p_app_inst);
        p_app->run_cb(p_app_inst);
    }
}

void mini_app_launcher_run(mini_app_launcher_t *p_launcher, uint32_t id) {
    mini_app_t *p_app = mini_app_registry_find_by_id(id);
    if (p_app == NULL) {
        return;
    }

    if (!p_app->demeon && p_launcher->p_main_app_inst) {
        mini_app_launcher_inst_kill(p_launcher, p_launcher->p_main_app_inst);
    }

    mini_app_launcher_inst_run(p_launcher, id);

    if (!p_app->demaon) {
        p_launcher->p_main_app_inst = mui_app_inst_dict_get(p_launcher->app_inst_dict, id);
    }
}
void mini_app_launcher_kill(mini_app_launcher_t *p_launcher, uint32_t id) {
    mini_app_inst_t *p_app_inst = mui_app_inst_dict_get(p_launcher->app_inst_dict, id);
    if (p_app_inst) {
        mini_app_launcher_inst_kill(p_launcher, p_app_inst);
    }

    //run desktop if kill current app
    if(p_app_inst == p_launcher->p_main_app_inst){
        mini_app_launcher_inst_run(p_launcher, MINI_APP_ID_DESKTOP);
        p_launcher->p_main_app_inst = mui_app_inst_dict_get(p_launcher->app_inst_dict, MINI_APP_ID_DESKTOP);
    }

}
void mini_app_launcher_init(mini_app_launcher_t *p_launcher) {
    mini_app_launcher_run(p_launcher, MINI_APP_ID_STATUS_BAR);
    mini_app_launcher_run(p_launcher, MINI_APP_ID_DESKTOP);
}
