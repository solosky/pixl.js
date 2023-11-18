#ifndef MINI_APP_LAUNCHER_H
#define MINI_APP_LAUNCHER_H

#include "mui_include.h"

#include "mini_app_defines.h"


DICT_DEF2(mui_app_inst_dict, uint32_t, M_DEFAULT_OPLIST, mini_app_inst_t*, M_PTR_OPLIST)


typedef struct {
    mini_app_inst_t* p_main_app_inst; 
    mui_app_inst_dict_t app_inst_dict;
} mini_app_launcher_t;


mini_app_launcher_t* mini_app_launcher();
void mini_app_launcher_run(mini_app_launcher_t* p_launcher, uint32_t id);
void mini_app_launcher_kill(mini_app_launcher_t* p_launcher, uint32_t id);
void mini_app_launcher_exit(mini_app_launcher_t* p_launcher);
void mini_app_launcher_init(mini_app_launcher_t* p_launcher);
void mini_app_launcher_sleep(mini_app_launcher_t* p_launcher);
void mini_app_launcher_post_event(mini_app_launcher_t* p_launcher, uint32_t id, mini_app_event_t* p_event);
void* mini_app_launcher_get_app_handle(mini_app_launcher_t* p_launcher, uint32_t id);

void mini_app_launcher_run_with_retain_data(mini_app_launcher_t *p_launcher, uint32_t id, uint8_t *retain_data);

#endif 