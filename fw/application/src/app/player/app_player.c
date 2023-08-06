#include "app_player.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "player_scene.h"
#include "player_view.h"

static void app_player_on_run(mini_app_inst_t *p_app_inst);
static void app_player_on_kill(mini_app_inst_t *p_app_inst);
static void app_player_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

void app_player_on_run(mini_app_inst_t *p_app_inst) {

    app_player_t *p_app_handle = mui_mem_malloc(sizeof(app_player_t));
    p_app_inst->p_handle = p_app_handle;

    string_init(p_app_handle->selected_file);

    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_player_view = player_view_create();
    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();
    p_app_handle->p_list_view = mui_list_view_create();
    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);
    player_view_set_user_data(p_app_handle->p_player_view, p_app_handle);


    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, PLAYER_VIEW_ID_PLAYER,
                                 player_view_get_view(p_app_handle->p_player_view));

                                     mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, PLAYER_VIEW_ID_LIST,
                                 mui_list_view_get_view(p_app_handle->p_list_view));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, player_scene_defines, PLAYER_SCENE_MAX);

    mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, PLAYER_SCENE_FILE_BROWSER);
}

void app_player_on_kill(mini_app_inst_t *p_app_inst) {
    app_player_t *p_app_handle = p_app_inst->p_handle;

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    player_view_free(p_app_handle->p_player_view);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_player_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

mini_app_t app_player_info = {.id = MINI_APP_ID_PLAYER,
                                 .name = "动画播放器",
                                 .icon = 0xe020,
                                 .deamon = false,
                                 .sys = false,
                                 .hibernate_enabled = false,
                                 .run_cb = app_player_on_run,
                                 .kill_cb = app_player_on_kill,
                                 .on_event_cb = app_player_on_event};
