#include "app_settings.h"
#include "settings_scene.h"
#include "settings.h"
#include "i18n/language.h"

static void app_settings_on_run(mini_app_inst_t *p_app_inst);
static void app_settings_on_kill(mini_app_inst_t *p_app_inst);
static void app_settings_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

void app_settings_on_run(mini_app_inst_t *p_app_inst) {

    app_settings_t *p_app_handle = mui_mem_malloc(sizeof(app_settings_t));

    p_app_inst->p_handle = p_app_handle;
    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_list_view = mui_list_view_create();
    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);
    p_app_handle->p_progress_bar = mui_progress_bar_create();
    mui_progress_bar_set_user_data(p_app_handle->p_progress_bar, p_app_handle);

    p_app_handle->p_msg_box = mui_msg_box_create();
    mui_msg_box_set_user_data(p_app_handle->p_msg_box, p_app_handle);

    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, settings_scene_defines,
                                           SETTINGS_SCENE_MAX);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN,
                                 mui_list_view_get_view(p_app_handle->p_list_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, SETTINGS_VIEW_ID_PROGRESS_BAR,
                                 mui_progress_bar_get_view(p_app_handle->p_progress_bar));
                                 mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, SETTINGS_VIEW_ID_MSG_BOX,
                                 mui_msg_box_get_view(p_app_handle->p_msg_box));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    p_app_handle->p_toast_view = mui_toast_view_create();
    mui_toast_view_set_user_data(p_app_handle->p_toast_view, p_app_handle);

    p_app_handle->p_view_dispatcher_toast = mui_view_dispatcher_create();
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher_toast, SETTINGS_VIEW_ID_TOAST,
                                 mui_toast_view_get_view(p_app_handle->p_toast_view));
    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher_toast, MUI_LAYER_TOAST);
    mui_view_dispatcher_switch_to_view(p_app_handle->p_view_dispatcher_toast, SETTINGS_VIEW_ID_TOAST);

    mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, SETTINGS_SCENE_MAIN);
}

void app_settings_on_kill(mini_app_inst_t *p_app_inst) {

    settings_save();

    app_settings_t *p_app_handle = p_app_inst->p_handle;
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);
    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_WINDOW);

    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    mui_progress_bar_free(p_app_handle->p_progress_bar);
    mui_msg_box_free(p_app_handle->p_msg_box);

    mui_toast_view_free(p_app_handle->p_toast_view);
    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher_toast, MUI_LAYER_TOAST);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher_toast);

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_settings_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

mini_app_t app_settings_info = {.id = MINI_APP_ID_SETTINGS,
                                      .name = "系统设置",
                                      .name_i18n_key = _L_APP_SET,
                                      .icon = 0xe02b,
                                      .deamon = false,
                                      .sys = false,
                                      .hibernate_enabled = false,
                                      .icon_32x32 = &app_settings_32x32,
                                      .run_cb = app_settings_on_run,
                                      .kill_cb = app_settings_on_kill,
                                      .on_event_cb = app_settings_on_event};
