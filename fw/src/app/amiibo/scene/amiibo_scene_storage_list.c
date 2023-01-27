#include "amiibo_helper.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vfs.h"

static void amiibo_scene_storage_list_load_amiibo_keys(vfs_driver_t *p_driver) {
    if (!amiibo_helper_is_key_loaded()) {
        uint8_t key_data[160];
        int32_t err = p_driver->read_file_data("/key_retail.bin", key_data, sizeof(key_data));
        NRF_LOG_INFO("amiibo key read: %d", err);
        if (err == sizeof(key_data)) {
            ret_code_t ret = amiibo_helper_load_keys(key_data);
            if (ret == NRF_SUCCESS) {
                NRF_LOG_INFO("amiibo key loaded!");
            }
        }
    }
}

static void amiibo_scene_storage_list_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                  mui_list_item_t *p_item) {
    app_amiibo_t *p_app = p_list_view->user_data;
    p_app->current_drive = (vfs_drive_t)p_item->user_data;
    string_set_str(p_app->current_folder, "/");
    vfs_driver_t *p_driver = vfs_get_driver(p_app->current_drive);

    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (p_driver->mounted()) {
            amiibo_scene_storage_list_load_amiibo_keys(p_driver);
            mui_scene_dispatcher_next_scene(p_app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
        } else {
            int32_t err = p_driver->mount();
            amiibo_scene_storage_list_load_amiibo_keys(p_driver);
            if (err == VFS_OK) {
                mui_scene_dispatcher_next_scene(p_app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
            }
        }
    } else {
        mui_scene_dispatcher_next_scene(p_app->p_scene_dispatcher, AMIIBO_SCENE_STORAGE_LIST_MENU);
    }
}

void amiibo_scene_storage_list_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    if (vfs_drive_enabled(VFS_DRIVE_INT)) {
        mui_list_view_add_item(app->p_list_view, 0xe1bb, "[Internal Flash]", (void *)VFS_DRIVE_INT);
    }
    if (vfs_drive_enabled(VFS_DRIVE_EXT)) {
        mui_list_view_add_item(app->p_list_view, 0xe1bb, "[External Flash]", (void *)VFS_DRIVE_EXT);
    }

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_storage_list_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_storage_list_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}