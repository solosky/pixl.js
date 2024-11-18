#ifndef APP_game_H
#define APP_game_H

#include "mini_app_defines.h"
#include "mui_list_view.h"
#include "game_view.h"

typedef struct {
    game_view_t *p_game_view;
    mui_list_view_t *p_list_view;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;
    string_t selected_file;
} app_game_t;

typedef enum { GAME_VIEW_ID_LIST, GAME_VIEW_ID_GAME } game_view_id_t;

extern mini_app_t app_game_info;

#endif