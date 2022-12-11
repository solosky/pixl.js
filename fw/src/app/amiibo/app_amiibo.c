#include "app_amiibo.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "amiibo_data.h"
#include "amiibo_view.h"

#include "amiibo_scene.h"

#include "mui_list_view.h"
#include "ntag_store.h"



static void app_amiibo_on_run(mini_app_inst_t *p_app_inst);
static void app_amiibo_on_kill(mini_app_inst_t *p_app_inst);
static void app_amiibo_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

static void app_amiibo_list_view_on_selected(mui_list_view_t *p_list_view,
                                             mui_list_item_t *p_item) {
    app_amiibo_t *p_app_handle = p_list_view->user_data;
    ntag_store_read_default((uint8_t)p_item->user_data, &p_app_handle->ntag);
    p_app_handle->p_amiibo_view->amiibo = &p_app_handle->ntag;
    mui_view_dispatcher_switch_to_view(p_app_handle->p_view_dispatcher,
                                       AMIIBO_VIEW_ID_DETAIL);
}

static void app_amiibo_view_on_close(amiibo_view_t *p_view) {
    app_amiibo_t *p_app_handle = p_view->user_data;
    mui_view_dispatcher_switch_to_view(p_app_handle->p_view_dispatcher,
                                       AMIIBO_VIEW_ID_LIST);
}

void app_amiibo_on_run(mini_app_inst_t *p_app_inst) {

    app_amiibo_t *p_app_handle = mui_mem_malloc(sizeof(app_amiibo_t));

    p_app_inst->p_handle = p_app_handle;
    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_amiibo_view = amiibo_view_create();
    p_app_handle->p_amiibo_view->user_data = p_app_handle;
    p_app_handle->p_amiibo_view->on_close_cb = app_amiibo_view_on_close;
    p_app_handle->p_list_view = mui_list_view_create();
    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();

    string_init(p_app_handle->current_file);
    string_init(p_app_handle->current_folder);

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, amiibo_scene_defines, AMIIBO_SCENE_MAX);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBO_VIEW_ID_LIST,
                                 mui_list_view_get_view(p_app_handle->p_list_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBO_VIEW_ID_DETAIL,
                                 amiibo_view_get_view(p_app_handle->p_amiibo_view));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, AMIIBO_SCENE_STORAGE_LIST);
}

void app_amiibo_on_kill(mini_app_inst_t *p_app_inst) {
    app_amiibo_t *p_app_handle = p_app_inst->p_handle;

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);
    amiibo_view_free(p_app_handle->p_amiibo_view);

    string_clear(p_app_handle->current_file);
    string_clear(p_app_handle->current_folder);

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_amiibo_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

const mini_app_t app_amiibo_info = {.id = MINI_APP_ID_AMIIBO,
                                    .name = "Amiibo模拟器",
                                    .icon = 0xe082,
                                    .sys = false,
                                    .deamon = false,
                                    .run_cb = app_amiibo_on_run,
                                    .kill_cb = app_amiibo_on_kill,
                                    .on_event_cb = app_amiibo_on_event};
