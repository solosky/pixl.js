#include "app_amiibo.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "amiibo_view.h"
#include "mui_list_view.h"

typedef enum { AMIIBO_VIEW_ID_MAIN } amiibo_view_id_t;

static void app_amiibo_on_run(mini_app_inst_t *p_app_inst);
static void app_amiibo_on_kill(mini_app_inst_t *p_app_inst);
static void app_amiibo_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

typedef struct {
    amiibo_view_t *p_amiibo_view;
    mui_list_view_t *p_list_view;
    mui_view_dispatcher_t *p_view_dispatcher;
} app_amiibo_t;

void app_amiibo_on_run(mini_app_inst_t *p_app_inst) {

    app_amiibo_t *p_app_handle = malloc(sizeof(app_amiibo_t));

    p_app_inst->p_handle = p_app_handle;
    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_amiibo_view = amiibo_view_create();
    p_app_handle->p_list_view = mui_list_view_create();

    mui_list_view_add_item(p_app_handle->p_list_view, 0xe1d6, "-- 上级目录 --", NULL);    
    mui_list_view_add_item(p_app_handle->p_list_view, 0xe1ed, "Link - Ocarina of Time", NULL);
    mui_list_view_add_item(p_app_handle->p_list_view, 0xe1ed, "Link - Link's Awakening", NULL);
    mui_list_view_add_item(p_app_handle->p_list_view, 0xe1ed, "Toon Link - The Wind Waker", NULL);
    mui_list_view_add_item(p_app_handle->p_list_view, 0xe1ed, "Link - Ocarina of Time", NULL);
    mui_list_view_add_item(p_app_handle->p_list_view, 0xe1ed, "Midna & Wolf Link", NULL);
    mui_list_view_add_item(p_app_handle->p_list_view, 0xe1ed, "Zelda & Loftwing", NULL);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBO_VIEW_ID_MAIN,
                                 mui_list_view_get_view(p_app_handle->p_list_view));
    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_switch_to_view(p_app_handle->p_view_dispatcher,
                                       AMIIBO_VIEW_ID_MAIN);
}

void app_amiibo_on_kill(mini_app_inst_t *p_app_inst) {
    app_amiibo_t *p_app_handle = p_app_inst->p_handle;

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    amiibo_view_free(p_app_handle->p_amiibo_view);

    free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_amiibo_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

const mini_app_t app_amiibo_info = {.id = MINI_APP_ID_AMIIBO,
                                     .name = "amiibo",
                                     .deamon = false,
                                     .run_cb = app_amiibo_on_run,
                                     .kill_cb = app_amiibo_on_kill,
                                     .on_event_cb = app_amiibo_on_event};
