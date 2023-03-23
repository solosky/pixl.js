#include "app_amiibolink.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "amiibolink_scene.h"

static void app_amiibolink_on_run(mini_app_inst_t *p_app_inst);
static void app_amiibolink_on_kill(mini_app_inst_t *p_app_inst);
static void app_amiibolink_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

void app_amiibolink_on_run(mini_app_inst_t *p_app_inst) {

    app_amiibolink_t *p_app_handle = mui_mem_malloc(sizeof(app_amiibolink_t));
    p_app_inst->p_handle = p_app_handle;

    p_app_handle->amiibolink_mode = BLE_AMIIBOLINK_MODE_RANDOM;
    p_app_handle->auto_generate = false;

    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_amiibolink_view = amiibolink_view_create();
    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();
    p_app_handle->p_list_view = mui_list_view_create();
    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);
    amiibolink_view_set_user_data(p_app_handle->p_amiibolink_view, p_app_handle);


    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBOLINK_VIEW_ID_MAIN,
                                 amiibolink_view_get_view(p_app_handle->p_amiibolink_view));

                                     mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBOLINK_VIEW_ID_LIST,
                                 mui_list_view_get_view(p_app_handle->p_list_view));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, amiibolink_scene_defines, AMIIBOLINK_SCENE_MAX);

    mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, AMIIBOLINK_SCENE_MAIN);
}

void app_amiibolink_on_kill(mini_app_inst_t *p_app_inst) {
    app_amiibolink_t *p_app_handle = p_app_inst->p_handle;

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    amiibolink_view_free(p_app_handle->p_amiibolink_view);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_amiibolink_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

const mini_app_t app_amiibolink_info = {.id = MINI_APP_ID_AMIIBOLINK,
                                 .name = "AmiiboLink",
                                 .icon = 0xe1c1,
                                 .deamon = false,
                                 .sys = false,
                                 .run_cb = app_amiibolink_on_run,
                                 .kill_cb = app_amiibolink_on_kill,
                                 .on_event_cb = app_amiibolink_on_event};
