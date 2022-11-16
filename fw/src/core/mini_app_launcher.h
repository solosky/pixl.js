#ifndef MINI_APP_LAUNCHER_H
#define MINI_APP_LAUNCHER_H

#include <stdint.h>
#include "m-dict.h"


#include "mini_app_defines.h"


DICT_DEF2(mui_app_inst_dict, uint32_t, M_DEFAULT_OPLIST, mini_app_inst_t*, M_PTR_OPLIST)


typedef struct {
    mini_app_inst_t current_app_inst; 
    mui_app_inst_dict_t app_inst_dict;
} mini_app_launcher_t;


static mini_app_launcher_t launcher;

void mini_app_launcher_launch(uint32_t id);
void mini_app_launcher_kill(uint32_t id);




#endif 