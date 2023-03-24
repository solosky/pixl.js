#ifndef app_amiibolink_H
#define app_amiibolink_H

#include "mini_app_defines.h"
#include "mui_list_view.h"
#include "amiibolink_view.h"
#include "ble_amiibolink.h"
#include "mui_msg_box.h"

typedef struct {
    amiibolink_view_t *p_amiibolink_view;
    mui_list_view_t *p_list_view;
    mui_msg_box_t *p_msg_box;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;

    ble_amiibolink_mode_t amiibolink_mode;
    bool auto_generate;
} app_amiibolink_t;

typedef enum { AMIIBOLINK_VIEW_ID_LIST, AMIIBOLINK_VIEW_ID_MAIN, AMIIBOLINK_VIEW_ID_MSG_BOX } app_amiibolink_view_id_t;

extern const mini_app_t app_amiibolink_info;

#endif