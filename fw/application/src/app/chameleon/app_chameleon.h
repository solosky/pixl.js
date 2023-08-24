#ifndef APP_CHAMELEON_H
#define APP_CHAMELEON_H

#include "mini_app_defines.h"
#include "mui_list_view.h"
#include "chameleon_view.h"
#include "mui_msg_box.h"

typedef struct {
    chameleon_view_t *p_chameleon_view;
    mui_list_view_t *p_list_view;
    mui_msg_box_t *p_msg_box;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;
} app_chameleon_t;

typedef enum { CHAMELEON_VIEW_ID_LIST, CHAMELEON_VIEW_ID_MAIN, CHAMELEON_VIEW_ID_MSG_BOX } app_chameleon_view_id_t;

extern mini_app_t app_chameleon_info;

#endif