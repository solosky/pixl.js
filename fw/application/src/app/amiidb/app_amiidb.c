#include "app_amiidb.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "amiidb_scene.h"

#include "mui_list_view.h"

#include "cache.h"
#include "m-string.h"
#include "settings.h"

#include "amiibo_helper.h"

static void app_amiidb_on_run(mini_app_inst_t *p_app_inst);
static void app_amiidb_on_kill(mini_app_inst_t *p_app_inst);
static void app_amiidb_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

static void app_amiidb_try_mount_drive(app_amiidb_t *p_app_inst) {
    vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
    if (p_driver->mounted()) {
        amiibo_helper_try_load_amiibo_keys_from_vfs();
    } else {
        int32_t err = p_driver->mount();
        amiibo_helper_try_load_amiibo_keys_from_vfs();
    }

    p_driver->create_dir("/amiibo");
    p_driver->create_dir("/amiibo/fav");
    p_driver->create_dir("/amiibo/data");
}

void app_amiidb_on_run(mini_app_inst_t *p_app_inst) {

    app_amiidb_t *p_app_handle = mui_mem_malloc(sizeof(app_amiidb_t));
    memset(p_app_handle, 0, sizeof(app_amiidb_t));
    

    p_app_inst->p_handle = p_app_handle;
    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_list_view = mui_list_view_create();
    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);
    p_app_handle->p_text_input = mui_text_input_create();
    mui_text_input_set_user_data(p_app_handle->p_text_input, p_app_handle);
    p_app_handle->p_msg_box = mui_msg_box_create();
    mui_msg_box_set_user_data(p_app_handle->p_msg_box, p_app_handle);
    p_app_handle->p_amiibo_view = amiibo_view_create();
    p_app_handle->p_amiibo_view->user_data = p_app_handle;
    p_app_handle->p_toast_view = mui_toast_view_create();
    mui_toast_view_set_user_data(p_app_handle->p_toast_view, p_app_handle);
    
    string_init(p_app_handle->cur_fav_dir);
    string_init(p_app_handle->search_str);
    p_app_handle->in_fav_folders = true;

    amiidb_fav_array_init(p_app_handle->fav_array);

    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, amiidb_scene_defines, AMIIDB_SCENE_MAX);
    mui_mui_scene_dispatcher_set_default_scene_id(p_app_handle->p_scene_dispatcher, AMIIDB_SCENE_MAIN);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIDB_VIEW_ID_LIST,
                                 mui_list_view_get_view(p_app_handle->p_list_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIDB_VIEW_ID_DETAIL,
                                 amiibo_view_get_view(p_app_handle->p_amiibo_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIDB_VIEW_ID_INPUT,
                                 mui_text_input_get_view(p_app_handle->p_text_input));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, AMIIDB_VIEW_ID_MSG_BOX,
                                 mui_msg_box_get_view(p_app_handle->p_msg_box));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    p_app_handle->p_view_dispatcher_toast = mui_view_dispatcher_create();
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher_toast, AMIIDB_VIEW_ID_TOAST,
                                 mui_toast_view_get_view(p_app_handle->p_toast_view));
    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher_toast, MUI_LAYER_TOAST);
    mui_view_dispatcher_switch_to_view(p_app_handle->p_view_dispatcher_toast, AMIIDB_VIEW_ID_TOAST);

    app_amiidb_try_mount_drive(p_app_handle);

    if (p_app_inst->p_retain_data) {
        app_amiidb_cache_data_t *p_cache_data = (app_amiidb_cache_data_t *)p_app_inst->p_retain_data;
        if (p_cache_data->cached_enabled) {
            //recover states from retain data
            p_app_handle->game_id_index = p_cache_data->game_id_index;
            memcpy(p_app_handle->game_id_path, p_cache_data->game_id_path, sizeof(p_app_handle->game_id_path));
            p_app_handle->prev_scene_id = p_cache_data->prev_scene_id;
            p_app_handle->cur_focus_index = p_cache_data->current_focus_index;
            p_app_handle->cur_scroll_offset = p_cache_data->cur_scroll_offset;
            mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, p_cache_data->current_scene_id);

        } else {
            mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, AMIIDB_SCENE_MAIN);
        }
    } else {
        mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, AMIIDB_SCENE_MAIN);
    }
}

void app_amiidb_on_kill(mini_app_inst_t *p_app_inst) {
    app_amiidb_t *p_app_handle = p_app_inst->p_handle;

    uint32_t current_scene_id = mui_scene_dispatcher_current_scene(p_app_handle->p_scene_dispatcher);

    if (app_amiidb_info.hibernate_enabled) {
        app_amiidb_cache_data_t p_cache_data = {0};
        p_cache_data.cached_enabled = true;
        memcpy(p_cache_data.game_id_path, p_app_handle->game_id_path, sizeof(p_app_handle->game_id_path));
        p_cache_data.game_id_index = p_app_handle->game_id_index;
        p_cache_data.prev_scene_id = p_app_handle->prev_scene_id;
        p_cache_data.current_scene_id = current_scene_id;
        p_cache_data.current_focus_index = mui_list_view_get_focus(p_app_handle->p_list_view);
        p_cache_data.cur_scroll_offset = mui_list_view_get_scroll_offset(p_app_handle->p_list_view);

        memcpy(p_app_inst->p_retain_data, &p_cache_data, sizeof(app_amiidb_cache_data_t));
    } else {
        memset(p_app_inst->p_retain_data, 0, CACHEDATASIZE);
        memset(&(cache_get_data()->ntag), 0, sizeof(ntag_t));
    }

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    mui_text_input_free(p_app_handle->p_text_input);
    mui_msg_box_free(p_app_handle->p_msg_box);
    amiibo_view_free(p_app_handle->p_amiibo_view);
    mui_toast_view_free(p_app_handle->p_toast_view);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher_toast, MUI_LAYER_TOAST);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher_toast);

    string_clear(p_app_handle->cur_fav_dir);
    string_clear(p_app_handle->search_str);
    amiidb_fav_array_clear(p_app_handle->fav_array);

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_amiidb_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

const mini_app_t app_amiidb_info = {.id = MINI_APP_ID_AMIIDB,
                                    .name = "Amiibo数据库",
                                    .name_i18n_key = _L_APP_AMIIDB,
                                    .icon = 0xe0ba,
                                    .sys = false,
                                    .deamon = false,
                                    .hibernate_enabled = false,
                                    .run_cb = app_amiidb_on_run,
                                    .kill_cb = app_amiidb_on_kill,
                                    .on_event_cb = app_amiidb_on_event};
