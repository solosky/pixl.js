#ifndef APP_PLAYER_H
#define APP_PLAYER_H

#include "mini_app_defines.h"
#include "mui_list_view.h"
#include "player_view.h"

typedef struct {
    player_view_t *p_player_view;
    mui_list_view_t *p_list_view;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;
    string_t selected_file;
} app_player_t;

typedef enum { PLAYER_VIEW_ID_LIST, PLAYER_VIEW_ID_PLAYER } player_view_id_t;

extern mini_app_t app_player_info;

#endif