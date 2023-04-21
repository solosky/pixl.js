#include "amiibo_helper.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vfs.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "cache.h"
#include "settings.h"

#define ICON_BACK 0xe069
#define ICON_DRIVE 0xe1bb
#define ICON_HOME 0xe1f0

static void amiibo_scene_storage_list_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                  mui_list_item_t *p_item) {
    app_amiibo_t *p_app = p_list_view->user_data;
    p_app->current_drive = (vfs_drive_t)p_item->user_data;
    string_set_str(p_app->current_folder, "/");
    vfs_driver_t *p_driver = vfs_get_driver(p_app->current_drive);

    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {

        if (p_item->icon == ICON_DRIVE) {
            if (p_driver->mounted()) {
                amiibo_helper_try_load_amiibo_keys_from_vfs();
                mui_scene_dispatcher_next_scene(p_app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
            } else {
                int32_t err = p_driver->mount();
                amiibo_helper_try_load_amiibo_keys_from_vfs();
                if (err == VFS_OK) {
                    mui_scene_dispatcher_next_scene(p_app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
                }
            }
        } else if (p_item->icon == ICON_HOME) {
            mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIBO);
        }

    } else if(event == MUI_LIST_VIEW_EVENT_LONG_SELECTED){
        if(p_item->icon == ICON_DRIVE) {
            mui_scene_dispatcher_next_scene(p_app->p_scene_dispatcher, AMIIBO_SCENE_STORAGE_LIST_MENU);
        }
    }
}

void amiibo_scene_storage_list_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, ICON_HOME, ">>主菜单<<", (void *)-1);

    if (vfs_drive_enabled(VFS_DRIVE_INT)) {
        mui_list_view_add_item(app->p_list_view, ICON_DRIVE, "[Internal Flash]", (void *)VFS_DRIVE_INT);
    }
    if (vfs_drive_enabled(VFS_DRIVE_EXT)) {
        mui_list_view_add_item(app->p_list_view, ICON_DRIVE, "[External Flash]", (void *)VFS_DRIVE_EXT);
    }

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_storage_list_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);

    vfs_drive_t driver;

    if (vfs_drive_enabled(VFS_DRIVE_INT) && !vfs_drive_enabled(VFS_DRIVE_EXT)) {
        driver = VFS_DRIVE_INT;
    } else if (!vfs_drive_enabled(VFS_DRIVE_INT) && vfs_drive_enabled(VFS_DRIVE_EXT)) {
        driver = VFS_DRIVE_EXT;
    }

    cache_data_t *cache = cache_get_data();

    if (cache->enabled == 1) {
        NRF_LOG_DEBUG("Cache found $ amiibo storage list");
        app->current_drive = cache->current_drive;
        string_set_str(app->current_folder, cache->current_folder);
        vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);

        if (p_driver->mounted()) {
            amiibo_helper_try_load_amiibo_keys_from_vfs();
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
        } else {
            int32_t err = p_driver->mount();
            amiibo_helper_try_load_amiibo_keys_from_vfs();
            if (err == VFS_OK) {
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
            }
        }
        return;
    }
    settings_data_t* p_settings = settings_get_data();
    if (p_settings->skip_driver_select && driver) {
        app->current_drive = driver;
        string_set_str(app->current_folder, "/");
        vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);

        if (p_driver->mounted()) {
            amiibo_helper_try_load_amiibo_keys_from_vfs();
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
        } else {
            int32_t err = p_driver->mount();
            amiibo_helper_try_load_amiibo_keys_from_vfs();
            if (err == VFS_OK) {
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
            }
        }
        return;
    }
}

void amiibo_scene_storage_list_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}