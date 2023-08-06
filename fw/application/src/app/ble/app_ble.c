#include "app_ble.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "ble_scene.h"
#include "ble_status_view.h"

static void app_ble_on_run(mini_app_inst_t *p_app_inst);
static void app_ble_on_kill(mini_app_inst_t *p_app_inst);
static void app_ble_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

void app_ble_on_run(mini_app_inst_t *p_app_inst) {

    app_ble_t *p_app_handle = mui_mem_malloc(sizeof(app_ble_t));

    p_app_inst->p_handle = p_app_handle;
    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_ble_view = ble_status_view_create();
    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, BLE_VIEW_ID_MAIN,
                                 ble_status_view_get_view(p_app_handle->p_ble_view));
    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_WINDOW);

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, ble_scene_defines, BLE_SCENE_MAX);

    mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, BLE_SCENE_CONNECT_START);
}

void app_ble_on_kill(mini_app_inst_t *p_app_inst) {
    app_ble_t *p_app_handle = p_app_inst->p_handle;

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_WINDOW);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    ble_status_view_free(p_app_handle->p_ble_view);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_ble_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

mini_app_t app_ble_info = {.id = MINI_APP_ID_BLE,
                                 .name = "蓝牙传输",
                                 .icon = 0xe1b5,
                                 .deamon = false,
                                 .sys = false,
                                 .hibernate_enabled = false,
                                 .run_cb = app_ble_on_run,
                                 .kill_cb = app_ble_on_kill,
                                 .on_event_cb = app_ble_on_event};
