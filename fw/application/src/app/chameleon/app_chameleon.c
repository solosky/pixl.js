#include "app_chameleon.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "chameleon_scene.h"
#include "i18n/language.h"

static void app_chameleon_on_run(mini_app_inst_t *p_app_inst);
static void app_chameleon_on_kill(mini_app_inst_t *p_app_inst);
static void app_chameleon_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

typedef struct {
    uint8_t cycle_mode_index;
} app_chameleon_retain_data_t;

void app_chameleon_on_run(mini_app_inst_t *p_app_inst) {

    app_chameleon_t *p_app_handle = mui_mem_malloc(sizeof(app_chameleon_t));
    p_app_inst->p_handle = p_app_handle;
    

    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_chameleon_view = chameleon_view_create();
    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();
    p_app_handle->p_list_view = mui_list_view_create();
    p_app_handle->p_msg_box = mui_msg_box_create();

    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);
    chameleon_view_set_user_data(p_app_handle->p_chameleon_view, p_app_handle);
    mui_msg_box_set_user_data(p_app_handle->p_msg_box, p_app_handle);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, CHAMELEON_VIEW_ID_MAIN,
                                 chameleon_view_get_view(p_app_handle->p_chameleon_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST,
                                 mui_list_view_get_view(p_app_handle->p_list_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, CHAMELEON_VIEW_ID_MSG_BOX,
                                 mui_msg_box_get_view(p_app_handle->p_msg_box));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, chameleon_scene_defines,
                                           CHAMELEON_SCENE_MAX);

    vfs_get_default_driver()->create_dir("/chameleon");

    if (p_app_inst->p_retain_data) {
        app_chameleon_retain_data_t *p_retain = (app_chameleon_retain_data_t *)p_app_inst->p_retain_data;

    }

    mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, CHAMELEON_SCENE_MAIN);
}

void app_chameleon_on_kill(mini_app_inst_t *p_app_inst) {
    app_chameleon_t *p_app_handle = p_app_inst->p_handle;

    app_chameleon_retain_data_t *p_retain = (app_chameleon_retain_data_t *)p_app_inst->p_retain_data;


    p_retain->cycle_mode_index = chameleon_view_get_index(p_app_handle->p_chameleon_view);


    mui_scene_dispatcher_exit(p_app_handle->p_scene_dispatcher);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    chameleon_view_free(p_app_handle->p_chameleon_view);
   

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_chameleon_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

mini_app_t app_chameleon_info = {.id = MINI_APP_ID_CHAMELEON,
                                        .name = "Chameleon",
                                        .name_i18n_key = _L_APP_CHAMELEON,
                                        .icon = 0xe19b,
                                        .deamon = false,
                                        .sys = false,
                                        .hibernate_enabled = true,
                                        .run_cb = app_chameleon_on_run,
                                        .kill_cb = app_chameleon_on_kill,
                                        .on_event_cb = app_chameleon_on_event};
