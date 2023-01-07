#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vfs.h"

enum storage_list_menu_t {
    STORAGE_LIST_MENU_MOUNT,
    STORAGE_LIST_MENU_FORMAT,
    STORAGE_LIST_MENU_BACK,
};

static void amiibo_scene_storage_list_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                       mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
    switch (index) {

    case STORAGE_LIST_MENU_FORMAT: {
        int32_t res = p_driver->format();
        if (res == VFS_OK) {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    } break;

    case STORAGE_LIST_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void amiibo_scene_storage_list_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
    vfs_stat_t stat;
    int32_t res = p_driver->stat(&stat);
    char txt[64];
    if (res == VFS_OK) {
        mui_list_view_add_item(app->p_list_view, 0xe1ca, stat.avaliable ? "=====已挂载=====" : "=====未挂载=====", (void *)-1);
        if (stat.avaliable) {
            snprintf(txt, sizeof(txt), "总空间: %d kB", stat.total_bytes / 1024);
            mui_list_view_add_item(app->p_list_view, 0xe1cb, txt, (void *)-1);
            snprintf(txt, sizeof(txt), "可用空间: %d kB", stat.free_bytes / 1024);
            mui_list_view_add_item(app->p_list_view, 0xe1cc, txt, (void *)-1);
        }
    }

    mui_list_view_add_item(app->p_list_view, 0xe1cd, "格式化", (void *)STORAGE_LIST_MENU_FORMAT);

    mui_list_view_add_item(app->p_list_view, 0xe069, "返回列表", (void *)STORAGE_LIST_MENU_BACK);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_storage_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_storage_list_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}