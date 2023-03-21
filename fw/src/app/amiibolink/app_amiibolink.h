#ifndef app_amiibolink_H
#define app_amiibolink_H

#include "mini_app_defines.h"
#include "mui_list_view.h"
#include "amiibolink_view.h"

typedef struct {
    amiibolink_view_t *p_amiibolink_view;
    mui_list_view_t *p_list_view;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;
    string_t selected_file;
} app_amiibolink_t;

typedef enum { AMIIBOLINK_VIEW_ID_LIST, AMIIBOLINK_VIEW_ID_MAIN } app_amiibolink_view_id_t;

extern const mini_app_t app_amiibolink_info;

#endif