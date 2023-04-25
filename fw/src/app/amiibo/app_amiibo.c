#include "app_amiibo.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "amiibo_detail_view.h"

#include "amiibo_scene.h"

#include "mui_list_view.h"

#include "m-string.h"
#include "settings.h"
#include "cache.h"

static void app_amiibo_on_run(mini_app_inst_t *p_app_inst);
static void app_amiibo_on_kill(mini_app_inst_t *p_app_inst);
static void app_amiibo_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

void app_amiibo_on_run(mini_app_inst_t *p_app_inst) {

    app_amiibo_t *p_app_handle = mui_mem_malloc(sizeof(app_amiibo_t));
    memset(p_app_handle, 0, sizeof(app_amiibo_t));

    p_app_inst->p_handle = p_app_handle;
    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_amiibo_detail_view = amiibo_detail_view_create();
    p_app_handle->p_amiibo_detail_view->user_data = p_app_handle;
    p_app_handle->p_list_view = mui_list_view_create();
    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);
    p_app_handle->p_text_input = mui_text_input_create();
    mui_text_input_set_user_data(p_app_handle->p_text_input, p_app_handle);
    p_app_handle->p_msg_box = mui_msg_box_create();
    mui_msg_box_set_user_data(p_app_handle->p_msg_box, p_app_handle);

    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();

    string_init(p_app_handle->current_file);
    string_init(p_app_handle->current_folder);
    string_array_init(p_app_handle->amiibo_files);

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, amiibo_scene_defines, AMIIBO_SCENE_MAX);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBO_VIEW_ID_LIST,
                                 mui_list_view_get_view(p_app_handle->p_list_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBO_VIEW_ID_DETAIL,
                                 amiibo_detail_view_get_view(p_app_handle->p_amiibo_detail_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT,
                                 mui_text_input_get_view(p_app_handle->p_text_input));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX,
                                 mui_msg_box_get_view(p_app_handle->p_msg_box));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, AMIIBO_SCENE_STORAGE_LIST);

    if (!cache_empty(p_app_inst->retain_data)) {
        app_amiibo_cache_data_t *p_cache_data = (app_amiibo_cache_data_t *) p_app_inst->retain_data;
        p_app_handle->current_drive = p_cache_data->current_drive;
        string_set_str(p_app_handle->current_file, p_cache_data->current_file);
        string_set_str(p_app_handle->current_folder, p_cache_data->current_folder);
        memcpy(&(p_app_handle->ntag), &(cache_get_data()->ntag), sizeof(ntag_t));

        vfs_driver_t *p_driver = vfs_get_driver(p_app_handle->current_drive);
        if (p_driver->mounted()) {
            amiibo_helper_try_load_amiibo_keys_from_vfs();
        } else {
            int32_t err = p_driver->mount();
            amiibo_helper_try_load_amiibo_keys_from_vfs();
        }

        p_app_handle->reload_amiibo_files = true;
        mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_DETAIL);
    }
}

void app_amiibo_on_kill(mini_app_inst_t *p_app_inst) {
    app_amiibo_t *p_app_handle = p_app_inst->p_handle;

    if (string_cmp_str(p_app_handle->current_file, "") && string_cmp_str(p_app_handle->current_folder, "") && p_app_handle->current_drive != VFS_DRIVE_MAX) {
        app_amiibo_cache_data_t p_cache_data = {0};
        strcpy(p_cache_data.current_file, string_get_cstr(p_app_handle->current_file));
        strcpy(p_cache_data.current_folder, string_get_cstr(p_app_handle->current_folder));
        p_cache_data.current_drive = p_app_handle->current_drive;
        memcpy(&(cache_get_data()->ntag), &(p_app_handle->ntag), sizeof(ntag_t));
        if (settings_get_data()->auto_gen_amiibo) {
            amiibo_helper_ntag_generate(&(cache_get_data()->ntag));
        }
        memcpy(p_app_inst->retain_data, &p_cache_data, sizeof(app_amiibo_cache_data_t));
    } else {
        memset(p_app_inst->retain_data, 0, CACHEDATASIZE);
        memset(&(cache_get_data()->ntag), 0, sizeof(ntag_t));
    }

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    mui_text_input_free(p_app_handle->p_text_input);
    mui_msg_box_free(p_app_handle->p_msg_box);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);
    amiibo_detail_view_free(p_app_handle->p_amiibo_detail_view);

    string_clear(p_app_handle->current_file);
    string_clear(p_app_handle->current_folder);
    string_array_clear(p_app_handle->amiibo_files);

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
