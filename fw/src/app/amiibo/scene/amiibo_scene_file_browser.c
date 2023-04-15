#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "cwalk.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vfs.h"
#include "vfs_meta.h"
#include "mini_app_registry.h"

#include "cache.h"
#include "settings.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb
#define ICON_HOME 0xe1f0

#define FOLDER_LIST_PARENT 0xFFFF


static int amiibo_scene_file_browser_list_item_cmp(const mui_list_item_t* p_item_a, const mui_list_item_t* p_item_b){
    if (p_item_a->icon == ICON_HOME || p_item_b->icon == ICON_HOME) {
        return 0;
    }
    if(p_item_a->icon != p_item_b->icon){
        return p_item_a->icon - p_item_b->icon;
    }else{
        return string_cmp(p_item_a->text, p_item_b->text);
    }
}

static void amiibo_scene_file_browser_reload_folders(app_amiibo_t *app) {
    vfs_driver_t *p_vfs_driver;
    vfs_dir_t dir;
    vfs_obj_t obj;

    mui_list_view_clear_items(app->p_list_view);
    if (string_cmp_str(app->current_folder, "/") == 0) {
        bool one_driver = (vfs_drive_enabled(VFS_DRIVE_INT) && !vfs_drive_enabled(VFS_DRIVE_EXT)) || (!vfs_drive_enabled(VFS_DRIVE_INT) && vfs_drive_enabled(VFS_DRIVE_EXT));
        settings_data_t* p_settings = settings_get_data();
        mui_list_view_add_item(app->p_list_view, (one_driver && p_settings->skip_driver_select) ? ICON_HOME : ICON_BACK, (one_driver && p_settings->skip_driver_select) ? ">>主菜单<<" : "..", (void *)(one_driver && p_settings->skip_driver_select) ? -1 : FOLDER_LIST_PARENT);
    } else {
        mui_list_view_add_item(app->p_list_view, ICON_BACK, "..", (void *)FOLDER_LIST_PARENT);
    }

    p_vfs_driver = vfs_get_driver(app->current_drive);

    int32_t res = p_vfs_driver->open_dir(string_get_cstr(app->current_folder), &dir);
    if (res == VFS_OK) {
        while (res = p_vfs_driver->read_dir(&dir, &obj) == VFS_OK) {
            //hide file or dir if flagged with hidden
            vfs_meta_t meta;
            memset(&meta, 0, sizeof(vfs_meta_t));
            vfs_meta_decode(obj.meta, sizeof(obj.meta), &meta);
            if(meta.has_flags && (meta.flags && VFS_OBJ_FLAG_HIDDEN)){
                continue;
            }
            uint16_t icon = obj.type == VFS_TYPE_DIR ? ICON_FOLDER : ICON_FILE;
            mui_list_view_add_item(app->p_list_view, icon, obj.name, (void *)-1);
        }
        p_vfs_driver->close_dir(&dir);
    } else {
        mui_list_view_add_item(app->p_list_view, ICON_ERROR, "打开文件夹失败", (void *)-1);
    }

    mui_list_view_sort(app->p_list_view, amiibo_scene_file_browser_list_item_cmp);


}

static void amiibo_scene_file_browser_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                  mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t idx = (uint32_t)p_item->user_data;

    string_set(app->current_file, p_item->text);

    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (idx == FOLDER_LIST_PARENT) {
            if (string_cmp_str(app->current_folder, "/") == 0) {
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_STORAGE_LIST);
            } else {
                struct cwk_segment segment;
                const char *folder_cstr = string_get_cstr(app->current_folder);
                cwk_path_get_last_segment(folder_cstr, &segment);
                string_left(app->current_folder, segment.begin - folder_cstr);
                if (string_size(app->current_folder) == 0) {
                    string_cat_str(app->current_folder, "/");
                }
                amiibo_scene_file_browser_reload_folders(app);
            }
        } else {
            if (p_item->icon == ICON_FOLDER) {
                if (!string_end_with_str_p(app->current_folder, "/")) {
                    string_cat_str(app->current_folder, "/");
                }
                string_cat(app->current_folder, p_item->text);
                amiibo_scene_file_browser_reload_folders(app);
            } else if (p_item->icon == ICON_HOME) {
                mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIBO);
            } else {
                // TODO AMIIBO test ..

                // cache tag data
                cache_data_t *cache_data = cache_get_data();
                cache_data->current_drive = app->current_drive;
                strcat(cache_data->current_folder, string_get_cstr(app->current_folder));
                strcat(cache_data->current_file, string_get_cstr(app->current_file));

                // read tag
                app->reload_amiibo_files = true;
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_DETAIL);
            }
        }
    } else {
        if (p_item->icon == ICON_HOME) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_STORAGE_LIST_MENU);
        } else {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER_MENU);
        }
    }
}

void amiibo_scene_file_browser_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;
    NRF_LOG_INFO("%X", app);
    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_file_browser_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    amiibo_scene_file_browser_reload_folders(app);
    NRF_LOG_INFO("%X", app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);

    cache_data_t *cache = cache_get_data();

    if (cache->enabled && cache->current_file != "") {
        NRF_LOG_DEBUG("Cache found $ amiibo file browser");
        string_set_str(app->current_file, cache->current_file);
        weak_up_set_lcdled(true);
        app->reload_amiibo_files = true;
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_DETAIL);
    }
}

void amiibo_scene_file_browser_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}