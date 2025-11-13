#include "app_amiibolink.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "amiibolink_scene.h"
#include "i18n/language.h"
#include "settings.h"

static void app_amiibolink_on_run(mini_app_inst_t *p_app_inst);
static void app_amiibolink_on_kill(mini_app_inst_t *p_app_inst);
static void app_amiibolink_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

typedef struct {
    ble_amiibolink_mode_t amiibolink_mode;
    uint8_t cycle_mode_index;
} app_amiibolink_retain_data_t;

void app_amiibolink_on_run(mini_app_inst_t *p_app_inst) {

    app_amiibolink_t *p_app_handle = mui_mem_malloc(sizeof(app_amiibolink_t));
    p_app_inst->p_handle = p_app_handle;

    // Load saved mode from settings, or use default (manual/random)
    settings_data_t *p_settings = settings_get_data();
    if (p_settings->amiibolink_mode != 0) {
        p_app_handle->amiibolink_mode = p_settings->amiibolink_mode;
        NRF_LOG_INFO("Loaded saved amiibolink_mode: %d", p_app_handle->amiibolink_mode);
    } else {
        p_app_handle->amiibolink_mode = BLE_AMIIBOLINK_MODE_RANDOM;
    }

    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_amiibolink_view = amiibolink_view_create();
    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();
    p_app_handle->p_list_view = mui_list_view_create();
    p_app_handle->p_msg_box = mui_msg_box_create();

    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);
    amiibolink_view_set_user_data(p_app_handle->p_amiibolink_view, p_app_handle);
    mui_msg_box_set_user_data(p_app_handle->p_msg_box, p_app_handle);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBOLINK_VIEW_ID_MAIN,
                                 amiibolink_view_get_view(p_app_handle->p_amiibolink_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBOLINK_VIEW_ID_LIST,
                                 mui_list_view_get_view(p_app_handle->p_list_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBOLINK_VIEW_ID_MSG_BOX,
                                 mui_msg_box_get_view(p_app_handle->p_msg_box));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, amiibolink_scene_defines,
                                           AMIIBOLINK_SCENE_MAX);

    extern const ntag_t default_ntag215;
    APP_ERROR_CHECK(ntag_emu_init(&default_ntag215));

    if (p_app_inst->p_retain_data) {
        app_amiibolink_retain_data_t *p_retain = (app_amiibolink_retain_data_t *)p_app_inst->p_retain_data;
        p_app_handle->amiibolink_mode = p_retain->amiibolink_mode;
        if (p_app_handle->amiibolink_mode == BLE_AMIIBOLINK_MODE_CYCLE) {
            amiibolink_view_set_index(p_app_handle->p_amiibolink_view, p_retain->cycle_mode_index);
        }
    }

    mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, AMIIBOLINK_SCENE_MAIN);
}

void app_amiibolink_on_kill(mini_app_inst_t *p_app_inst) {
    app_amiibolink_t *p_app_handle = p_app_inst->p_handle;

    app_amiibolink_retain_data_t *p_retain = (app_amiibolink_retain_data_t *)p_app_inst->p_retain_data;

    p_retain->amiibolink_mode = p_app_handle->amiibolink_mode;
    p_retain->cycle_mode_index = amiibolink_view_get_index(p_app_handle->p_amiibolink_view);

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    amiibolink_view_free(p_app_handle->p_amiibolink_view);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_amiibolink_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

mini_app_t app_amiibolink_info = {.id = MINI_APP_ID_AMIIBOLINK,
                                        .name = "AmiiboLink",
                                        .name_i18n_key = _L_APP_AMIIBOLINK,
                                        .icon = 0xe1c1,
                                        .deamon = false,
                                        .sys = false,
                                        .hibernate_enabled = true,
                                        .icon_32x32 = &app_amiibo_link_32x32,
                                        .run_cb = app_amiibolink_on_run,
                                        .kill_cb = app_amiibolink_on_kill,
                                        .on_event_cb = app_amiibolink_on_event};
