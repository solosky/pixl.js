#include "app_game.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "game_scene.h"
#include "game_view.h"
#include "i18n/language.h"

static void app_game_on_run(mini_app_inst_t *p_app_inst);
static void app_game_on_kill(mini_app_inst_t *p_app_inst);
static void app_game_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

void app_game_on_run(mini_app_inst_t *p_app_inst) {

    app_game_t *p_app_handle = mui_mem_malloc(sizeof(app_game_t));
    p_app_inst->p_handle = p_app_handle;

    string_init(p_app_handle->selected_file);

    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_game_view = game_view_create();
    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();
    p_app_handle->p_list_view = mui_list_view_create();
    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);
    game_view_set_user_data(p_app_handle->p_game_view, p_app_handle);


    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, GAME_VIEW_ID_GAME,
                                 game_view_get_view(p_app_handle->p_game_view));

                                     mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, GAME_VIEW_ID_LIST,
                                 mui_list_view_get_view(p_app_handle->p_list_view));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, game_scene_defines, GAME_SCENE_MAX);

    mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, GAME_SCENE_GAME_LIST);
}

void app_game_on_kill(mini_app_inst_t *p_app_inst) {
    app_game_t *p_app_handle = p_app_inst->p_handle;

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    game_view_free(p_app_handle->p_game_view);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_game_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

mini_app_t app_game_info = {.id = MINI_APP_ID_GAME,
                                 .name = "动画播放器",
                                 .name_i18n_key = _L_APP_GAME,
                                 .icon = 0xe021,
                                 .deamon = false,
                                 .sys = false,
                                 .hibernate_enabled = false,
                                 .icon_32x32 = &app_card_emulator_32x32,
                                 .run_cb = app_game_on_run,
                                 .kill_cb = app_game_on_kill,
                                 .on_event_cb = app_game_on_event};
