#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "vfs.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb

#define FOLDER_LIST_PARENT 0xFFFF

static void amiibo_scene_file_brwser_reload_folders(app_amiibo_t *app) {

    char breadcrumb[VFS_MAX_PATH_LEN + 3];
    vfs_driver_t *p_vfs_driver;
    vfs_dir_t dir;
    vfs_obj_t obj;

    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, "..", (void *)FOLDER_LIST_PARENT);

    p_vfs_driver = vfs_get_driver(app->current_drive);

    int32_t res = p_vfs_driver->open_dir(string_get_cstr(app->current_folder), &dir);
    if (res == VFS_OK) {
        while (res = p_vfs_driver->read_dir(&dir, &obj) == VFS_OK) {
            uint16_t icon = obj.type == VFS_TYPE_DIR ? ICON_FOLDER : ICON_FILE;
            mui_list_view_add_item(app->p_list_view, icon, obj.name, (void *)-1);
        }
        p_vfs_driver->close_dir(&dir);
    } else {
        mui_list_view_add_item(app->p_list_view, ICON_ERROR, "打开文件夹失败", (void *)-1);
    }
}

static void amiibo_scene_file_browser_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                  mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t idx = (uint32_t)p_item->user_data;
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (idx == FOLDER_LIST_PARENT) {
            // mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_STORAGE_LIST);
        } else {
            if (!string_end_with_str_p(app->current_folder, "/")) {
                string_cat_str(app->current_folder, "/");
            }
            string_cat(app->current_folder, p_item->text);
            amiibo_scene_file_brwser_reload_folders(app);
        }
    } else {
        string_set(app->current_folder, p_item->text);
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST_MENU);
    }
}

void amiibo_scene_file_browser_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_file_browser_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    amiibo_scene_file_brwser_reload_folders(app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_file_browser_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}