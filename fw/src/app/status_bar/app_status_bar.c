#include "app_status_bar.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "status_bar_view.h"

typedef enum { STATUS_BAR_VIEW_ID_MAIN } status_bar_view_id_t;

static void app_status_bar_on_run(mini_app_inst_t *p_app_inst);
static void app_status_bar_on_kill(mini_app_inst_t *p_app_inst);
static void app_status_bar_on_event(mini_app_inst_t *p_app_inst,
                                    mini_app_event_t *p_event);

typedef struct {
    status_bar_view_t *p_status_bar_view;
    mui_view_dispatcher_t *p_view_dispatcher;
} app_status_bar_t;

void app_status_bar_on_run(mini_app_inst_t *p_app_inst) {

    app_status_bar_t *p_app_handle = malloc(sizeof(app_status_bar_t));

    p_app_inst->p_handle = p_app_handle;
    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_status_bar_view = status_bar_view_create();

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, STATUS_BAR_VIEW_ID_MAIN,
                                 p_app_handle->p_status_bar_view);
    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_STATUS_BAR);
    mui_view_dispatcher_display_view(STATUS_BAR_VIEW_ID_MAIN);
}

void app_status_bar_on_kill(mini_app_inst_t *p_app_inst) {
    app_status_bar_t *p_app_handle = p_app_inst->p_handle;

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    status_bar_view_free(p_app_handle->p_status_bar_view);

    free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_status_bar_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

const mini_app_t app_status_bar_info = {.id = MINI_APP_ID_STATUS_BAR,
                                        .run_cb = app_status_bar_on_run,
                                        .kill_cb = app_status_bar_on_kill,
                                        .post_event_cb = app_status_bar_on_event};
