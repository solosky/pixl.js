#include "app_chameleon.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "chameleon_scene.h"
#include "fds_utils.h"
#include "i18n/language.h"
#include "settings.h"
#include "tag_helper.h"

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
    p_app_handle->p_toast_view = mui_toast_view_create();
    p_app_handle->p_text_input = mui_text_input_create();

    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);
    chameleon_view_set_user_data(p_app_handle->p_chameleon_view, p_app_handle);
    mui_msg_box_set_user_data(p_app_handle->p_msg_box, p_app_handle);
    mui_toast_view_set_user_data(p_app_handle->p_toast_view, p_app_handle);
    mui_text_input_set_user_data(p_app_handle->p_text_input, p_app_handle);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, CHAMELEON_VIEW_ID_MAIN,
                                 chameleon_view_get_view(p_app_handle->p_chameleon_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST,
                                 mui_list_view_get_view(p_app_handle->p_list_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, CHAMELEON_VIEW_ID_MSG_BOX,
                                 mui_msg_box_get_view(p_app_handle->p_msg_box));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, CHAMELEON_VIEW_ID_TEXT_INPUT,
                                 mui_text_input_get_view(p_app_handle->p_text_input));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, chameleon_scene_defines,
                                           CHAMELEON_SCENE_MAX);

    p_app_handle->p_view_dispatcher_toast = mui_view_dispatcher_create();
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher_toast, CHAMELEON_VIEW_ID_TOAST,
                                 mui_toast_view_get_view(p_app_handle->p_toast_view));
    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher_toast, MUI_LAYER_TOAST);
    mui_view_dispatcher_switch_to_view(p_app_handle->p_view_dispatcher_toast, CHAMELEON_VIEW_ID_TOAST);

    if (p_app_inst->p_retain_data) {
        app_chameleon_retain_data_t *p_retain = (app_chameleon_retain_data_t *)p_app_inst->p_retain_data;
    }

    // init emulation
    tag_emulation_init();

    if (!fds_config_file_exists()) {
        mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, CHAMELEON_SCENE_FACTORY);
    } else {
        mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, CHAMELEON_SCENE_MAIN);
    }
}

void app_chameleon_on_kill(mini_app_inst_t *p_app_inst) {
    app_chameleon_t *p_app_handle = p_app_inst->p_handle;

    app_chameleon_retain_data_t *p_retain = (app_chameleon_retain_data_t *)p_app_inst->p_retain_data;

    p_retain->cycle_mode_index = chameleon_view_get_index(p_app_handle->p_chameleon_view);

    settings_data_t *settings = settings_get_data();
    if (tag_helper_valid_default_slot()) {
        tag_emulation_change_slot(settings->chameleon_default_slot_index, false);
    }

    tag_emulation_save();
    settings_save();

    mui_scene_dispatcher_exit(p_app_handle->p_scene_dispatcher);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    mui_text_input_free(p_app_handle->p_text_input);
    chameleon_view_free(p_app_handle->p_chameleon_view);

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher_toast, MUI_LAYER_TOAST);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher_toast);
    mui_toast_view_free(p_app_handle->p_toast_view);

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_chameleon_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

mini_app_t app_chameleon_info = {.id = MINI_APP_ID_CHAMELEON,
                                 .name = "Chameleon",
                                 .name_i18n_key = _L_APP_CHAMELEON,
                                 .icon = 0xe1db,
                                 .deamon = false,
                                 .sys = false,
                                 .hibernate_enabled = true,
                                 .icon_32x32 = &app_card_emulator_32x32,
                                 .run_cb = app_chameleon_on_run,
                                 .kill_cb = app_chameleon_on_kill,
                                 .on_event_cb = app_chameleon_on_event};
